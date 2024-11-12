@echo off

echo Проверка наличия обновлений...
git fetch origin

for /f "delims=" %%i in ('git rev-parse HEAD') do set LOCAL=%%i
for /f "delims=" %%i in ('git rev-parse origin/main') do set REMOTE=%%i

if "%LOCAL%"=="%REMOTE%" (
    echo Нет новых обновлений. Локальная и удаленная ветка синхронизированы.
    exit /b 0
)

echo Обновления доступны. Загружаем изменения...
git pull origin main

echo Проект успешно обновлен!
exit /b 0