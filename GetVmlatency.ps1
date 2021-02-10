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
#>

Param (
    [ValidateSet("build", "measure")][String]$Command = "measure",
    [Switch]$UseICC=$False
)

$VmlatencyRoot = Split-Path -Parent -Path $Script:MyInvocation.MyCommand.Definition
$WDKRoot = "C:\WinDDK\7600.16385.1"

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

SetupBuildEnvironment

Build vmm
Build win

If ($Command -Eq "measure") {
    Load-Vmlatency
    Unload-Vmlatency
}
