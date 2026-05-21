# ===============================================
# release.ps1 - Windows release packager for CBLT
# Run from a regular PowerShell terminal (NOT Git Bash)
# ===============================================
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# ---------------- CONFIG ----------------
$PROJECT_ROOT = "C:\Users\Peaky\Desktop\GpapPeaky\Code\RUST\rakey\"
$RELEASE_BASE = "$PROJECT_ROOT\release"
$RELEASE_NAME = "cblt-win32-x86_64"
$RELEASE_DIR  = "$RELEASE_BASE\$RELEASE_NAME"
$FOLDERS      = @("log", "assets", "meta", "options")       # folders to include
# ----------------------------------------

Write-Host "=== Starting release packaging ===" -ForegroundColor Cyan

# Clean previous release dir (but not the release root, .dll/.a sit there after make rel)
if (Test-Path $RELEASE_DIR) {
    Remove-Item -Recurse -Force $RELEASE_DIR
}
New-Item -ItemType Directory -Path $RELEASE_DIR | Out-Null

# ---------------- Build ----------------
Write-Host "Building DLL + import lib..."
& make -f "$PROJECT_ROOT\makeWin32.mk" rel
if ($LASTEXITCODE -ne 0) { throw "make rel failed" }

Write-Host "Building executable..."
& make -f "$PROJECT_ROOT\makeWin32.mk" all
if ($LASTEXITCODE -ne 0) { throw "make all failed" }

# ---------------- Copy files ----------------
Write-Host "Copying binary..."
Copy-Item "$PROJECT_ROOT\CoBaLT.exe" "$RELEASE_DIR\"

Write-Host "Copying DLL and import lib..."
foreach ($lib in @("CoBaLT.dll", "libCoBaLT.dll.a")) {
    $src = "$RELEASE_BASE\$lib"
    if (Test-Path $src) {
        Copy-Item $src "$RELEASE_DIR\"
    } else {
        Write-Warning "$lib not found in release\, skipping"
    }
}

Write-Host "Copying resource folders..."
foreach ($d in $FOLDERS) {
    $src = "$PROJECT_ROOT\$d"
    if (Test-Path $src) {
        Copy-Item -Recurse $src "$RELEASE_DIR\"
    } else {
        Write-Warning "$d folder not found, skipping"
    }
}

# ---------------- Package ----------------
$zipPath = "$RELEASE_BASE\$RELEASE_NAME.zip"
Write-Host "Creating zip release..."
if (Test-Path $zipPath) {
    Remove-Item -Force $zipPath
}
Compress-Archive -Path $RELEASE_DIR -DestinationPath $zipPath

Write-Host ""
Write-Host "=== Release complete ===" -ForegroundColor Green
Write-Host "Folder : $RELEASE_DIR"
Write-Host "Archive: $zipPath"
Write-Host ""
Write-Host "Contents of release folder:"
Get-ChildItem -Recurse $RELEASE_DIR | Select-Object FullName