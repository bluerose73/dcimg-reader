# DCIMG MEX

Read DCIMG images to MATLAB.

## Usage

Add ``dcimg_get_size.mexw64`` and ``dcimg_read_frame.mexw64`` to MATLAB path.

Call the functions

```MATLAB
[height, width, n_frames] = dcimg_get_size('filename.dcimg')

image = dcimg_read_frame('filename.dcimg', 1)  % frame id is 1-indexed
```

## Build

DCIMG MEX only supports 64-bit Windows platform.

To build the mex files from source, you need these dependencies installed:
- CMake
- At least one C++ build tools. For example,
  - Visual Studio
  - MinGW

Run the following lines in the top-level directory of this repository.

```bash
mkdir build
cd build
cmake ..
cmake —-build . —-config Release
```