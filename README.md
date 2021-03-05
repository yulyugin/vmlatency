# vmlatency
Simple code to demonstrate how one can use Intel(R) VT-x technology to setup Virtual Machine Control Structure (VMCS) and launch virtual machine. Virtual machine state mirrors host state.

The tool measures VM-Entry and VM-Exit turnaroound time in cycles. RDTSC instruction is used to do the measurements.

## Results

![VMlatency](/results/vmlatency.png)

## Running on Linux
Superuser access is required to run the tool.

    $ ./get_vmlatency.sh

### Requirements
1. gcc
2. kernel headers
3. virtualization must be enabled in BIOS.

## Running on Windows
**NOTE:** Hyper-V has to be disabled to run the tools.

    > .\GetVmltency.ps1

### Requirements
1. [Windows Driver Kit Version 7.1.0](https://download.microsoft.com/download/4/A/2/4A25C7D5-EFBE-4182-B6A9-AE6850409A78/GRMWDK_EN_7600_1.ISO)

### Self signing
Windows does not allow to load unsigned drivers so one have to setup self
signing to run vmlatency. "set-testsigning" and "clean-testsigning" commands of
*GetVmlatency.ps1* script to enable and cleanup testsigning.
