#!/bin/bash
# Setup script for Bluetooth HID on Raspberry Pi Zero
# This script configures the Pi as a Bluetooth keyboard

set -e

echo "=== Bluetooth HID Setup for Raspberry Pi Zero ==="

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (sudo)"
    exit 1
fi

# Install required packages
echo "Installing Bluetooth packages..."
apt-get update
apt-get install -y \
    bluez \
    bluetooth \
    pi-bluetooth \
    bluez-tools

# Enable Bluetooth service
echo "Enabling Bluetooth service..."
systemctl enable bluetooth
systemctl start bluetooth

# Configure Bluetooth adapter
echo "Configuring Bluetooth adapter..."

# Create Bluetooth configuration
cat > /etc/bluetooth/main.conf << 'EOF'
[General]
Name = MacroPad
Class = 0x002540
DiscoverableTimeout = 0
PairableTimeout = 0

[Policy]
AutoEnable=true
EOF

# Create udev rules for Bluetooth permissions
cat > /etc/udev/rules.d/99-bluetooth-hid.rules << 'EOF'
# Allow the pi user to access Bluetooth HID
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0666"
SUBSYSTEM=="bluetooth", MODE="0666"
EOF

# Reload udev rules
udevadm control --reload-rules
udevadm trigger

# Add pi user to bluetooth group
usermod -a -G bluetooth pi 2>/dev/null || true

# Restart Bluetooth
systemctl restart bluetooth

echo ""
echo "=== Setup Complete ==="
echo "The Raspberry Pi is now configured as a Bluetooth HID device."
echo "Device name: MacroPad"
echo ""
echo "To pair with a computer:"
echo "1. Put your computer in Bluetooth pairing mode"
echo "2. Run the MacroPad application"
echo "3. Press the 'Pair' button on the touchscreen"
echo ""
