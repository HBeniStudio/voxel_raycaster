$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $root 'build'
$lastTrigger = [datetime]::MinValue
$buildLock = $false

function Initialize-VisualStudioEnvironment {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) {
        $vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
    }

    if (-not (Test-Path $vswhere)) {
        return
    }

    $vsInstallPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vsInstallPath) {
        return
    }

    $vcvars = Join-Path $vsInstallPath 'VC\Auxiliary\Build\vcvars64.bat'
    if (-not (Test-Path $vcvars)) {
        return
    }

    $envOutput = & cmd /c "call `"$vcvars`" x64 >NUL && set"
    foreach ($line in $envOutput) {
        if ($line -match '^([^=]+)=(.*)$') {
            $name = $matches[1]
            $value = $matches[2]
            Set-Item -Path "Env:$name" -Value $value
        }
    }
}

Initialize-VisualStudioEnvironment

function Invoke-ProjectBuild {
    param(
        [string]$TriggerPath
    )

    $now = Get-Date
    if (($now - $lastTrigger).TotalSeconds -lt 1.5) {
        return
    }

    if ($buildLock) {
        return
    }

    $script:lastTrigger = $now
    $script:buildLock = $true

    try {
        if (Test-Path $buildDir) {
            Remove-Item -Recurse -Force $buildDir
        }

        Write-Host "Build triggered by: $TriggerPath"
        cmake -S $root -B $buildDir -G "Visual Studio 18 2026" -DCMAKE_BUILD_TYPE=Debug
        cmake --build $buildDir --config Debug
        Write-Host 'Build finished successfully.'
    }
    catch {
        Write-Error $_
    }
    finally {
        $script:buildLock = $false
    }
}

$watchers = @()
$watchConfigs = @(
    @{ Path = (Join-Path $root 'src'); IncludeSubdirectories = $true },
    @{ Path = (Join-Path $root 'include'); IncludeSubdirectories = $true },
    @{ Path = (Join-Path $root 'resources'); IncludeSubdirectories = $true },
    @{ Path = (Join-Path $root 'vendors'); IncludeSubdirectories = $true }
)

foreach ($watchConfig in $watchConfigs) {
    $path = $watchConfig.Path
    if (-not (Test-Path $path)) {
        continue
    }

    $watcher = New-Object System.IO.FileSystemWatcher $path
    $watcher.IncludeSubdirectories = $watchConfig.IncludeSubdirectories
    $watcher.EnableRaisingEvents = $true

    Register-ObjectEvent -InputObject $watcher -EventName Changed -Action {
        Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
    } | Out-Null

    Register-ObjectEvent -InputObject $watcher -EventName Created -Action {
        Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
    } | Out-Null

    Register-ObjectEvent -InputObject $watcher -EventName Deleted -Action {
        Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
    } | Out-Null

    Register-ObjectEvent -InputObject $watcher -EventName Renamed -Action {
        Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
    } | Out-Null

    $watchers += $watcher
}

$rootWatcher = New-Object System.IO.FileSystemWatcher $root
$rootWatcher.Filter = 'CMakeLists.txt'
$rootWatcher.EnableRaisingEvents = $true
Register-ObjectEvent -InputObject $rootWatcher -EventName Changed -Action {
    Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
} | Out-Null
Register-ObjectEvent -InputObject $rootWatcher -EventName Created -Action {
    Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
} | Out-Null
Register-ObjectEvent -InputObject $rootWatcher -EventName Deleted -Action {
    Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
} | Out-Null
Register-ObjectEvent -InputObject $rootWatcher -EventName Renamed -Action {
    Invoke-ProjectBuild -TriggerPath $Event.SourceEventArgs.FullPath
} | Out-Null

$watchers += $rootWatcher

Write-Host "Watching $root for changes..."
while ($true) {
    Start-Sleep -Seconds 2
}
