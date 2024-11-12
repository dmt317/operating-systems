#!/bin/bash

echo "Проверка наличия обновлений..."
git fetch origin

LOCAL=$(git rev-parse HEAD)
REMOTE=$(git rev-parse origin/main)

if [ "$LOCAL" == "$REMOTE" ]; then
    echo "Нет новых обновлений. Локальная и удаленная ветка синхронизированы."
    exit 0
fi

echo "Обновления доступны. Загружаем изменения..."
git pull origin main

echo "Проект успешно обновлен!"
exit 0