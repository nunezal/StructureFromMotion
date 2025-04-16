#!/bin/bash

# Exit on error
set -e

echo "Structure from Motion - Setup Script"
echo "===================================="

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
else
    echo "Homebrew is already installed."
fi

# Install OpenCV
echo "Installing OpenCV..."
brew install opencv

# Install COLMAP
echo "Installing COLMAP..."
brew install colmap

# Create build directory
echo "Setting up build environment..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build
echo "Building project..."
make -j4

echo "Setup complete!"
echo "To run the program: ./build/video_to_3d IMG_1934.MOV" 