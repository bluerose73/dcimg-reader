/*
 * dcimg_read_frame
 *
 * Author:
 *   Shengjie Ma
 *
 * Description:
 *   Read one frame from a DCIMG image stack.
 *
 *   Assumes the DCIMG file has only one session.
 *   In case of more than one sessions, it reads from the first session.
 *
 *   This function is not thread safe.
 *
 *   This source file intends to be compiled to a mex file and called from MATLAB.
 *
 * Usage:
 *   image = dcimg_read_frame(filename, frame_id)
 *
 * Args:
 *   filename: single-quoted character vector, DCIMG filename.
 *             Must have ASCII characters only.
 *   frame_id: double, 1-indexed frame id.
 *
 * Return:
 *   image: 2D array of shape (width x height). Data type depends on the DCIMG file.
 */

#include <climits>
#include <cmath>
#include <dcimg_mex/impl/dcimg_ops.hpp>
#include <dcimg_mex/mex_interface/util.hpp>
#include <exception>
#include <mex.hpp>
#include <mexAdapter.hpp>

using namespace matlab::data;
using matlab::mex::ArgumentList;

class MexFunction : public matlab::mex::Function {
public:
  void operator()(ArgumentList outputs, ArgumentList inputs) {
    int ret_code = check_arguments(outputs, inputs);
    if (ret_code != 0) {
      return;
    }

    const CharArray filename = inputs[0];
    const TypedArray<double> frame_id_array = inputs[1];
    int frame_id = (int)frame_id_array[0] - 1;

    void *image_buffer = nullptr;
    int height, width, pixel_bytes;

    // This line is where real work happens
    ret_code = dcimg_read_frame(filename.toAscii(), frame_id, &image_buffer, &height,
                                &width, &pixel_bytes);

    if (ret_code != 0) {
      if (image_buffer != nullptr)
        free(image_buffer);
      std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
      report_error(matlabPtr, "Fail to read frame");
      return;
    }

    ArrayFactory factory;
    if (pixel_bytes == 1) { // uint_8
      uint8_t *image = (uint8_t *)image_buffer;
      outputs[0] =
          factory.createArray({(size_t)height, (size_t)width}, image,
                              image + (height * width), InputLayout::ROW_MAJOR);
    } else if (pixel_bytes == 2) { // uint_16
      uint16_t *image = (uint16_t *)image_buffer;
      outputs[0] =
          factory.createArray({(size_t)height, (size_t)width}, image,
                              image + (height * width), InputLayout::ROW_MAJOR);
    } else {
      // Should never reach here
      free(image_buffer);
      std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
      report_error(matlabPtr, "Unknown pixel type");
      return;
    }
    free(image_buffer);
    return;
  }

private:
  /* Checks input and output args, report errors to MATLAB if any.
   * Returns 0 if all checks pass.
   */
  int check_arguments(ArgumentList outputs, ArgumentList inputs) {
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

    if (inputs.size() != 2) {
      report_error(matlabPtr,
                   "Function 'dcimg_read_frame' accepts exactly 2 input arguments");
      return -1;
    }

    if (inputs[0].getType() != ArrayType::CHAR) {
      report_error(matlabPtr,
                   "Input argument 'filename' must be a character vector");
      return -1;
    }

    if (inputs[1].getType() != ArrayType::DOUBLE ||
        inputs[1].getNumberOfElements() != 1) {
      report_error(matlabPtr, "Input argument 'frame_id' must be a double scalar");
      return -1;
    }

    const TypedArray<double> frame_id_array = inputs[1];
    double frame_id_double = frame_id_array[0];
    if (frame_id_double != floor(frame_id_double) || frame_id_double <= 0) {
      report_error(matlabPtr,
                   "Input argument 'frame_id' must be a positive integer");
      return -1;
    }

    if (frame_id_double > INT_MAX) {
      report_error(matlabPtr,
                   ("Input argument 'frame_id' is too large. The limit is " +
                    std::to_string(INT_MAX))
                       .c_str());
      return -1;
    }

    if (outputs.size() != 1) {
      report_error(matlabPtr, "Function 'dcimg_read_frame' returns 1 value");
      return -1;
    }

    return 0;
  }
};