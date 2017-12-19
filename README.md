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

### Requirements
1. [Windows Driver Kit Version 7.1.0](https://download.microsoft.com/download/4/A/2/4A25C7D5-EFBE-4182-B6A9-AE6850409A78/GRMWDK_EN_7600_1.ISO)

### Self signing
Windows does not allow to load unsigned drivers so one have to setup self signing to run the tool.
The instructions below are based on VirtualBox's [wiki page](https://www.virtualbox.org/wiki/Windows%20build%20instructions) and on Windows's [TechNet article](https://technet.microsoft.com/en-us/library/dd919238(v=ws.10).aspx)

All pathes are specified for Windows Driver Kit Version 7.1.0.

#### Creating and installing a test certificate

1. Launch administrator command prompt
2. Create the test certifiate
```
C:\WinDDK\7600.16385.1\bin\amd64\MakeCert.exe -r -pe -ss my -n "CN=MyTestCertificate" mytestcert.cer
```
3. Register the certificate in your system
```
C:\WinDDK\7600.16385.1\bin\amd64\CertMgr.exe -add mytestcert.cer -s -r localMachine root
```
4. Keep mytestcert.cer in a safe place

#### Configuring the system to run test signed drivers
1. Launch administrator command prompt
2. Enable testing mode in Windows registry
```
Bcdedit.exe -set TESTSIGNING ON
```
3. ```C:\WinDDK\7600.16385.1\bin\amd64\CertMgr.exe -add mytestcert.cer -s -r localMachine root```
4. ```C:\WinDDK\7600.16385.1\bin\amd64\CertMgr.exe -add mytestcert.cer -s -r localMachine trustedpublisher```
5. Reboot the system
6. After reboot you will see ```"Test Mode<CR>Windows 7<CR>Build 7600"``` in the right left corner if you use Windows 7

#### Signing the driver
```C:\WinDDK\7600.16385.1\bin\amd64\SignTool.exe sign /f <path to>\mytestcert.cer build-win7-fre\amd64\vmlatency.sys```
