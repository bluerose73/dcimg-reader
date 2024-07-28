#include <cstdio>
#include <cstring>
#include <dcimg_mex/impl/dcimg_ops.hpp>
#include <fiji_plugin/src/main/cpp/io_bluerose73_fiji_dcimg_DcimgReader.h>
#include <string>

/*
 * Class:     io_bluerose73_fiji_dcimg_DcimgReader
 * Method:    ReadSize
 * Signature: (Ljava/lang/String;)Lio/bluerose73/fiji/dcimg/DcimgReader/Size;
 */
JNIEXPORT jobject JNICALL Java_io_bluerose73_fiji_dcimg_DcimgReader_ReadSize(
    JNIEnv *env, jclass dcimg_reader_class, jstring filepath) {
  printf("info: in c++ dcimg read size\n");

  jclass cls = env->FindClass("io/bluerose73/fiji/dcimg/DcimgReader$Size");
  if (cls == NULL) {
    printf("class not found\n");
    return NULL;
  }
  jmethodID mid_init = env->GetMethodID(cls, "<init>", "()V");
  if (mid_init == NULL) {
    printf("method init not found\n");
    return NULL;
  }
  jobject ret = env->NewObject(cls, mid_init);
  if (ret == NULL) {
    printf("return value init failed\n");
    return NULL;
  }

  const char *filepath_str = env->GetStringUTFChars(filepath, NULL);
  int width, height, n_frames, bytes_per_pixel;
  int err =
      dcimg_get_size(filepath_str, &n_frames, &height, &width, &bytes_per_pixel);
  if (err) {
    printf("dcimg fail to get size\n");
    return ret;
  }
  int bits_per_pixel = bytes_per_pixel * 8;

  // public int width;
  // public int height;
  // public int nFrames;
  // public int bitsPerPixel;
  jfieldID width_id = env->GetFieldID(cls, "width", "I");
  jfieldID height_id = env->GetFieldID(cls, "height", "I");
  jfieldID n_frames_id = env->GetFieldID(cls, "nFrames", "I");
  jfieldID bits_per_pixel_id = env->GetFieldID(cls, "bitsPerPixel", "I");

  env->SetIntField(ret, width_id, width);
  env->SetIntField(ret, height_id, height);
  env->SetIntField(ret, n_frames_id, n_frames);
  env->SetIntField(ret, bits_per_pixel_id, bits_per_pixel);
  return ret;
}

/*
 * Class:     io_bluerose73_fiji_dcimg_DcimgReader
 * Method:    ReadFrame
 * Signature: (Ljava/lang/String;I)[B
 */
JNIEXPORT jbyteArray JNICALL Java_io_bluerose73_fiji_dcimg_DcimgReader_ReadFrame(
    JNIEnv *env, jclass dcimg_reader_class, jstring filepath, jint frame_id) {
  const char *filepath_str = env->GetStringUTFChars(filepath, NULL);
  int i_frame_id = frame_id;
  signed char *buffer;
  int width, height, bytes_per_pixel;

  int err = dcimg_read_frame(filepath_str, frame_id, (void **)&buffer, &height,
                             &width, &bytes_per_pixel);
  if (err) {
    return env->NewByteArray(0);
  }

  // TODO(shengjie): eliminate the copy
  long n_bytes = (long)height * width * bytes_per_pixel;
  jbyteArray ret = env->NewByteArray(n_bytes);
  env->SetByteArrayRegion(ret, 0, n_bytes, buffer);
  free(buffer);
  return ret;
}
