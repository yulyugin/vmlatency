# vmlatency
Simple code to demonstrate how one can use Intel(R) VT-x technology to setup Virtual Machine Control Structure (VMCS) and launch virtual machine. Virtual machine state mirrors host state.

The tool also measures VM-Entry and VM-Exit turnaroound time in cycles. RDTSC instruction is used to do the measurements.

## Requirements
1. gcc
2. kernel headers
3. virtualization must be enabled in BIOS.
