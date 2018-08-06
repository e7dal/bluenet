/*
 * Author: Crownstone Team
 * Copyright: Crownstone (https://crownstone.rocks)
 * Date: Apr 23, 2015
 * License: LGPLv3+, Apache License 2.0, and/or MIT (triple-licensed)
 */

#include "ble/cs_Stack.h"

#include "cfg/cs_Config.h"
#include "ble/cs_Handlers.h"

#include "structs/buffer/cs_MasterBuffer.h"

#if BUILD_MESHING == 1
extern "C" {
	#include <rbc_mesh.h>
}
#endif

extern "C" {
#include "app_util.h"
}

#include <drivers/cs_Storage.h>
#include <storage/cs_Settings.h>
#include "util/cs_Utils.h"
#include <cfg/cs_UuidConfig.h>

#include <events/cs_EventDispatcher.h>
#include <processing/cs_Scanner.h>
#include <processing/cs_Tracker.h>

//#define PRINT_STACK_VERBOSE

// Define test pin to enable gpio debug.
//#define TEST_PIN 19

Stack::Stack() :
	_appearance(BLE_APPEARANCE_GENERIC_TAG), //_clock_source(defaultClockSource),
	_tx_power_level(TX_POWER), _sec_mode({ }),
	_interval(defaultAdvertisingInterval_0_625_ms), _timeout(defaultAdvertisingTimeout_seconds),
	_gap_conn_params( { }),
	_initializedStack(false), _initializedServices(false), _initializedRadio(false), _advertising(false), _scanning(false),
	_conn_handle(BLE_CONN_HANDLE_INVALID),
	_radio_notify(0),
	//_dm_app_handle(0), 
	_dm_initialized(false),
	_lowPowerTimeoutId(NULL),
	_secReqTimerId(NULL),
	_connectionKeepAliveTimerId(NULL),
	_advInterleaveCounter(0),
	_adv_manuf_data(NULL),
	_serviceData(NULL)
{

	_lowPowerTimeoutData = { {0} };
	_lowPowerTimeoutId = &_lowPowerTimeoutData;
	_secReqTimerData = { {0} };
	_secReqTimerId = &_secReqTimerData;
	_connectionKeepAliveTimerData = { {0} };
	_connectionKeepAliveTimerId = &_connectionKeepAliveTimerData;

	//! setup default values.
	memcpy(_passkey, STATIC_PASSKEY, BLE_GAP_PASSKEY_LEN);

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&_sec_mode);

	_gap_conn_params.min_conn_interval = MIN_CONNECTION_INTERVAL;
	_gap_conn_params.max_conn_interval = MAX_CONNECTION_INTERVAL;
	_gap_conn_params.slave_latency = SLAVE_LATENCY;
	_gap_conn_params.conn_sup_timeout = CONNECTION_SUPERVISION_TIMEOUT;
}

Stack::~Stack() {
	shutdown();
}

/*
const nrf_clock_lf_cfg_t Stack::defaultClockSource = { 
	.source        = NRF_CLOCK_LF_SRC_XTAL,        
	.rc_ctiv       = 0,                     
	.rc_temp_ctiv  = 0,                     
	.xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM 
};
*/

/**
 * Called by the Softdevice handler on any BLE event. It is initialized from the SoftDevice using the app scheduler.
 * This means that the callback runs on the main thread. 
 */
extern "C" void ble_evt_dispatch(ble_evt_t* p_ble_evt) {
#if BUILD_MESHING == 1
	if (Settings::getInstance().isSet(CONFIG_MESH_ENABLED)) {
		rbc_mesh_ble_evt_handler(p_ble_evt);
	}
#endif
	Stack::getInstance().on_ble_evt(p_ble_evt);
}

/**
 * Initialize SoftDevice, handlers, and brown-out.
 */
void Stack::init() {
	if (checkCondition(C_STACK_INITIALIZED, false)) return;

	// Check if SoftDevice is already running and disable it in that case (to restart later)
	uint8_t enabled;
	BLE_CALL(sd_softdevice_is_enabled, (&enabled));
	if (enabled) {
		LOGw(MSG_BLE_SOFTDEVICE_RUNNING);
		BLE_CALL(sd_softdevice_disable, ());
	}

	LOGi(MSG_BLE_SOFTDEVICE_INIT);

	// Enable power-fail comparator
	sd_power_pof_enable(true);
	// set threshold value, if power falls below threshold,
	// an NRF_EVT_POWER_FAILURE_WARNING will be triggered.
	sd_power_pof_threshold_set(BROWNOUT_TRIGGER_THRESHOLD);
	
	//_adv_data.adv_data = _advdata;
	//_adv_data.scan_rsp_data = _scanrsp;

	_initializedStack = true;
}

/** Initialize or configure the BLE radio.
 *
 * We are using the S132 softdevice. Online documentation can be found at the Nordic website:
 *
 * - http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.softdevices52/dita/softdevices/s130/s130api.html
 * - http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.s132.api.v6.0.0/modules.html
 * - http://infocenter.nordicsemi.com/pdf/S132_SDS_v6.0.pdf
 *
 * The easiest is to follow the migration documents.
 *
 * - https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v15.0.0/migration.html?cp=4_0_0_1_9
 * - https://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v14.0.0/migration.html?cp=4_0_3_1_7
 *
 * Set BLE parameters now in <third/nrf/sdk_config.h>:
 *   NRF_SDH_BLE_PERIPHERAL_LINK_COUNT = 1
 *   NRF_SDH_BLE_CENTRAL_LINK_COUNT = 0
 */
void Stack::initRadio() {
	ret_code_t err_code;
	if (!checkCondition(C_STACK_INITIALIZED, true)) return;
	if (checkCondition(C_RADIO_INITIALIZED, false)) return;

	nrf_sdh_enable_request(); 

	// Enable BLE stack
	uint32_t ram_start = RAM_R1_BASE;
	uint32_t const conn_cfg_tag = 1;
	err_code = nrf_sdh_ble_default_cfg_set(conn_cfg_tag, &ram_start); 
	if (err_code == NRF_ERROR_NO_MEM) {
		LOGe("Unrecoverable, memory softdevice and app overlaps: %p", ram_start);
		APP_ERROR_CHECK(NRF_ERROR_NO_MEM);
	}
	APP_ERROR_CHECK(err_code);
	if (ram_start != RAM_R1_BASE) {
		LOGw("Application address is too high, memory is unused: %p", ram_start);
	}

	err_code = nrf_sdh_ble_enable(&ram_start);
	APP_ERROR_CHECK(err_code);

	// Version is not saved or shown yet
	ble_version_t version( { });
	version.company_id = 12;
	err_code = sd_ble_version_get(&version);
	APP_ERROR_CHECK(err_code);

	std::string device_name = "noname";
	if (!_device_name.empty()) {
		device_name = _device_name;
	}
	err_code = sd_ble_gap_device_name_set(&_sec_mode, (uint8_t*) device_name.c_str(), device_name.length());
	APP_ERROR_CHECK(err_code);
	
	err_code = sd_ble_gap_appearance_set(_appearance);
	APP_ERROR_CHECK(err_code);

	// BLE address with which we will broadcast, store in object field
	err_code = sd_ble_gap_addr_get(&_connectable_address);
	APP_ERROR_CHECK(err_code);
	err_code = sd_ble_gap_addr_get(&_nonconnectable_address);
	APP_ERROR_CHECK(err_code);
	// have non-connectable address one value higher than connectable one
	_nonconnectable_address.addr[0] += 0x1; 

	_initializedRadio = true;

	updateConnParams();
	updatePasskey();
	updateTxPowerLevel();
}


void Stack::setAppearance(uint16_t appearance) {
	_appearance = appearance;
}

void Stack::setDeviceName(const std::string& deviceName) {
	_device_name = deviceName;
}

void Stack::setClockSource(nrf_clock_lf_cfg_t clockSource) {
	_clock_source = clockSource;
}

void Stack::setAdvertisingInterval(uint16_t advertisingInterval) {
	if (advertisingInterval < 0x0020 || advertisingInterval > 0x4000) {
		LOGw("Invalid advertising interval");
		return;
	}
	_interval = advertisingInterval;
}

void Stack::updateAdvertisingInterval(uint16_t advertisingInterval, bool apply) {
	if (!_initializedRadio) {
		LOGw(FMT_NOT_INITIALIZED, "radio");
		return;
	}
	setAdvertisingInterval(advertisingInterval);
	configureAdvertisementParameters();

	// Apply new interval.
	if (apply && _advertising) {
		stopAdvertising();
		startAdvertising();
	}
}

void Stack::updateDeviceName(const std::string& deviceName) {
	_device_name = deviceName;

	if (!_initializedRadio) {
		return;
	}
	if (!_device_name.empty()) {
		BLE_CALL(sd_ble_gap_device_name_set,
				(&_sec_mode, (uint8_t*) _device_name.c_str(), _device_name.length()));
	} else {
		std::string name = "not set...";
		BLE_CALL(sd_ble_gap_device_name_set,
				(&_sec_mode, (uint8_t*) name.c_str(), name.length()));
	}
}

void Stack::updateAppearance(uint16_t appearance) {
	_appearance = appearance;
	BLE_CALL(sd_ble_gap_appearance_set, (_appearance));
}

void Stack::createCharacteristics() {
	if (!_initializedRadio) {
		LOGw(FMT_NOT_INITIALIZED, "radio");
		return;
	}
	for (Service* svc: _services) {
		svc->createCharacteristics();
	}
}

void Stack::initServices() {
	if (!_initializedRadio) {
		LOGw(FMT_NOT_INITIALIZED, "radio");
		return;
	}
	if (_initializedServices) {
		LOGw(FMT_ALREADY_INITIALIZED, "services");
		return;
	}

	for (Service* svc : _services) {
		svc->init(this);
	}

	_initializedServices = true;
}

// TODO: remove this function
void Stack::startTicking() {
//	LOGi(FMT_START, "ticking");
//	for (Service* svc : _services) {
//		svc->startTicking();
//	}
}

void Stack::stopTicking() {
//	LOGi(FMT_STOP, "ticking");
//	for (Service* svc : _services) {
//		svc->stopTicking();
//	}
}

void Stack::shutdown() {
	stopAdvertising();

	for (Service* svc : _services) {
		svc->stopAdvertising();
	}

	_initializedStack = false;
}

void Stack::addService(Service* svc) {
	//APP_ERROR_CHECK_BOOL(_services.size() < MAX_SERVICE_COUNT);

	_services.push_back(svc);
}

//! accepted values are -40, -30, -20, -16, -12, -8, -4, 0, and 4 dBm
//! Can be done at any moment (also when advertising)
void Stack::setTxPowerLevel(int8_t powerLevel) {
#ifdef PRINT_STACK_VERBOSE
	LOGd(FMT_SET_INT_VAL, "tx power", powerLevel);
#endif

	switch (powerLevel) {
	case -40: break;
//	case -30: break; // -30 is only available on nrf51
	case -20: break;
	case -16: break;
	case -12: break;
	case -8: break;
	case -4: break;
	case 0: break;
	case 4: break;
	default: return;
	}
	if (_tx_power_level != powerLevel) {
		_tx_power_level = powerLevel;
		if (_initializedRadio) {
			updateTxPowerLevel();
		}
	}
}

void Stack::updateTxPowerLevel() {
	uint32_t err_code;
	err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, _tx_power_level, _adv_handle);
	APP_ERROR_CHECK(err_code);
}

void Stack::setMinConnectionInterval(uint16_t connectionInterval_1_25_ms) {
	if (_gap_conn_params.min_conn_interval != connectionInterval_1_25_ms) {
		_gap_conn_params.min_conn_interval = connectionInterval_1_25_ms;
		if (_initializedRadio) {
			updateConnParams();
		}
	}
}

void Stack::setMaxConnectionInterval(uint16_t connectionInterval_1_25_ms) {
	if (_gap_conn_params.max_conn_interval != connectionInterval_1_25_ms) {
		_gap_conn_params.max_conn_interval = connectionInterval_1_25_ms;
		if (_initializedRadio) {
			updateConnParams();
		}
	}
}

void Stack::setSlaveLatency(uint16_t slaveLatency) {
	if ( _gap_conn_params.slave_latency != slaveLatency ) {
		_gap_conn_params.slave_latency = slaveLatency;
		if (_initializedRadio) {
			updateConnParams();
		}
	}
}

void Stack::setConnectionSupervisionTimeout(uint16_t conSupTimeout_10_ms) {
	if (_gap_conn_params.conn_sup_timeout != conSupTimeout_10_ms) {
		_gap_conn_params.conn_sup_timeout = conSupTimeout_10_ms;
		if (_initializedRadio) {
			updateConnParams();
		}
	}
}

void Stack::updateConnParams() {
	BLE_CALL(sd_ble_gap_ppcp_set, (&_gap_conn_params));
}

/** Configure advertisement data according to iBeacon specification.
 *
 * The iBeacon payload has 31 bytes:
 *   4 bytes to define lengths of major, minor, uuid and rssi
 *   3 bytes for major (1 byte for type,  2 bytes for data)
 *   3 bytes for minor (1 byte for type,  2 bytes for data)
 *  17 bytes for uuid  (1 byte for type, 16 bytes for data)
 *   2 bytes for rssi  (1 byte for type,  1 byte  for data)
 *   2 bytes undefined
 * The company identifier has to be set to 0x004C or it will not be recognized as an iBeacon by iPhones.
 */
void Stack::configureIBeaconAdvData(IBeacon* beacon) {

	memset(&_manufac_apple, 0, sizeof(_manufac_apple));
	_manufac_apple.company_identifier = 0x004C; 
	_manufac_apple.data.p_data = beacon->getArray();
	_manufac_apple.data.size = beacon->size();

	memset(&_advdata, 0, sizeof(_advdata));

	_advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	_advdata.p_manuf_specific_data = &_manufac_apple;
}

/** Configurate advertisement data according to Crowstone specification.
 *
 * The Crownstone advertisement payload has 31 bytes:
 *   1 byte  to define length of the data
 *   2 bytes for flags    (1 byte for type,  1 byte  for data)
 *   2 bytes for tx level (1 byte for type,  1 byte  for data)
 *  17 bytes for uuid     (1 byte for type, 16 bytes for data)
 *   7 bytes undefined
 * TODO: TX power is only set here. Why not when configuring as iBeacon?
 */
void Stack::configureBleDeviceAdvData() {

	//! check if (and how many) services where enabled
	uint8_t uidCount = _services.size();
	if (uidCount == 0) {
		LOGw(MSG_BLE_NO_CUSTOM_SERVICES);
	}

	memset(&_advdata, 0, sizeof(_advdata));
	_advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
	_advdata.p_tx_power_level = &_tx_power_level;
}

/** Configurate scan response according to Crowstone specification.
 *
 * The Crownstone scan response payload has 31 bytes:
 *   1 byte  to define length of the data
 *   X bytes for service data (1 byte for type, 29-N bytes for data)
 *   N bytes for name         (1 byte for type,  N-1 bytes for data)
 */
void Stack::configureScanResponse(uint8_t deviceType) {

	uint8_t serviceDataLength = 0;

	memset(&_scanrsp, 0, sizeof(_scanrsp));
	_scanrsp.name_type = BLE_ADVDATA_SHORT_NAME;

	if (_serviceData && deviceType != DEVICE_UNDEF) {
		memset(&_service_data, 0, sizeof(_service_data));

		_service_data.service_uuid = CROWNSTONE_PLUG_SERVICE_DATA_UUID;

		_service_data.data.p_data = _serviceData->getArray();
		_service_data.data.size = _serviceData->getArraySize();

		_scanrsp.p_service_data_array = &_service_data;
		_scanrsp.service_data_count = 1;

		serviceDataLength += 2 + sizeof(_service_data.service_uuid) + _service_data.data.size;
	}

	const uint8_t maxDataLength = 29;
	uint8_t nameLength = maxDataLength - serviceDataLength;
	nameLength = std::min(nameLength, (uint8_t)(getDeviceName().length()));
	_scanrsp.short_name_len = nameLength;
}

void Stack::configureIBeacon(IBeacon* beacon, uint8_t deviceType) {
	LOGi(FMT_BLE_CONFIGURE_AS, "iBeacon");
	configureIBeaconAdvData(beacon);
	configureScanResponse(deviceType);
	setAdvertisementData();
	configureAdvertisementParameters();
}

void Stack::configureBleDevice(uint8_t deviceType) {
	LOGi(FMT_BLE_CONFIGURE_AS, "BleDevice");
	configureBleDeviceAdvData();
	configureScanResponse(deviceType);
	setAdvertisementData();
	configureAdvertisementParameters();
}

void Stack::configureAdvertisementParameters() {
	_adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
	_adv_params.p_peer_addr = NULL;
	_adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;
	_adv_params.interval = _interval;
	_adv_params.duration = _timeout;
}

void Stack::setConnectable() {
	_adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
	uint32_t err_code;
	do {
		err_code = sd_ble_gap_addr_set(&_connectable_address);
	} while (err_code == NRF_ERROR_INVALID_STATE);
	APP_ERROR_CHECK(err_code);
}

void Stack::setNonConnectable() {
	_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
	uint32_t err_code;
	do {
		err_code = sd_ble_gap_addr_set(&_nonconnectable_address);
	} while (err_code == NRF_ERROR_INVALID_STATE);
	APP_ERROR_CHECK(err_code);
}

void Stack::restartAdvertising() {
	if (!_initializedRadio) {
		LOGw(FMT_NOT_INITIALIZED, "radio");
		return;
	}

	if (_advertising) {
		stopAdvertising();
	}

	startAdvertising();
}

void Stack::startAdvertising() {
	if (_advertising) {
		LOGw("invalid state");
		return;
	}

	LOGi(MSG_BLE_ADVERTISING_STARTING);
	
	//uint32_t err_code = ble_advertising_start(_ble_adv_mode);
	uint32_t err_code = sd_ble_gap_adv_start(_adv_handle, _conn_cfg_tag);
	switch (err_code) {
	case NRF_ERROR_BUSY:
		// Docs say: retry again later.
		// Since restartAdvertising() gets called all the time anyway, I guess we don't have to schedule a retry.
		LOGw("adv_start busy");
		break;
	case NRF_ERROR_INVALID_PARAM:
		LOGf(MSG_BLE_ADVERTISEMENT_CONFIG_INVALID);
		APP_ERROR_CHECK(err_code);
		_advertising = true;
		break;
	default:
		APP_ERROR_CHECK(err_code);
		_advertising = true;
	}

	_advertising = true;
}

void Stack::stopAdvertising() {
	if (!_advertising) {
		LOGw("Already not advertising");
		return;
	}

	// This function call can take 31ms
	uint32_t err_code = sd_ble_gap_adv_stop(_adv_handle); 
	// Ignore invalid state error, see: https://devzone.nordicsemi.com/question/80959/check-if-currently-advertising/
	APP_ERROR_CHECK_EXCEPT(err_code, NRF_ERROR_INVALID_STATE);

	LOGi(MSG_BLE_ADVERTISING_STOPPED);

	_advertising = false;
}

/** Update the advertisement package.
 *
 * This function toggles between connectable and non-connectable advertisements. If we are connected or scanning we
 * will not advertise.
 *
 * @param toggle  When false, use connectable, when true toggle between connectable and non-connectable.
 */
void Stack::updateAdvertisement(bool toggle) {
	if (!checkCondition(C_ADVERTISING, true)) return;

	if (isConnected() || isScanning()) {
		return;
	} 

	bool connectable = true;
	if (toggle) {
		connectable = (++_advInterleaveCounter % 2);
	}

	if (connectable) {
		setConnectable();
	} else {
		setNonConnectable();
	}

	restartAdvertising();
}

/** Utility function which logs unexpected state
 * 
 */
bool Stack::checkCondition(condition_t condition, bool expectation) {
	bool field = false;
	switch(condition) {
	case C_ADVERTISING: 
		field = _advertising;
		if (expectation != field) {
			LOGw("Advertising %i", field);
		}
		break;
	case C_STACK_INITIALIZED:
		field = _initializedStack;
		if (expectation != field) {
			LOGw("Stack init %i", field);
		}
		break;
	case C_RADIO_INITIALIZED:
		field = _initializedRadio;
		if (expectation != field) {
			LOGw("Radio init %i", field);
		}
		break;
	}
	return field;
}

void Stack::setAdvertisementData() {
	if (!checkCondition(C_RADIO_INITIALIZED, true)) return;

	uint32_t err;
	err = sd_ble_gap_adv_set_configure(&_adv_handle, &_adv_data, &_adv_params);
//	err = ble_advdata_set(&_advdata, &_scan_resp);

	//! TODO: why do we allow (and get) invalid configuration?
	//! It seems like we setAdvertisementData before we configure as iBeacon (probably from the service data?)
	switch(err) {
	case NRF_SUCCESS:
		break;
	case NRF_ERROR_INVALID_PARAM:
		LOGw("Invalid advertisement configuration");
		// TODO: why not APP_ERROR_CHECK?
		break;
	default:
		LOGd("Retry setAdvData (err %d)", err);
		BLE_CALL(sd_ble_gap_adv_set_configure, (&_adv_handle, &_adv_data, &_adv_params));
	}
}

void Stack::startScanning() {
	if (!_initializedRadio) {
		LOGw(FMT_NOT_INITIALIZED, "radio");
		return;
	}
	if (_scanning)
		return;

	LOGi(FMT_START, "scanning");
	ble_gap_scan_params_t p_scan_params;
	p_scan_params.active = 0;
	p_scan_params.timeout = 0x0000;

	Settings::getInstance().get(CONFIG_SCAN_INTERVAL, &p_scan_params.interval);
	Settings::getInstance().get(CONFIG_SCAN_WINDOW, &p_scan_params.window);

	//! todo: which fields to set here?
	// TODO: p_adv_report_buffer
	//BLE_CALL(sd_ble_gap_scan_start, (&p_scan_params));
	_scanning = true;

	EventDispatcher::getInstance().dispatch(EVT_SCAN_STARTED);
}


void Stack::stopScanning() {
	if (!_initializedRadio) {
		LOGw(FMT_NOT_INITIALIZED, "radio");
		return;
	}
	if (!_scanning)
		return;

	LOGi(FMT_STOP, "scanning");
	BLE_CALL(sd_ble_gap_scan_stop, ());
	_scanning = false;

	EventDispatcher::getInstance().dispatch(EVT_SCAN_STOPPED);
}

bool Stack::isScanning() {
	return _scanning;
}

void Stack::setAesEncrypted(bool encrypted) {

	//! set characteristics of all services to encrypted
	for (Service* svc : _services) {
		svc->setAesEncrypted(encrypted);
	}
}

void Stack::setPasskey(uint8_t* passkey) {
#ifdef PRINT_STACK_VERBOSE
	LOGd(FMT_SET_STR_VAL, "passkey", std::string((char*)passkey, BLE_GAP_PASSKEY_LEN).c_str());
#endif

	memcpy(_passkey, passkey, BLE_GAP_PASSKEY_LEN);

	if (_initializedRadio) {
		updatePasskey();
	}
}

void Stack::updatePasskey() {
	if (!_initializedRadio) {
		LOGw(FMT_NOT_INITIALIZED, "radio");
		return;
	}
	ble_opt_t static_pin_option;
	static_pin_option.gap_opt.passkey.p_passkey = _passkey;
	BLE_CALL(sd_ble_opt_set, (BLE_GAP_OPT_PASSKEY, &static_pin_option));
}

void Stack::lowPowerTimeout(void* p_context) {
	LOGw("bonding timeout!");
	((Stack*)p_context)->changeToNormalTxPowerMode();
}

void Stack::changeToLowTxPowerMode() {
	int8_t lowTxPower;
	Settings::getInstance().get(CONFIG_LOW_TX_POWER, &lowTxPower);
	setTxPowerLevel(lowTxPower);
}

void Stack::changeToNormalTxPowerMode() {
	int8_t txPower;
	Settings::getInstance().get(CONFIG_TX_POWER, &txPower);
	setTxPowerLevel(txPower);
}

void Stack::on_ble_evt(ble_evt_t * p_ble_evt) {//, void * p_context) {

	if (_dm_initialized) {
		// Note: peer manager is removed
	}

	switch (p_ble_evt->header.evt_id) {
	case BLE_GAP_EVT_CONNECTED:
		on_connected(p_ble_evt);
		EventDispatcher::getInstance().dispatch(EVT_BLE_CONNECT);
		break;
	case BLE_EVT_USER_MEM_REQUEST: {

		buffer_ptr_t buffer = NULL;
		uint16_t size = 0;
		MasterBuffer::getInstance().getBuffer(buffer, size);

		_user_mem_block.len = size+6;
		_user_mem_block.p_mem = buffer-6;
		BLE_CALL(sd_ble_user_mem_reply, (getConnectionHandle(), &_user_mem_block));

		break;
	}
	case BLE_EVT_USER_MEM_RELEASE:
		//! nothing to do
		break;
	case BLE_GAP_EVT_DISCONNECTED:
		on_disconnected(p_ble_evt);
		EventDispatcher::getInstance().dispatch(EVT_BLE_DISCONNECT);
		break;
	case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
	case BLE_GATTS_EVT_TIMEOUT:
	case BLE_GAP_EVT_RSSI_CHANGED:
		for (Service* svc : _services) {
			svc->on_ble_event(p_ble_evt);
		}
		break;
	case BLE_GATTS_EVT_WRITE: {
		resetConnectionAliveTimer();

		if (p_ble_evt->evt.gatts_evt.params.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) {

			buffer_ptr_t buffer = NULL;
			uint16_t size = 0;

			// we want to have the actual payload word aligned, but header only needs 6 bytes,
			// so we need to skip the first 2 bytes
			MasterBuffer::getInstance().getBuffer(buffer, size, 2);

			uint16_t* header = (uint16_t*)buffer;

			for (Service* svc : _services) {
				// for a long write, don't have the service handle available to check for the correct
				// service, so we just go through all the services and characteristics until we find
				// the correct characteristic, then we return
				if (svc->on_write(p_ble_evt->evt.gatts_evt.params.write, header[0])) {
					return;
				}
			}

		} else {
			for (Service* svc : _services) {
				svc->on_write(p_ble_evt->evt.gatts_evt.params.write, p_ble_evt->evt.gatts_evt.params.write.handle);
			}
		}

		break;
	}

	case BLE_GATTS_EVT_HVC:
		for (Service* svc : _services) {
			// TODO: this check is probably be wrong
			if (svc->getHandle() == p_ble_evt->evt.gatts_evt.params.write.handle) {
				svc->on_ble_event(p_ble_evt);
				return;
			}
		}
		break;

	case BLE_GATTS_EVT_SYS_ATTR_MISSING:
		BLE_CALL(sd_ble_gatts_sys_attr_set, (_conn_handle, NULL, 0,
                BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS | BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS));
		break;

	case BLE_GAP_EVT_PASSKEY_DISPLAY: {
#ifdef PRINT_STACK_VERBOSE
		LOGd("PASSKEY: %.6s", p_ble_evt->evt.gap_evt.params.passkey_display.passkey);
#endif
		break;
	}

	case BLE_GAP_EVT_ADV_REPORT:
		EventDispatcher::getInstance().dispatch(EVT_BLE_EVENT, p_ble_evt, -1);
		break;
	case BLE_GAP_EVT_TIMEOUT:
		LOGw("Timeout!");
		// BLE_GAP_TIMEOUT_SRC_ADVERTISING does not exist anymore...
//		if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING) {
//			_advertising = false; 
//			//! Advertising stops, see: https://devzone.nordicsemi.com/question/80959/check-if-currently-advertising/
//		}
		break;

	case BLE_GATTS_EVT_HVN_TX_COMPLETE:
		onTxComplete(p_ble_evt);
		break;

	default:
		break;

	}
}

#if CONNECTION_ALIVE_TIMEOUT>0
static void connection_keep_alive_timeout(void* p_context) {
	LOGw("connection keep alive timeout!");
	Stack::getInstance().disconnect();
}
#endif

void Stack::startConnectionAliveTimer() {
#if CONNECTION_ALIVE_TIMEOUT>0
	Timer::getInstance().createSingleShot(_connectionKeepAliveTimerId, connection_keep_alive_timeout);
	Timer::getInstance().start(_connectionKeepAliveTimerId, MS_TO_TICKS(CONNECTION_ALIVE_TIMEOUT), NULL);
#endif
}

void Stack::stopConnectionAliveTimer() {
#if CONNECTION_ALIVE_TIMEOUT>0
	Timer::getInstance().stop(_connectionKeepAliveTimerId);
#endif
}

void Stack::resetConnectionAliveTimer() {
#if CONNECTION_ALIVE_TIMEOUT>0
	Timer::getInstance().reset(_connectionKeepAliveTimerId, MS_TO_TICKS(CONNECTION_ALIVE_TIMEOUT), NULL);
#endif
}

void Stack::on_connected(ble_evt_t * p_ble_evt) {
	//ble_gap_evt_connected_t connected_evt = p_ble_evt->evt.gap_evt.params.connected;
	_advertising = false; //! Advertising stops on connect, see: https://devzone.nordicsemi.com/question/80959/check-if-currently-advertising/
	_disconnectingInProgress = false;
	BLE_CALL(sd_ble_gap_conn_param_update, (p_ble_evt->evt.gap_evt.conn_handle, &_gap_conn_params));

//	ble_gap_addr_t* peerAddr = &p_ble_evt->evt.gap_evt.params.connected.peer_addr;
//	LOGi("Connection from: %02X:%02X:%02X:%02X:%02X:%02X", peerAddr->addr[5],
//	                       peerAddr->addr[4], peerAddr->addr[3], peerAddr->addr[2], peerAddr->addr[1],
//	                       peerAddr->addr[0]);

	if (_callback_connected) {
		_callback_connected(p_ble_evt->evt.gap_evt.conn_handle);
	}
	_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
	for (Service* svc : _services) {
		svc->on_ble_event(p_ble_evt);
	}

	startConnectionAliveTimer();
}

void Stack::on_disconnected(ble_evt_t * p_ble_evt) {
	//ble_gap_evt_disconnected_t disconnected_evt = p_ble_evt->evt.gap_evt.params.disconnected;
	_conn_handle = BLE_CONN_HANDLE_INVALID;
	if (_callback_connected) {
		_callback_disconnected(p_ble_evt->evt.gap_evt.conn_handle);
	}
	for (Service* svc : _services) {
		svc->on_ble_event(p_ble_evt);
	}

	stopConnectionAliveTimer();
}

void Stack::disconnect() {
	//! Only disconnect when we are actually connected to something
	if (_conn_handle != BLE_CONN_HANDLE_INVALID && _disconnectingInProgress == false) {
		_disconnectingInProgress = true;
		LOGi("Forcibly disconnecting from device");
		//! It seems like we're only allowed to use BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION.
		//! This sometimes gives us an NRF_ERROR_INVALID_STATE (disconnection is already in progress)
		//! NRF_ERROR_INVALID_STATE can safely be ignored, see: https://devzone.nordicsemi.com/question/81108/handling-nrf_error_invalid_state-error-code/
		uint32_t errorCode = sd_ble_gap_disconnect(_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		if (errorCode != NRF_ERROR_INVALID_STATE) {
			APP_ERROR_CHECK(errorCode);
		}
	}
}

bool Stack::isDisconnecting() {
		return _disconnectingInProgress;
};

bool Stack::isConnected() {
	return _conn_handle != BLE_CONN_HANDLE_INVALID;
}

void Stack::onTxComplete(ble_evt_t * p_ble_evt) {
	for (Service* svc: _services) {
		svc->onTxComplete(&p_ble_evt->evt.common_evt);
	}
}



