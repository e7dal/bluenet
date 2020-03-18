# Release process

Mostly copied from firmware release process. So before releasing a bootloader check if the firmware release process was updated.

## Bluenet

The release process starts with updating version information. The bootloader version has to be bumped. This is an integer that is checked by the bootloader to make sure that the
bootloader version is actually newer than the previous release.

* Update the `VERSION` file in `source/bootloader/VERSION`.

Then to create a release:

```
cd build
make create_git_release_bootloader
```

This will create a `release/` directory with the version information in a subdirectory, e.g. `release/bootloader_2.0.1-RC0`.
Double check if the CMakeBuild.config is correct. Eventually add a CMakeBuild.config.overwrite file for your own
passkey file.
Now using this information, we will build everything for this release.

```
cd build
cmake -DCONFIG_DIR=release -DBOARD_TARGET=bootloader_2.0.1-RC0 -DCMAKE_BUILD_TYPE=Release ..
make
```

Now we can start generating the DFU packages that will be released:

```
cd bootloader_2.0.1-RC0
make generate_dfu_package_all
make generate_dfu_package_bootloader
make install
```

After make install you will find the `.zip` files in (subdirectories of) `bin/bootloader_2.0.1-RC0`.

Don't forget to commit the release config:

```
git add source/bootloader/VERSION
git add release/bootloader_2.0.1-RC0/CMakeBuild.config
git commit -m "Added bootloader 2.0.1-RC0"
git push
```

And create a git tag:

```
git tag -a -m "Tagging bootloader version 2.0.1-RC0" "bootloader-2.0.1-RC0"
git push --tags
```

## Release repository

After the above, the release `.zip` files are generated, however, they are not yet made available to the public.

There are two repositories that maintain releases.

* https://github.com/crownstone/bluenet-release
* https://github.com/crownstone/bluenet-release-candidate

The parent directory of the repositories is assumed to be the same as the parent directory of the bluenet repository.
This can be adjusted by the CMake variables `RELEASE_REPOSITORY` and `RELEASE_CANDIDATE_REPOSITORY` in the bluenet
project. Depending on the existence of RC (release candidate) version information in the `VERSION` file, the proper
repository will be used. To create a release directory and fill it, call:

```
make create_bootloader_release_in_repository
```

This will copy all `.zip`, `.elf`, `.bin`, files, as well as configuration files.