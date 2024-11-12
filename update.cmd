@echo off

echo Checking for updates...
git fetch origin

for /f "delims=" %%i in ('git rev-parse HEAD') do set LOCAL=%%i
for /f "delims=" %%i in ('git rev-parse origin/main') do set REMOTE=%%i

if "%LOCAL%"=="%REMOTE%" (
    echo There are no new updates. The local and remote branches are synchronized.
    exit /b 0
)

echo Updates are available. Uploading the changes...
git pull origin main

echo The project has been successfully updated!
exit /b 0