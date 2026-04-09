#!/bin/bash
set -e

# ---------------- CONFIG ----------------
PROJECT_ROOT="$HOME/Desktop/GpapPeaky/DEV/Co.Ba.L.T"  # adjust to your project path
RELEASE_BASE="$PROJECT_ROOT/release"
RELEASE_NAME="cblt-release"
RELEASE_DIR="$RELEASE_BASE/$RELEASE_NAME"
BIN_NAME="cblt"
FOLDERS=("log" "assets" "meta" "options")  # folders to include
LINK_GLOBAL=true
# ----------------------------------------

echo "=== Starting release packaging ==="

rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"

echo "Building shared library..."
make -f "$PROJECT_ROOT/makeLinux.mk" release

echo "Building executable..."
make -f "$PROJECT_ROOT/makeLinux.mk" all

echo "Copying binary..."
cp "$PROJECT_ROOT/$BIN_NAME" "$RELEASE_DIR/"
chmod +x "$RELEASE_DIR/$BIN_NAME"

echo "Copying folders..."
for d in "${FOLDERS[@]}"; do
    if [ -d "$PROJECT_ROOT/$d" ]; then
        cp -r "$PROJECT_ROOT/$d" "$RELEASE_DIR/"
    else
        echo "Warning: $d folder not found, skipping"
    fi
done

if [ -f "$PROJECT_ROOT/release/libCoBaLT.so" ]; then
    cp "$PROJECT_ROOT/release/libCoBaLT.so" "$RELEASE_DIR/"
fi

RUN_SCRIPT="$RELEASE_DIR/run.sh"
echo "Creating run.sh wrapper..."
cat > "$RUN_SCRIPT" << 'EOF'
#!/bin/bash
# Robust wrapper for CBLT
# Works from anywhere and ensures assets/fonts/logs are found

# Resolve real path of this script (symlink-safe)
SOURCE="${BASH_SOURCE[0]}"
while [ -L "$SOURCE" ]; do
    DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"

# Store terminal's current working directory
USER_PWD="$PWD"

# Switch to release folder so binary finds assets/fonts/logs
cd "$DIR" || exit 1

# Run binary with original terminal directory as argument
./cblt "$USER_PWD"
EOF

chmod +x "$RUN_SCRIPT"

if [ "$LINK_GLOBAL" = true ]; then
    echo "Linking run.sh to /usr/local/bin/cblt"
    sudo ln -sf "$RUN_SCRIPT" "/usr/local/bin/cblt"
fi

cd "$RELEASE_BASE"
tar -czvf "$RELEASE_NAME.tar.gz" "$RELEASE_NAME"

echo "✅ Release complete: $RELEASE_BASE/$RELEASE_NAME.tar.gz"
echo "Contents of release folder:"
tree "$RELEASE_DIR" 2>/dev/null || ls -R "$RELEASE_DIR"