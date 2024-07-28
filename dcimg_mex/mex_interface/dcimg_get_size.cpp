/*
 * dcimg_get_size
 *
 * Author:
 *   Shengjie Ma
 *
 * Description:
 *   Get height, width, and number of frames of a DCIMG image stack.
 *
 *   Assumes the DCIMG file has only one session.
 *   In case of more than one sessions, the size of the first session is returned.
 *
 *   This source file intends to be compiled to a mex file and called from MATLAB.
 *
 * Usage:
 *   [height, width, nframes] = dcimg_get_size(filename)
 *
 * Args:
 *   filename (single-quoted character vector): DCIMG filename.
 *             Must have ASCII characters only.
 *
 * Return:
 *   n_frames (double): Number of frames
 *   height (double): Image height (number of rows)
 *   width: (double): Image width (number of columns)
 */

#include <dcimg_mex/impl/dcimg_ops.hpp>
#include <dcimg_mex/mex_interface/util.hpp>
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
    int n_frames, height, width, pixel_bytes;

    // This line is where real work happens
    ret_code =
        dcimg_get_size(filename.toAscii(), &n_frames, &height, &width, &pixel_bytes);

    if (ret_code != 0) {
      std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
      report_error(matlabPtr, "Fail to get image size");
      return;
    }

    ArrayFactory factory;
    outputs[0] = factory.createScalar<double>(height);
    outputs[1] = factory.createScalar<double>(width);
    outputs[2] = factory.createScalar<double>(n_frames);
  }

private:
  /* Checks input and output args, report errors to MATLAB if any.
   * Returns 0 if all checks pass.
   */
  int check_arguments(ArgumentList outputs, ArgumentList inputs) {
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

    if (inputs.size() != 1) {
      report_error(matlabPtr,
                   "Function 'dcimg_get_size' accepts exactly 1 input arguments");
      return -1;
    }

    if (inputs[0].getType() != ArrayType::CHAR) {
      report_error(matlabPtr,
                   "Input argument 'filename' must be a character vector");
      return -1;
    }

    if (outputs.size() != 3) {
      report_error(matlabPtr, "Function 'dcimg_get_size' returns 3 values");
      return -1;
    }

    return 0;
  }
};