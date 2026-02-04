#!/bin/bash

# Check if a directory argument is provided
DIR=${1:-.}  # Default to current directory if none provided

# Check if the directory exists
if [ ! -d "$DIR" ]; then
    echo "Error: '$DIR' is not a valid directory."
    exit 1
fi

# Check if 'tree' command is available
if command -v tree >/dev/null 2>&1; then
    # Use tree if installed
    tree "$DIR"
else
    # Fallback using ls recursively
    echo "'tree' command not found. Using recursive 'ls' instead."
    ls -R "$DIR"
fi
