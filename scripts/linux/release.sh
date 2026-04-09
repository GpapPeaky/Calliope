#!/bin/bash
set -e

# ---------------- CONFIG ----------------
PROJECT_ROOT="$HOME/Desktop/GpapPeaky/DEV/Co.Ba.L.T"   # adjust to your project path
RELEASE_DIR="$HOME/releases/cblt"
BIN_NAME="cblt"
ASSETS=("assets" "themes" "config")             # folders to include in release
LINK_GLOBAL=true                                # set false if you don't want global symlink

echo "Starting release process..."

echo "Building release library..."
make -f "$PROJECT_ROOT/makeLinux.mk" release

echo "Building executable..."
make -f "$PROJECT_ROOT/makeLinux.mk" all

echo "Creating release folder..."
mkdir -p "$RELEASE_DIR/bin"

echo "Copying binary..."
cp "$PROJECT_ROOT/$BIN_NAME" "$RELEASE_DIR/bin/"

echo "Copying assets..."
for d in "${ASSETS[@]}"; do
    if [ -d "$PROJECT_ROOT/$d" ]; then
        cp -r "$PROJECT_ROOT/$d" "$RELEASE_DIR/"
    else
        echo "Warning: $d not found, skipping"
    fi
done

WRAPPER="$RELEASE_DIR/run.sh"
echo "Creating wrapper script at $WRAPPER..."
cat > "$WRAPPER" << 'EOF'
#!/bin/bash
# This wrapper works even if symlinked globally
# Resolve the real path to the release folder
SOURCE="${BASH_SOURCE[0]}"
while [ -L "$SOURCE" ]; do
    DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"
# Run the binary relative to the release folder, start in current terminal directory
"$DIR/bin/cblt" "$PWD"
EOF
chmod +x "$WRAPPER"

# 6️⃣ Optional: symlink to /usr/local/bin
if [ "$LINK_GLOBAL" = true ]; then
    echo "Linking wrapper to /usr/local/bin/$BIN_NAME"
    sudo ln -sf "$WRAPPER" "/usr/local/bin/$BIN_NAME"
fi

echo "✅ Release completed!"
echo "You can now run '$BIN_NAME' from any terminal."