try {
    $url = 'https://raw.githubusercontent.com/Marcopit92/Flipper-zero/refs/heads/main/dw_scripts/auto_pair_badkb.ps1'
    $response = Invoke-WebRequest -Uri $url -UseBasicParsing
    Write-Host "Status Code: $($response.StatusCode)"
    Write-Host "Content Length: $($response.Content.Length)"
    Write-Host "First 5 lines of content:"
    $lines = $response.Content -split '\n'
    for ($i = 0; $i -lt [Math]::Min(5, $lines.Length); $i++) {
        Write-Host $lines[$i]
    }
    if ($response.Content -match 'param\(') {
        Write-Host "Content appears to be a valid PowerShell script."
    } else {
        Write-Host "Content does not appear to be a valid PowerShell script."
    }
} catch {
    Write-Host "Error: $_"
}
