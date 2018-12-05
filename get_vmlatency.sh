#!/bin/sh

SUDO=""
if [ ! -z `id -u` ]; then
    SUDO="sudo"
fi

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    VMLATENCY=vmlatency.ko
    CPU_NAME=`cat /proc/cpuinfo |grep "model name" |uniq |sed s/"^.*: "//`

    function getlog {
        dmesg
    }

    function build {
        make clean
        make
    }

    function load {
        $SUDO /sbin/insmod $VMLATENCY
    }

    function unload {
        $SUDO /sbin/rmmod $VMLATENCY
    }
elif [[ "$OSTYPE" == "darwin"* ]]; then
    VMLATENCY=build/Release/vmlatency.kext
    CPU_NAME=`sysctl -n machdep.cpu.brand_string`

    function getlog {
        $SUDO dmesg
    }

    function build {
        xcodebuild
    }

    function load {
        $SUDO cp -r $1 /tmp/
        $SUDO kextload /tmp/vmlatency.kext
    }

    function unload {
        $SUDO kextunload /tmp/vmlatency.kext
    }
else
    echo "Unsupported OSTYPE $OSTYPE"
    exit 1
fi 

build

[ -e $VMLATENCY ] || { echo "$VMLATENCY does not exist" ; exit 1 ; }

for i in $(seq 1 20)
do
    load $VMLATENCY
    unload $VMLATENCY
done

getlog | grep "\[vmlatency\]" | sed s/^"\[ *"[0-9]*\.[0-9]*"\] "// |sed s/"\[vmlatency\] "// > "$CPU_NAME".txt
