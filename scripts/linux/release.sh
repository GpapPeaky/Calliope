#!/bin/bash
set -e

# ---------------- CONFIG ----------------
PROJECT_ROOT="$HOME/Desktop/GpapPeaky/DEV/Co.Ba.L.T"  # adjust to your project path
RELEASE_BASE="$PROJECT_ROOT/release"
RELEASE_NAME="cblt-linux-x86_64"
RELEASE_DIR="$RELEASE_BASE/$RELEASE_NAME"
BIN_NAME="cblt"
FOLDERS=("log" "assets" "meta" "options")  # folders to include
# ----------------------------------------

echo "=== Starting release packaging ==="

# Clean previous release
rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"

# ---------------- Build ----------------
echo "Building shared library..."
make -f "$PROJECT_ROOT/makeLinux.mk" release

echo "Building executable..."
make -f "$PROJECT_ROOT/makeLinux.mk" all

# ---------------- Copy files ----------------
echo "Copying binary..."
cp "$PROJECT_ROOT/$BIN_NAME" "$RELEASE_DIR/"
chmod +x "$RELEASE_DIR/$BIN_NAME"

echo "Copying resource folders..."
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

# ---------------- Create run.sh ----------------
RUN_SCRIPT="$RELEASE_DIR/run.sh"
echo "Creating run.sh..."
cat > "$RUN_SCRIPT" << 'EOF'
#!/bin/bash
# Linux wrapper for CBLT
# Opens editor in user's terminal directory while loading resources from release folder

# Resolve folder where run.sh resides (release folder)
SOURCE="${BASH_SOURCE[0]}"
while [ -L "$SOURCE" ]; do
    DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
RELEASE_DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"

# Terminal's current working directory
USER_PWD="$PWD"

# Export resource folder for binary to load assets/fonts/logs
export CBLT_RESOURCES="$RELEASE_DIR"

# Run the binary in the user's terminal directory
"$RELEASE_DIR/cblt" "$USER_PWD"
EOF
chmod +x "$RUN_SCRIPT"

# ---------------- Create install.sh ----------------
INSTALL_SCRIPT="$RELEASE_DIR/install.sh"
echo "Creating install.sh..."
cat > "$INSTALL_SCRIPT" << 'EOF'
#!/bin/bash
# Installer for CBLT
# Sets up a global command to run from anywhere

set -e

SOURCE="${BASH_SOURCE[0]}"
while [ -L "$SOURCE" ]; do
    DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$(cd -P "$(dirname "$SOURCE")" >/dev/null 2>&1 && pwd)"

TARGET="/usr/local/bin/cblt"

# Remove old symlink if exists
if [ -L "$TARGET" ] || [ -f "$TARGET" ]; then
    echo "Removing old cblt symlink..."
    sudo rm -f "$TARGET"
fi

# Symlink run.sh in release folder
sudo ln -s "$DIR/run.sh" "$TARGET"

echo "CBLT installed successfully."
echo "Have fun developing!"
echo "          -GpapPeaky."
echo "You can now run 'cblt' from any terminal."
EOF
chmod +x "$INSTALL_SCRIPT"

# ---------------- Package ----------------
echo "Creating tar.gz release..."
cd "$RELEASE_BASE"
tar -czvf "$RELEASE_NAME.tar.gz" "$RELEASE_NAME"

echo "Release complete: $RELEASE_BASE/$RELEASE_NAME.tar.gz"
echo "Contents of release folder:"
tree "$RELEASE_DIR" 2>/dev/null || ls -R "$RELEASE_DIR"