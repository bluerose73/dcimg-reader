/*
 * DCIMG operations.
 *
 * Author:
 *   Hamamatsu Photonics
 *
 * Modified by:
 *   Shengjie Ma
 */

#include <Windows.h>
#include <cstring>
#include <dcimg_mex/impl/dcimg_ops.hpp>
#include <dcimgapi/dcimgapi.h>

// Helper Functions
// ================

int dcimg_init_open(const std::string &filename, HDCIMG *dcimg_handle) {
  DCIMG_ERR err;

  // initialize DCIMG-API
  DCIMG_INIT imginit;
  memset(&imginit, 0, sizeof(imginit));
  imginit.size = sizeof(imginit);
  err = dcimg_init(&imginit);
  if (failed(err)) {
    return -1;
  }

  // open DCIMG file
  DCIMG_OPEN imgopen;
  memset(&imgopen, 0, sizeof(imgopen));
  imgopen.size = sizeof(imgopen);
  imgopen.path = filename.c_str();
  err = dcimg_open(&imgopen);
  if (failed(err)) {
    return -1;
  }

  *dcimg_handle = imgopen.hdcimg;
  return 0;
}

int get_image_information(HDCIMG hdcimg, int *n_frames, int *height, int *width) {
  DCIMG_ERR err;

  // NOTE(Shengjie)
  // DCIMG API likely has a bug about int32 type.
  // In x64 Windows, int32 is long, which is 64 bit.
  int32 n_frames_i32, width_i32, height_i32;

  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_NUMBEROF_FRAME, &n_frames_i32);
  if (failed(err)) {
    return -1;
  }

  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_IMAGE_HEIGHT, &height_i32);
  if (failed(err)) {
    return -1;
  }

  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_IMAGE_WIDTH, &width_i32);
  if (failed(err)) {
    return -1;
  }

  *n_frames = n_frames_i32;
  *height = height_i32;
  *width = width_i32;

  return 0;
}

int lock_frame_helper(HDCIMG hdcimg, int frame_id, void **image, int *height,
                      int *width, int *pixel_bytes, int *row_stride) {
  DCIMG_ERR err;

  DCIMG_FRAME imgframe;
  memset(&imgframe, 0, sizeof(imgframe));
  imgframe.size = sizeof(imgframe);
  imgframe.iFrame = frame_id;
  err = dcimg_lockframe(hdcimg, &imgframe);
  if (failed(err)) {
    return -1;
  }
  *image = imgframe.buf;
  *height = imgframe.height;
  *width = imgframe.width;
  *row_stride = imgframe.rowbytes;
  switch (imgframe.type) {
  case DCIMG_PIXELTYPE_MONO8:
    *pixel_bytes = 1;
    break;
  case DCIMG_PIXELTYPE_MONO16:
    *pixel_bytes = 2;
    break;
  default:
    return -1;
  }

  return 0;
}

int copy_frame_row_major(void *target, void *source, int height, int width,
                         int pixel_bytes, int row_stride) {
  char *dst = (char *)target;
  char *src = (char *)source;
  int row_length = width * pixel_bytes;

  for (int i = 0; i < height; i++) {
    memcpy(dst + row_length * i, src + row_stride * i, row_length);
  }
  return 0;
}

// MEX Interface Implementations
// ================================

int dcimg_get_size(const std::string &filename, int *n_frames, int *height,
                   int *width) {
  int err_code;
  DCIMG_ERR dcimg_err;

  HDCIMG dcimg_handle;
  err_code = dcimg_init_open(filename, &dcimg_handle);
  if (err_code != 0) {
    return err_code;
  }

  err_code = get_image_information(dcimg_handle, n_frames, height, width);
  if (err_code != 0) {
    return err_code;
  }

  dcimg_err = dcimg_close(dcimg_handle);
  if (failed(dcimg_err)) {
    return -1;
  }

  return 0;
}

int dcimg_read_frame(const std::string &filename, int frame_id, void **image,
                     int *height, int *width, int *pixel_bytes) {
  int err_code;
  DCIMG_ERR dcimg_err;

  HDCIMG dcimg_handle;
  err_code = dcimg_init_open(filename, &dcimg_handle);
  if (err_code != 0) {
    return err_code;
  }

  // access frame data
  int row_stride;
  void *buffer;
  err_code = lock_frame_helper(dcimg_handle, frame_id, &buffer, height, width,
                               pixel_bytes, &row_stride);
  if (err_code != 0) {
    return err_code;
  }

  *image = malloc((*height) * (*width) * (*pixel_bytes));
  copy_frame_row_major(*image, buffer, *height, *width, *pixel_bytes, row_stride);

  dcimg_err = dcimg_close(dcimg_handle);
  if (failed(dcimg_err)) {
    return -1;
  }

  return 0;
}

// Not Implemented
// int dcimg_read_all(const std::string &filename, void **image, int *n_frames,
//                    int *height, int *width, int *pixel_bytes) {
//   int err_code;
//   DCIMG_ERR dcimg_err;

//   HDCIMG dcimg_handle;
//   err_code = dcimg_init_open(filename, &dcimg_handle);
//   if (err_code != 0) {
//     return err_code;
//   }

//   // access frame data
//   int row_stride;
//   void *buffer;
//   err_code = lock_frame_helper(dcimg_handle, frame_id, &buffer, height, width,
//                                pixel_bytes, &row_stride);
//   if (err_code != 0) {
//     return err_code;
//   }

//   *image = malloc((*height) * (*width) * (*pixel_bytes));
//   copy_frame_row_major(*image, buffer, *height, *width, *pixel_bytes, row_stride);

//   dcimg_err = dcimg_close(dcimg_handle);
//   if (failed(dcimg_err)) {
//     return -1;
//   }

//   return 0;
// }