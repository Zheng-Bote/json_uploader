#!/usr/bin/bash

# =============================================================================
# KONFIGURATION
# =============================================================================
APP_NAME="json_uploader"
PROJECT_DIR=$(pwd)
BUILD_DIR="$PROJECT_DIR/build_appimage"
APP_DIR="$BUILD_DIR/AppDir"
ICON_SOURCE="$PROJECT_DIR/resources/app_icon.png"
DESKTOP_FILE="$PROJECT_DIR/resources/$APP_NAME.desktop"

# QMake (wird für das Plugin benötigt, um Qt-Pfade zu finden)
#export QMAKE="qmake6"
#export QMAKE="${QMAKE:-qmake6}"

# Check Voraussetzungen
for tool in patchelf file conan; do
    if ! command -v $tool &> /dev/null; then
        echo "FEHLER: '$tool' wurde nicht gefunden. Bitte installieren."
        exit 1
    fi
done

# Check Conan version
CONAN_VER=$(conan --version | grep -oE '[0-9]+\.[0-9]+' | head -1)
if (( $(echo "$CONAN_VER < 2.0" | bc -l) )); then
    echo "FEHLER: Conan Version 2.0+ erforderlich (gefunden: $CONAN_VER)"
    exit 1
fi

echo "--- Starte AppImage Erstellung (Modern Method) ---"

# =============================================================================
# 1. BUILD UMGEBUNG VORBEREITEN
# =============================================================================
echo "--- Vorbereiten der Build-Umgebung ---"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# =============================================================================
# 2. CONAN DEPENDENCIES
# =============================================================================
echo "--- Conan Install ---"
# Wir führen Conan im BUILD_DIR aus, verweisen aber auf das conanfile im PROJECT_DIR
cd "$BUILD_DIR"
conan install "$PROJECT_DIR" --output-folder=. --build=missing -s compiler.cppstd=23

if [ $? -ne 0 ]; then
    echo "FEHLER: Conan install fehlgeschlagen!"
    exit 1
fi

# =============================================================================
# 3. KOMPILIEREN
# =============================================================================
echo "--- CMake Konfiguration ---"
# In Conan v2 mit cmake_layout liegen die Generatoren in build/Release/generators (oder ähnlich)
# Da wir --output-folder=. genutzt haben, liegen sie direkt in $BUILD_DIR/build/Release/generators
cmake "$PROJECT_DIR" -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "FEHLER: CMake Konfiguration fehlgeschlagen!"
    exit 1
fi

echo "--- Build ---"
cmake --build . -j"$(nproc)"

if [ $? -ne 0 ]; then
    echo "FEHLER: Build fehlgeschlagen!"
    exit 1
fi

# =============================================================================
# 4. APPDIR STRUKTUR VORBEREITEN
# =============================================================================
echo "--- Vorbereiten des AppDirs ---"
mkdir -p "$APP_DIR/usr/bin"
mkdir -p "$APP_DIR/usr/share/applications"
mkdir -p "$APP_DIR/usr/share/icons/hicolor/256x256/apps"

# Binärdatei kopieren (liegt im aktuellen Verzeichnis nach dem Build)
cp "$APP_NAME" "$APP_DIR/usr/bin/"

# Icons und Desktop-Datei kopieren
if [ -f "$ICON_SOURCE" ]; then
    cp "$ICON_SOURCE" "$APP_DIR/usr/share/icons/hicolor/256x256/apps/$APP_NAME.png"
else
    echo "WARNUNG: Icon nicht gefunden unter $ICON_SOURCE"
fi

if [ -f "$DESKTOP_FILE" ]; then
    cp "$DESKTOP_FILE" "$APP_DIR/usr/share/applications/"
else
    # Erstelle eine temporäre Desktop-Datei, falls keine existiert
    echo "Erstelle Standard .desktop Datei..."
    cat > "$APP_DIR/usr/share/applications/$APP_NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=JSON Uploader
Exec=$APP_NAME
Icon=$APP_NAME
Comment=Stream large JSON files to REST API
Categories=Utility;
EOF
    DESKTOP_FILE="$APP_DIR/usr/share/applications/$APP_NAME.desktop"
fi

# Optional: Bereinigen von Conan-Metadaten im AppDir, falls diese mitinstalliert wurden
find "$APP_DIR" -name "*.cmake" -delete
find "$APP_DIR" -name "*.pc" -delete

# =============================================================================
# 5. LINUXDEPLOY & QT PLUGIN LADEN
# =============================================================================
# Basis-Tool
if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    wget -q "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod +x linuxdeploy-x86_64.AppImage
fi

# Qt-Plugin
#if [ ! -f "linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
#    wget -q "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
#    chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
#fi

# =============================================================================
# 5. APPIMAGE GENERIEREN
# =============================================================================
echo "--- Generiere AppImage ---"

# Environment Variablen für das Qt Plugin setzen
export LD_LIBRARY_PATH="$APP_DIR/usr/lib:$LD_LIBRARY_PATH"

# Das Tool aufrufen
# --appdir: Wo liegt die App?
# --output appimage: Erstelle am Ende die fertige Datei
./linuxdeploy-x86_64.AppImage \
    --appdir "$APP_DIR" \
    --executable "$APP_DIR/usr/bin/$APP_NAME" \
    --desktop-file "$DESKTOP_FILE" \
    --icon-file "$ICON_SOURCE" \
    --output appimage

if [ $? -eq 0 ]; then
    echo "------------------------------------------------"
    echo "ERFOLG! AppImage erstellt:"
    ls -lh *.AppImage
    echo "------------------------------------------------"
else
    echo "FEHLER: linuxdeploy ist fehlgeschlagen."
    exit 1
fi