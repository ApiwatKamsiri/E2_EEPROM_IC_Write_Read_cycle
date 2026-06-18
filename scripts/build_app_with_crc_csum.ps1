##############################################################################
# Create Release Package (Folder + Copy + Zip)
# Author: Apiwat Kamsiri
##############################################################################

function Write-Success {
    param([string]$msg)
    Write-Host "[OK] $msg" -ForegroundColor Green
}

function Write-Info {
    param([string]$msg)
    Write-Host "[INFO] $msg" -ForegroundColor Cyan
}

function Write-Error {
    param([string]$msg)
    Write-Host "[ERROR] $msg" -ForegroundColor Red
}

function Write-Step {
    param([string]$msg)
    Write-Host ""
    Write-Host ">>> $msg" -ForegroundColor Yellow
}

##############################################################################
# PATH CONFIGURATION
##############################################################################

Write-Step "Initialize Path"

$SCRIPT_DIR  = Split-Path -Parent $PSCommandPath
$PROJECT_ROOT = Split-Path -Parent $SCRIPT_DIR

$SYSTEM_H_FILE = Join-Path $PROJECT_ROOT "source\system.h"
$WORK_PATH     = Join-Path $PROJECT_ROOT "HardwareDebug"
$DEST_BASE     = Join-Path $PROJECT_ROOT "Output"

Write-Info "PROJECT_ROOT : $PROJECT_ROOT"
Write-Info "SYSTEM_H_FILE: $SYSTEM_H_FILE"
Write-Info "WORK_PATH    : $WORK_PATH"
Write-Info "DEST_BASE    : $DEST_BASE"

if (!(Test-Path $SYSTEM_H_FILE))
{
    Write-Error "system.h not found"
    exit 1
}

##############################################################################
# READ VERSION
##############################################################################

Write-Step "Reading Version Information"

$content = Get-Content $SYSTEM_H_FILE -Raw

$StockNo = $null
$CoreVersion = $null
$ParameterVersion = $null

if ($content -match '#define\s+CoreSoftwareStock\s+(\d+)')
{
    $StockNo = $matches[1]
}

if ($content -match '#define\s+CoreSoftwareVersion\s+(\d+)')
{
    $CoreVersion = $matches[1]
}

if ($content -match '#define\s+ParameterSoftwareVersion\s+(\d+)')
{
    $ParameterVersion = $matches[1]
}

if (($null -eq $StockNo) -or
    ($null -eq $CoreVersion) -or
    ($null -eq $ParameterVersion))
{
    Write-Error "Failed to parse version information from system.h"
    exit 1
}

$CoreVersion = $CoreVersion.PadLeft(2,'0')
$ParameterVersion = $ParameterVersion.PadLeft(2,'0')

$VERSION_STRING = "RF_${StockNo}-V${CoreVersion}_V${ParameterVersion}"

Write-Success "Stock No          : $StockNo"
Write-Success "Core Version      : $CoreVersion"
Write-Success "Parameter Version : $ParameterVersion"
Write-Success "Version String    : $VERSION_STRING"

##############################################################################
# FIND BUILD FILES
##############################################################################

Write-Step "Searching Build Files"

$SourceMot = Get-ChildItem -Path $WORK_PATH -Filter "*.mot" -ErrorAction SilentlyContinue | Select-Object -First 1
$SourceMap = Get-ChildItem -Path $WORK_PATH -Filter "*.map" -ErrorAction SilentlyContinue | Select-Object -First 1

if ($null -eq $SourceMot)
{
    Write-Error "MOT file not found in HardwareDebug"
    exit 1
}

if ($null -eq $SourceMap)
{
    Write-Error "MAP file not found in HardwareDebug"
    exit 1
}

Write-Success "MOT Found : $($SourceMot.Name)"
Write-Success "MAP Found : $($SourceMap.Name)"

##############################################################################
# CREATE PACKAGE
##############################################################################

Write-Step "Creating Package"

$PackageFolderName = "${VERSION_STRING}_PACKSOFT"
$PackageFolderPath = Join-Path $WORK_PATH $PackageFolderName
$ZipPath = Join-Path $DEST_BASE "${VERSION_STRING}_PACKSOFT.zip"

if (Test-Path $ZipPath)
{
    Remove-Item $ZipPath -Force
}

New-Item -ItemType Directory -Path $PackageFolderPath | Out-Null

##############################################################################
# COPY FILES
##############################################################################

$NewMotName = "${VERSION_STRING}.mot"
$NewMapName = "${VERSION_STRING}.map"

Copy-Item `
    $SourceMot.FullName `
    (Join-Path $PackageFolderPath $NewMotName) `
    -Force

Copy-Item `
    $SourceMap.FullName `
    (Join-Path $PackageFolderPath $NewMapName) `
    -Force

Write-Success "Copied MOT"
Write-Success "Copied MAP"

##############################################################################
# README
##############################################################################

$ReadmeText = @"
Build Date : $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")

Stock No   : $StockNo

Core Ver   : $CoreVersion

Param Ver  : $ParameterVersion

Version    : $VERSION_STRING
"@

$ReadmeText | Out-File `
    (Join-Path $PackageFolderPath "README_INFO.txt") `
    -Encoding UTF8

##############################################################################
# ZIP PACKAGE
##############################################################################

Write-Step "Compress ZIP"

Compress-Archive `
    -Path "$PackageFolderPath\*" `
    -DestinationPath $ZipPath `
    -Force

Write-Success "ZIP Created"

##############################################################################
# EXPORT FILES
##############################################################################

#Write-Step "Export Output"

#$TARGET_DIR = Join-Path $DEST_BASE $VERSION_STRING

#if (!(Test-Path $TARGET_DIR))
#{
#    New-Item -ItemType Directory -Path $TARGET_DIR | Out-Null
#}

#Copy-Item `
#    (Join-Path $PackageFolderPath $NewMotName) `
#    (Join-Path $TARGET_DIR $NewMotName) `
#    -Force

#Copy-Item `
#    (Join-Path $PackageFolderPath $NewMapName) `
#    (Join-Path $TARGET_DIR $NewMapName) `
#    -Force

#Write-Success "Files exported"

Copy-Item `
    (Join-Path $PackageFolderPath $NewMotName) `
    (Join-Path $DEST_BASE $NewMotName) `
    -Force

Copy-Item `
    (Join-Path $PackageFolderPath $NewMapName) `
    (Join-Path $DEST_BASE $NewMapName) `
    -Force

##############################################################################
# CLEANUP
##############################################################################

Remove-Item $PackageFolderPath -Recurse -Force

##############################################################################
# DONE
##############################################################################

Write-Host ""
Write-Host "================================================" -ForegroundColor Green
Write-Host " BUILD PACKAGE SUCCESS" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Green
Write-Host ""

Write-Host "Version : $VERSION_STRING"
Write-Host "ZIP     : $ZipPath"
Write-Host "Output  : $TARGET_DIR"