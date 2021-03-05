<#
.SYNOPSIS

Build vmlatency driver and measure VM-Entry and VM-Exit turnaroound time.

.DESCRIPTION

This scripts builds vmlatency driver and measures VM-Entry and VM-Exit
turnaroound time in cycles.

.PARAMETER Command

Supported commands:

"build"    Build vmlatency kernel driver.

"measure"  Default. Build vmatency kernel driver and measure VM-Entry and
           VM-Exit turnaround time.

"set-testsigning"    Switch current host to test mode and create self-signed
                     certificate.

"clean-testsigning"  Revert changes made by "set-testsigning" command.
#>

Param (
    [ValidateSet("build", "measure", "set-testsigning",
                 "clean-testsigning")][String]$Command = "measure",
    [Switch]$UseICC=$False
)

$VmlatencyRoot = Split-Path -Parent -Path $Script:MyInvocation.MyCommand.Definition
$WDKRoot = "C:\WinDDK\7600.16385.1"
$CertificateSubject = "VmlatencyTestCertificate"

Function Build([String]$Component) {
    Echo "Building Vmlatency ($Component)"

    $Cmds  = "/C"
    $Cmds += " $WDKRoot\bin\setenv.bat $WDKRoot fre x64 WIN7"
    If ($Script:ICCPath) {
        $Cmds += " & set SUBSTITUTE_AMD64_CC=`"$Script:ICCPath`""
        # Disable Inter-Procedural Optimization (IPO). Windows linker does not
        # support link time optimizable objects produced with this optimization
        $Cmds += " & set USER_C_FLAGS=/Qipo-"
    }
    $Cmds += " & cd /d $VmlatencyRoot\$Component"
    $Cmds += " & build /cfFgwb"

    $P = (Start-Process cmd -ArgumentList $Cmds -Wait -NoNewWindow -PassThru)
    If (-Not $P.ExitCode -Eq 0) {
        Echo "*** Failed to build Vmlatency ($Component)"
        Exit 1
    }
}

Function Load-Vmlatency() {
    $Driver = "$VmlatencyRoot\build-win7-fre\amd64\vmlatency.sys"

    # It's impossible to create kernel service using New-Service cmdlet
    $ScArgs = "create vmlatency binPath=$Driver type=kernel"
    $P = (Start-Process sc.exe -ArgumentList $ScArgs -Wait -NoNewWindow -PassThru)
    If (-Not $P.ExitCode -Eq 0) {
        Echo "*** Failed to create vmatency service"
        Exit 1
    }

    $Vmlatency = Get-Service vmlatency
    try {
        $Vmlatency.Start()
    } catch {
        Echo "*** Failed to start vmlatency service"
        Echo "*** Look at system logs for more information"
        Unload-Vmlatency
        Exit 1
    }
}

Function Unload-Vmlatency() {
    $Vmlatency = Get-Service vmlatency -ErrorAction SilentlyContinue
    If (-Not $Vmlatency) { Return }

    If ($Vmlatency.Status -eq 'Running') { $Vmlatency.Stop() }

    # Remove-Service cmdlet is available starting with PowerShell 6.0
    $ScArgs = "delete vmlatency"
    $P = (Start-Process sc.exe -ArgumentList $ScArgs -Wait -NoNewWindow -PassThru)
    If (-Not $P.ExitCode -Eq 0) {
        Echo "*** Failed to delete vmlatency service"
        Exit 1
    }
}

Function SetupBuildEnvironment() {
    If (-Not (Test-Path $WDKRoot)) {
        Echo "*** Windows Driver Kit Version 7 is not found"
        Exit 1
    }
    Echo "Using WDK='$WDKRoot'"

    If ($UseICC) {
        ForEach ($ICC in Get-Item Env:ICPP_COMPILER*) {
            If ($LatestICC.name -lt $ICC.name) { $LatestICC = $ICC }
        }

        If (-Not $LatestICC) {
            Echo "ICC not found"
            Exit 1
        }

        $Script:ICCPath = $LatestICC.value + "\bin\intel64\icl.exe"
        Echo "Using ICC='$Script:ICCPath'"
    }
}

Function Sign([String]$File) {
    $SignTool = "$WDKRoot\bin\amd64\SignTool.exe"
    $Args =  " sign /t http://timestamp.digicert.com"
    $Args += " /n $CertificateSubject"
    $Args += " $File"

    Echo "Signing $File"
    $P = (Start-Process $SignTool -ArgumentList $Args -NoNewWindow -PassThru)
    $P.WaitForExit()
    If ($P.ExitCode -Eq 1) {
       Echo "*** Failed to sign $File"
       Exit 1
    }
}

Function SetupSelfSigning() {
    Foreach ($c in (Get-ChildItem -Path "Cert:\CurrentUser\My")) {
        If ($c.Subject -Eq "CN=$CertificateSubject") {
            $t = $c.Thumbprint;
            Break
        }
    }

    If ($t) {
        Echo "Self-signed certificate for vmlatency $t already exists."
    } Else {
        $Param = @{
            'CertStoreLocation' = "Cert:\CurrentUser\My"
            'Subject' = "CN=$CertificateSubject"
            'Type' = "CodeSigningCert"
        }
        $c = New-SelfSignedCertificate @Param
        If (-Not $c) {
            Echo "Failed to create self-signed certificate."
            Exit 1
        }
        Echo ("Self-signed certificate " + $c.Thumbprint + " created.")
    }

    SetTestsigning "ON"
}

Function CleanupSelfSigning() {
    SetTestsigning "OFF"

    Foreach ($c in (Get-ChildItem -Path "Cert:\" -Recurse)) {
        If ($c.Subject -Eq "CN=VmlatencyTestCertificate") { Rm $c.PsPath }
    }
}

Function SetTestsigning($Value) {
    $CheckTestSigningCmd = "bcdedit.exe /v | Select-String testsigning"
    $CheckTestSigningCmd += " | Select-String Yes"
    $SetTestSigningCmd = "bcdedit.exe /set testsigning $Value"

    $ExitUnchanged = 104
    $ExitReboot = 1077
    $Change = "{ $SetTestSigningCmd; Exit $ExitReboot }"
    $AlreadySet = "{ Exit $ExitUnchanged }"

    If ($Value -Eq "ON") {
        $Cmd = "If ($CheckTestSigningCmd) $AlreadySet Else $Change"
    } ElseIf ($Value -Eq "OFF") {
        $Cmd = "If ($CheckTestSigningCmd) $Change Else $AlreadySet"
    } Else {
        Echo "Unsupported testsigning value $Value."
        Exit 1
    }

    $Param = @{
        'Verb' = 'runAs'
        'ArgumentList' = "$Cmd"
        'WindowStyle' = 'Hidden'
    }
    $p = (Start-Process powershell @Param -PassThru)
    $p.WaitForExit()

    If (-Not $p){
        Echo "Failed to elevate"
        Exit 0
    }

    If ($p.ExitCode -Eq $ExitUnchanged) {
        Echo "Testsigning mode is already $Value."
    } ElseIf ($p.ExitCode -Eq $ExitReboot) {
        Echo "Testsigning mode changed to $Value."
        Echo "Reboot is required for the changes to take effect."
    } Else {
        Echo ("Failed to set testsigning to $Value. ExitCode: " + $p.ExitCode)
        Exit 1
    }
}

Switch ($Command) {
    "set-testsigning" { SetupSelfSigning }
    "clean-testsigning" { CleanupSelfSigning }
    Default {
        SetupBuildEnvironment

        Build vmm
        Build win

        If ($Command -Eq "measure") {
            Sign "$VmlatencyRoot\build-win7-fre\amd64\vmlatency.sys"
            Load-Vmlatency
            Unload-Vmlatency
        }
    }
}
