/*
 * DCIMG operations.
 *
 * Author:
 *   Shengjie Ma
 *
 * Conventions:
 *   Input arguments are passed by value or by const reference.
 *   Output arguments are passed by pointer.
 *   Return value is error code. 0 means success.
 */

#ifndef DCIMG_MEX_IMPL_DCIMG_OPS_H
#define DCIMG_MEX_IMPL_DCIMG_OPS_H

#include <string>

int dcimg_get_size(const std::string &filename, int *n_frames, int *height,
                   int *width);

int dcimg_read_frame(const std::string &filename, int frame_id, void *image,
                     int *width, int *height, int *pixel_bytes);

int dcimg_read_all(const std::string &filename, void *image, int *n_frames,
                   int *height, int *width, int *pixel_bytes);

#endif