#!/bin/bash

# Function to generate sources.list for Debian/Ubuntu   
generate_debian_sources() {
    DISTRO=$1
    sudo tee /etc/apt/sources.list <<EOL
deb http://deb.debian.org/debian $DISTRO main contrib non-free
deb-src http://deb.debian.org/debian $DISTRO main contrib non-free

deb http://deb.debian.org/debian $DISTRO-updates main contrib non-free
deb-src http://deb.debian.org/debian $DISTRO-updates main contrib non-free

deb http://security.debian.org/debian-security $DISTRO-security main contrib non-free
deb-src http://security.debian.org/debian-security $DISTRO-security main contrib non-free
EOL
}

# Function to generate sources.list for Ubuntu
generate_ubuntu_sources() {
    DISTRO=$1
    sudo tee /etc/apt/sources.list <<EOL
deb http://archive.ubuntu.com/ubuntu/ $DISTRO main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $DISTRO main restricted universe multiverse

deb http://archive.ubuntu.com/ubuntu/ $DISTRO-updates main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $DISTRO-updates main restricted universe multiverse

deb http://archive.ubuntu.com/ubuntu/ $DISTRO-security main restricted universe multiverse
deb-src http://archive.ubuntu.com/ubuntu/ $DISTRO-security main restricted universe multiverse
EOL
}

# Detect the distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    case $ID in
        debian)
            generate_debian_sources $VERSION_CODENAME
            ;;
        ubuntu)
            generate_ubuntu_sources $VERSION_CODENAME
            ;;
        *)
            echo "Unsupported distribution: $ID"
            exit 1
            ;;
    esac
else
    echo "Unable to detect the distribution."
    exit 1
fi