# Structure from Motion

A C++ project for converting video to 3D models using COLMAP and OpenCV.

## Prerequisites

- CMake (>= 3.10)
- C++ compiler with C++17 support
- OpenCV
- COLMAP

## Installation

### Mac

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install OpenCV
brew install opencv

# Install COLMAP
brew install colmap

# Or build COLMAP from source:
# Follow instructions at https://colmap.github.io/install.html#mac
```

### Building the Project

```bash
# Clone the repository
git clone https://github.com/yourusername/StructureFromMotion.git
cd StructureFromMotion

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make
```

## Usage

```bash
# Run with default settings (100 frames at 2 fps)
./video_to_3d path/to/your/video.mp4

# Run with custom number of frames and frame rate
./video_to_3d path/to/your/video.mp4 200 5
```

## Project Structure

- `src/` - Source code
- `frames/` - Extracted video frames
- `output/` - COLMAP reconstruction output
  - `sparse/` - Sparse reconstruction
  - `dense/` - Dense reconstruction
  - `dense/fused.ply` - Final 3D model

## Viewing the Results

The final 3D model will be saved as a PLY file at `output/dense/fused.ply`. You can view this file with 3D model viewers like:

- [MeshLab](https://www.meshlab.net/)
- [CloudCompare](https://www.danielgm.net/cc/)
- [Blender](https://www.blender.org/)

## How it Works

1. The program extracts frames from the input video
2. COLMAP's Structure-from-Motion pipeline generates a sparse reconstruction
3. COLMAP's Multi-View Stereo pipeline creates a dense reconstruction
4. The final model is saved as a PLY file
