#!/bin/bash

echo "Checking for updates..."
git fetch origin

LOCAL=$(git rev-parse HEAD)
REMOTE=$(git rev-parse origin/main)

if [ "$LOCAL" == "$REMOTE" ]; then
    echo "There are no new updates. The local and remote branches are synchronized."
    exit 0
fi

echo "Updates are available. Uploading the changes..."
git pull origin main

echo "The project has been successfully updated!"
exit 0