# PowerShell script to scan for Bluetooth devices starting with "BadKB" and pair/connect to the first match
# Requires Windows 10/11 with Bluetooth support and .NET Framework

Add-Type -AssemblyName System.Runtime.WindowsRuntime
$asTaskGeneric = ([System.WindowsRuntimeSystemExtensions].GetMethods() | ? { $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncOperation`1' })[0]
Function Await($WinRtTask, $ResultType) {
    $asTask = $asTaskGeneric.MakeGenericMethod($ResultType)
    $netTask = $asTask.Invoke($null, @($WinRtTask))
    $netTask.Wait(-1) | Out-Null
    $netTask.Result
}
Function AwaitAction($WinRtAction) {
    $asTask = ([System.WindowsRuntimeSystemExtensions].GetMethods() | ? { $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and !$_.IsGenericMethod })[0]
    $netTask = $asTask.Invoke($null, @($WinRtAction))
    $netTask.Wait(-1) | Out-Null
}

[Windows.Devices.Bluetooth.BluetoothAdapter, Windows.Devices.Bluetooth, ContentType = WindowsRuntime] | Out-Null
[Windows.Devices.Bluetooth.BluetoothDevice, Windows.Devices.Bluetooth, ContentType = WindowsRuntime] | Out-Null
[Windows.Devices.Bluetooth.BluetoothLEDevice, Windows.Devices.Bluetooth, ContentType = WindowsRuntime] | Out-Null

$adapter = Await ([Windows.Devices.Bluetooth.BluetoothAdapter]::GetDefaultAsync()) ([Windows.Devices.Bluetooth.BluetoothAdapter])
if (!$adapter) {
    Write-Host "No Bluetooth adapter found."
    exit
}

$devices = Await ([Windows.Devices.Bluetooth.BluetoothDevice]::GetDeviceSelector()) ([string])
$deviceList = Await ([Windows.Devices.Enumeration.DeviceInformation]::FindAllAsync($devices)) ([Windows.Devices.Enumeration.DeviceInformationCollection])

$targetDevice = $null
foreach ($device in $deviceList) {
    if ($device.Name -like "BadKB*") {
        $targetDevice = $device
        break
    }
}

if (!$targetDevice) {
    Write-Host "No device starting with 'BadKB' found."
    exit
}

$btDevice = Await ([Windows.Devices.Bluetooth.BluetoothDevice]::FromIdAsync($targetDevice.Id)) ([Windows.Devices.Bluetooth.BluetoothDevice])
if ($btDevice.ConnectionStatus -eq [Windows.Devices.Bluetooth.BluetoothConnectionStatus]::Connected) {
    Write-Host "Device '$($btDevice.Name)' is already connected."
} else {
    AwaitAction ($btDevice.Pairing.ProtectAgainstManInTheMiddleRequests = $false)
    AwaitAction ($btDevice.Pairing.IOCapabilities = [Windows.Devices.Bluetooth.BluetoothPairingIOCapabilities]::NoInputNoOutput)
    $pairingResult = Await ($btDevice.Pairing.PairAsync()) ([Windows.Devices.Bluetooth.BluetoothPairingResult])
    if ($pairingResult.Status -eq [Windows.Devices.Bluetooth.BluetoothPairingStatus]::Paired) {
        Write-Host "Successfully paired with '$($btDevice.Name)'."
    } else {
        Write-Host "Failed to pair with '$($btDevice.Name)'. Status: $($pairingResult.Status)"
    }
}
