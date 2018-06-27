#!/bin/sh

xcodebuild

VMLATENCY_KEXT=build/Release/vmlatency.kext

[ -e $VMLATENCY_KEXT ] || { echo "$VMLATENCY_KEXT does not exist" ; exit 1 ; }

SUDO=""
if [ ! -z `id -u` ]; then
    SUDO="sudo"
fi

$SUDO cp -r $VMLATENCY_KEXT /tmp/

$SUDO kextload /tmp/vmlatency.kext
$SUDO kextunload /tmp/vmlatency.kext

cpu_name=`sysctl -n machdep.cpu.brand_string`

$SUDO dmesg |grep "\[vmlatency\]" >> "$cpu_name".txt
