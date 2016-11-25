#!/bin/bash

# optional target, use crownstone as default
target=${1:-crownstone}

path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $path/_utils.sh
source $path/_check_targets.sh $target
source $path/_config.sh

# cd $path/../build/$1
cd $BLUENET_BUILD_DIR

cp prog.map prog.tmp.map
ex -c '%g/\.text\S*[\s]*$/j' -c "wq" prog.map
ex -c '%g/\.rodata\S*[\s]*$/j' -c "wq" prog.map
ex -c '%g/\.data\S*[\s]*$/j' -c "wq" prog.map
ex -c '%g/\.bss\S*[\s]*$/j' -c "wq" prog.map
sed -i 's/\.text\S*/.text/g' prog.map
sed -i 's/\.rodata\S*/.rodata/g' prog.map
sed -i 's/\.data\S*/.data/g' prog.map
sed -i 's/\.bss\S*/.bss/g' prog.map

cd $path/../util/memory

log "Load now file prog.map from build directory in your browser"
gnome-open index.html
