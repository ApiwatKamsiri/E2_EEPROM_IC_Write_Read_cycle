@echo off
echo ================================================
echo            Build Script Runner
echo ================================================
echo.

REM --- Run PowerShell using absolute system path ---
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" ^
-NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass ^
-File "%~dp0build_app_with_crc_csum.ps1"

echo.
echo ================================================
echo              BUILD FINISHED
echo ================================================