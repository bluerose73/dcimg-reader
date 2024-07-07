// Usage: test_read.exe filename

#include <cstdio>
#include <dcimg_mex/impl/dcimg_ops.hpp>

int main(int argc, char **argv) {
  printf("start\n");
  if (argc != 2) {
    printf("Usage: test_read.exe filename\n");
    return -1;
  }

  int err = 0;

  void *buffer = NULL;
  int h, w, pixel_bytes;
  err = dcimg_read_frame(argv[1], 0, &buffer, &h, &w, &pixel_bytes);

  printf("here\n");

  if (err) {
    printf("Fail to read\n");
    return -1;
  }

  printf("height = %d  width = %d  pixel_bytes = %d buffer = %p\n", h, w,
         pixel_bytes, buffer);
  if (pixel_bytes != 2)
    return 0;

  printf("before reading\n");
  uint16_t *img = (uint16_t *)buffer;
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      printf("%u ", (unsigned int)img[i * w + j]);
      //   fflush(stdout);
    }
    printf("\n");
  }
  return 0;
}