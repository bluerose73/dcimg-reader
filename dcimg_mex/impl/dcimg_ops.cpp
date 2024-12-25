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

int hdcimg_get_size(HDCIMG hdcimg, int *n_frames, int *height, int *width,
                    int *pixel_bytes) {
  DCIMG_ERR err;

  // NOTE(Shengjie)
  // DCIMG API likely has a bug about int32 type.
  // In x64 Windows, int32 is long, which is 64 bit.
  int32 n_frames_i32, width_i32, height_i32, pixel_bytes_i32;

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

  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_IMAGE_PIXELTYPE, &pixel_bytes_i32);
  if (failed(err)) {
    return -1;
  }

  *n_frames = n_frames_i32;
  *height = height_i32;
  *width = width_i32;

  switch (pixel_bytes_i32) {
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

/**
 @brief	Get image information.
 @param	hdcimg:		DCIMG handle
 @param width:		image width
 @param height:		image height
 @param rowbytes:	image rowbytes
 @param pixeltype:	DCIMG_PIXELTYPE value
 @return	result to get information
 */
BOOL get_image_information(HDCIMG hdcimg, int32 &width, int32 &height,
                           int32 &rowbytes, int32 &pixeltype) {
  DCIMG_ERR err;

  int32 nWidth;
  // get width
  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_IMAGE_WIDTH, &nWidth);
  if (failed(err)) {
    return FALSE;
  }

  int32 nHeight;
  // get height
  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_IMAGE_HEIGHT, &nHeight);
  if (failed(err)) {
    return FALSE;
  }

  int32 nRowbytes;
  // get row bytes
  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_IMAGE_ROWBYTES, &nRowbytes);
  if (failed(err)) {
    return FALSE;
  }

  int32 nPixeltype;
  // get pixel type
  err = dcimg_getparaml(hdcimg, DCIMG_IDPARAML_IMAGE_PIXELTYPE, &nPixeltype);
  if (failed(err)) {
    return FALSE;
  }

  width = nWidth;
  height = nHeight;
  rowbytes = nRowbytes;
  pixeltype = nPixeltype;

  return TRUE;
}

// image is allocated by hdcimg_read_frame
int hdcimg_read_frame(HDCIMG hdcimg, int frame_id, void **image, int *height_ptr,
                      int *width_ptr, int *pixel_bytes_ptr) {
  DCIMG_ERR err;

  DCIMG_FRAME imgframe;
  memset(&imgframe, 0, sizeof(imgframe));
  imgframe.size = sizeof(imgframe);
  imgframe.iFrame = frame_id;

  int32 width, height, rowbytes, pixeltype;
  if (!get_image_information(hdcimg, width, height, rowbytes, pixeltype))
    return -1;

  int pixel_bytes;
  switch (pixeltype) {
  case DCIMG_PIXELTYPE_MONO8:
    pixel_bytes = 1;
    break;
  case DCIMG_PIXELTYPE_MONO16:
    pixel_bytes = 2;
    break;
  default:
    return -1;
  }

  void *buf = malloc(height * width * pixel_bytes);
  imgframe.buf = buf;
  imgframe.width = width;
  imgframe.height = height;
  imgframe.rowbytes = rowbytes;
  imgframe.type = (DCIMG_PIXELTYPE)pixeltype;

  err = dcimg_copyframe(hdcimg, &imgframe);
  if (failed(err)) {
    free(buf);
    return -1;
  }

  *image = buf;
  *height_ptr = height;
  *width_ptr = width;
  *pixel_bytes_ptr = pixel_bytes;

  return 0;
}

// Same as hdcimg_read_frame, but buffer is allocated by the caller
int hdcimg_read_frame_to_buffer(HDCIMG hdcimg, int frame_id, void *buffer, int *height_ptr,
                      int *width_ptr, int *pixel_bytes_ptr) {
  DCIMG_ERR err;

  DCIMG_FRAME imgframe;
  memset(&imgframe, 0, sizeof(imgframe));
  imgframe.size = sizeof(imgframe);
  imgframe.iFrame = frame_id;

  int32 width, height, rowbytes, pixeltype;
  if (!get_image_information(hdcimg, width, height, rowbytes, pixeltype))
    return -1;

  int pixel_bytes;
  switch (pixeltype) {
  case DCIMG_PIXELTYPE_MONO8:
    pixel_bytes = 1;
    break;
  case DCIMG_PIXELTYPE_MONO16:
    pixel_bytes = 2;
    break;
  default:
    return -1;
  }

  imgframe.buf = buffer;
  imgframe.width = width;
  imgframe.height = height;
  imgframe.rowbytes = rowbytes;
  imgframe.type = (DCIMG_PIXELTYPE)pixeltype;

  err = dcimg_copyframe(hdcimg, &imgframe);
  if (failed(err)) {
    return -1;
  }

  *height_ptr = height;
  *width_ptr = width;
  *pixel_bytes_ptr = pixel_bytes;

  return 0;
}

// MEX Interface Implementations
// ================================

int dcimg_get_size(const std::string &filename, int *n_frames, int *height,
                   int *width, int *pixel_size) {
  int err_code;
  DCIMG_ERR dcimg_err;

  HDCIMG dcimg_handle;
  err_code = dcimg_init_open(filename, &dcimg_handle);
  if (err_code != 0) {
    return err_code;
  }

  err_code = hdcimg_get_size(dcimg_handle, n_frames, height, width, pixel_size);
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
  err_code =
      hdcimg_read_frame(dcimg_handle, frame_id, image, height, width, pixel_bytes);
  if (err_code != 0) {
    return err_code;
  }

  dcimg_err = dcimg_close(dcimg_handle);
  if (failed(dcimg_err)) {
    free(*image);
    return -1;
  }

  return 0;
}

int dcimg_read_range_to_buffer(const std::string &filename, int start_frame, int n_frames, void *buffer) {
  int err_code;
  DCIMG_ERR dcimg_err;

  HDCIMG dcimg_handle;
  err_code = dcimg_init_open(filename, &dcimg_handle);
  if (err_code != 0) {
    return err_code;
  }

  // access frame data
  int height, width, pixel_bytes;
  for (int i = start_frame; i < start_frame + n_frames; i++) {
    err_code = hdcimg_read_frame_to_buffer(dcimg_handle, i, buffer, &height, &width, &pixel_bytes);
    if (err_code != 0) {
      return err_code;
    }
    buffer = (void *)((char *)buffer + height * width * pixel_bytes);
  }

  dcimg_err = dcimg_close(dcimg_handle);
  if (failed(dcimg_err)) {
    return -1;
  }

  return 0;
}