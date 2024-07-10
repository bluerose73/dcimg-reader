/*
 * DCIMG operations.
 *
 * Author:
 *   Shengjie Ma
 *
 * Conventions:
 *   Input arguments are passed by value or by const reference.
 *   Output arguments are passed by pointer.
 *   Return value is always error code. 0 means success.
 */

#ifndef DCIMG_MEX_IMPL_DCIMG_OPS_H
#define DCIMG_MEX_IMPL_DCIMG_OPS_H

#include <string>

int dcimg_get_size(const std::string &filename, int *n_frames, int *height,
                   int *width, int *pixel_bytes);

/* Reads one frame from DCIMG file.
 *
 * Usage:
 *   void *buffer;
 *   int height, width, pixel_bytes;
 *   dcimg_read_frame("filename.dcimg", 0, &buffer, &height, &width, &pixel_bytes);
 *   if (pixel_bytes == 1) {
 *     uint_8 *image = (uint_8 *) buffer;
 *     // Do something
 *   }
 *
 * Args:
 *   image: buffer containing image data.
 *          The buffer memory is allocated by dcimg_read_frame, but should be
 *          released by the caller.
 *          Data type can be uint8 or uint16, depending on the image's pixel type.
 *          This can be inferred from pixel_bytes.
 *   pixel_bytes: 1 (uint8) or 2 (uint16)
 */
int dcimg_read_frame(const std::string &filename, int frame_id, void **image,
                     int *height, int *width, int *pixel_bytes);

int dcimg_read_all(const std::string &filename, void **image, int *n_frames,
                   int *height, int *width, int *pixel_bytes);

#endif