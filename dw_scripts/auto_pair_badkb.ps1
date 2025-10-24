# ===============================================
# auto_pair_badkb.ps1
# Pair automatico con dispositivo Bluetooth che inizia per "BadKB"
# ===============================================

param(
    [string]$Prefix = "BadKB",
    [int]$UiWaitMs = 800
)

Add-Type -AssemblyName UIAutomationClient
Add-Type -AssemblyName UIAutomationTypes
Add-Type -AssemblyName System.Windows.Forms

function Log([string]$msg) {
    Write-Host "$(Get-Date -Format 'HH:mm:ss')  $msg"
}

function Invoke-Click($element) {
    if (-not $element) { return $false }
    try {
        $p = $null
        if ($element.TryGetCurrentPattern([System.Windows.Automation.InvokePattern]::Pattern, [ref]$p)) {
            $p.Invoke()
            Start-Sleep -Milliseconds $UiWaitMs
            return $true
        }

        $bounds = $element.Current.BoundingRectangle
        if ($bounds.Width -gt 0 -and $bounds.Height -gt 0) {
            $x = [int]($bounds.X + $bounds.Width / 2)
            $y = [int]($bounds.Y + $bounds.Height / 2)
            [System.Windows.Forms.Cursor]::Position = New-Object System.Drawing.Point($x, $y)
            Start-Sleep -Milliseconds 200
            Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public class Mouse {
    [DllImport("user32.dll", CharSet=CharSet.Auto, CallingConvention=CallingConvention.StdCall)]
    public static extern void mouse_event(long dwFlags, long dx, long dy, long cButtons, long dwExtraInfo);
}
"@
            [Mouse]::mouse_event(0x02, 0, 0, 0, 0)
            [Mouse]::mouse_event(0x04, 0, 0, 0, 0)
            Start-Sleep -Milliseconds $UiWaitMs
            return $true
        }
    } catch {
        Log "Invoke-Click errore: $_"
    }
    return $false
}

function Find-ElementLike($parent, $prefix) {
    $walker = New-Object System.Windows.Automation.TreeWalker([System.Windows.Automation.Condition]::TrueCondition)
    $element = $walker.GetFirstChild($parent)
    while ($element) {
        if ($element.Current.Name -match "^$prefix") {
            return $element
        }
        $child = Find-ElementLike $element $prefix
        if ($child) { return $child }
        $element = $walker.GetNextSibling($element)
    }
    return $null
}

# === Avvio ===
Log "=== Avvio auto_pair_badkb (prefisso: $Prefix) ==="
Start-Process "ms-settings:bluetooth"
Start-Sleep 3

$desktop = [System.Windows.Automation.AutomationElement]::RootElement
$settingsWin = $null
for ($i = 0; $i -lt 10; $i++) {
    $cond = New-Object System.Windows.Automation.PropertyCondition `
        ([System.Windows.Automation.AutomationElement]::NameProperty, "Impostazioni")
    $settingsWin = $desktop.FindFirst([System.Windows.Automation.TreeScope]::Children, $cond)
    if ($settingsWin) { break }
    Start-Sleep 1
}

if (-not $settingsWin) {
    Log "ERRORE: finestra Impostazioni non trovata."
    exit
}

Log "Finestra Impostazioni trovata, avvio pairing..."

$stopwatch = [Diagnostics.Stopwatch]::StartNew()
while ($stopwatch.Elapsed.TotalSeconds -lt 90) {

    $addBtn = $settingsWin.FindFirst(
        [System.Windows.Automation.TreeScope]::Descendants,
        (New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, "Aggiungi dispositivo"))
    )
    if ($addBtn) {
        Log "Clicco 'Aggiungi dispositivo'..."
        Invoke-Click $addBtn
        Start-Sleep 3

        # Mostra tutti i dispositivi
        $showAll = $settingsWin.FindFirst(
            [System.Windows.Automation.TreeScope]::Descendants,
            (New-Object System.Windows.Automation.OrCondition(
                (New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, "Mostra tutti i dispositivi")),
                (New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, "Show all devices"))
            ))
        )
        if ($showAll) {
            Log "Clicco 'Mostra tutti i dispositivi'..."
            Invoke-Click $showAll
            Start-Sleep 2
        }

        # Bluetooth
        $btBtn = $settingsWin.FindFirst(
            [System.Windows.Automation.TreeScope]::Descendants,
            (New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, "Bluetooth"))
        )
        if ($btBtn) {
            Log "Clicco 'Bluetooth'..."
            Invoke-Click $btBtn
            Start-Sleep 3
        }

        # Cerca dispositivo
        Log "Cerco dispositivo che inizia per '$Prefix'..."
        $device = Find-ElementLike $settingsWin $Prefix
        if ($device) {
            Log "Trovato dispositivo '$($device.Current.Name)'. Clicco..."
            Invoke-Click $device
            Start-Sleep 4

            # Ora cerchiamo "Connetti"
            Log "Attendo pulsante 'Connetti'..."
            $connectBtn = $null
            for ($c = 0; $c -lt 20; $c++) {
                $connectBtn = $settingsWin.FindFirst(
                    [System.Windows.Automation.TreeScope]::Descendants,
                    (New-Object System.Windows.Automation.OrCondition(
                        (New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, "Connetti")),
                        (New-Object System.Windows.Automation.PropertyCondition([System.Windows.Automation.AutomationElement]::NameProperty, "Connect"))
                    ))
                )
                if ($connectBtn) { break }
                Start-Sleep 1
            }

            if ($connectBtn) {
                Log "Trovato pulsante 'Connetti'. Clicco..."
                Invoke-Click $connectBtn
                Log "Pairing avviato correttamente!"
            } else {
                Log "ATTENZIONE: 'Connetti' non trovato entro 20 secondi."
            }
            break
        } else {
            Log "Nessun device '$Prefix' trovato, riprovo..."
            Start-Sleep 5
        }
    } else {
        Log "Bottone 'Aggiungi dispositivo' non trovato, ritento..."
        Start-Sleep 2
    }
}

$stopwatch.Stop()
if ($stopwatch.Elapsed.TotalSeconds -ge 90) {
    Log "Timeout raggiunto: pairing non completato."
} else {
    Log "Pairing completato o in corso."
}

# Chiudi finestra Impostazioni
try {
    $closePattern = $settingsWin.GetCurrentPattern([System.Windows.Automation.WindowPattern]::Pattern)
    $closePattern.Close()
} catch {
    Log "Errore nella chiusura della finestra: $_"
}

Log "=== Script terminato ==="
