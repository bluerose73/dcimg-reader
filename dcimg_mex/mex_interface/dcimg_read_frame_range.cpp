/*
 * dcimg_read_frame_range
 *
 * Author:
 *   Shengjie Ma
 *
 * Description:
 *   Read n frames from a DCIMG image stack.
 *
 *   Assumes the DCIMG file has only one session.
 *   In case of more than one sessions, it reads from the first session.
 *   (For the definition of a session, see DCIMG API reference.)
 *
 *   This function is not thread safe.
 *
 *   This source file intends to be compiled to a mex file and called from MATLAB.
 *
 * Usage:
 *   image = dcimg_read_frame_range(filename, start_frame, n_frames)
 *
 * Args:
 *   filename: single-quoted character vector, DCIMG filename.
 *             Must have ASCII characters only.
 *   start_frame: double, 1-indexed frame id.
 *   n_frames: double, number of frames to read.
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

    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
    const CharArray filename = inputs[0];
    const TypedArray<double> start_frame_array = inputs[1];
    int start_frame = (int)start_frame_array[0] - 1;
    const TypedArray<double> n_frames_array = inputs[2];
    int n_frames = (int)n_frames_array[0];

    int tot_frames, height, width, pixel_bytes;

    ret_code = dcimg_get_size(filename.toAscii(), &tot_frames, &height, &width, &pixel_bytes);

    if (ret_code != 0) {
      report_error(matlabPtr, "Fail to get image size");
      return;
    }

    ArrayFactory factory;
    if (pixel_bytes == 1) {      // uint_8
      buffer_ptr_t<uint8_t> m_buffer
        = factory.createBuffer<uint8_t>(n_frames * height * width);
      ret_code = dcimg_read_range_to_buffer(filename.toAscii(), start_frame, n_frames, m_buffer.get());
      if (ret_code != 0) {
        report_error(matlabPtr, "Fail to read frame");
        return;
      }
      TypedArray<uint8_t> array = factory.createArrayFromBuffer<uint8_t>(
        {(size_t) height, (size_t) width, (size_t) n_frames}, std::move(m_buffer));
      outputs[0] = matlabPtr->feval(u"pagetranspose", array);
    }
    else if (pixel_bytes == 2) { // uint_16
      buffer_ptr_t<uint16_t> m_buffer
        = factory.createBuffer<uint16_t>(n_frames * height * width);
      ret_code = dcimg_read_range_to_buffer(filename.toAscii(), start_frame, n_frames, m_buffer.get());
      if (ret_code != 0) {
        report_error(matlabPtr, "Fail to read frame");
        return;
      }
      TypedArray<uint16_t> array = factory.createArrayFromBuffer<uint16_t>(
        {(size_t) height, (size_t) width, (size_t) n_frames}, std::move(m_buffer));
      outputs[0] = matlabPtr->feval(u"pagetranspose", array);
    }
    else {
      // Should never reach here
      report_error(matlabPtr, "Unknown pixel type");
      return;
    }

    return;
  }

private:
  /* Checks input and output args, report errors to MATLAB if any.
   * Returns 0 if all checks pass.
   */
  int check_arguments(ArgumentList outputs, ArgumentList inputs) {
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

    if (inputs.size() != 3) {
      report_error(matlabPtr,
                   "Function 'dcimg_read_frame_range' accepts exactly 3 input arguments: "
                   "filename, start_frame, n_frames");
      return -1;
    }

    if (inputs[0].getType() != ArrayType::CHAR) {
      report_error(matlabPtr,
                   "Input argument 'filename' must be a character vector");
      return -1;
    }

    if (inputs[1].getType() != ArrayType::DOUBLE ||
        inputs[1].getNumberOfElements() != 1) {
      report_error(matlabPtr, "Input argument 'start_frame' must be a double scalar");
      return -1;
    }

    if (inputs[2].getType() != ArrayType::DOUBLE ||
        inputs[2].getNumberOfElements() != 1) {
      report_error(matlabPtr, "Input argument 'n_frames' must be a double scalar");
      return -1;
    }

    const TypedArray<double> start_frame_array = inputs[1];
    double start_frame_double = start_frame_array[0];
    if (start_frame_double != floor(start_frame_double) || start_frame_double <= 0) {
      report_error(matlabPtr,
             "Input argument 'start_frame' must be a positive integer");
      return -1;
    }

    if (start_frame_double > INT_MAX) {
      report_error(matlabPtr,
             ("Input argument 'start_frame' is too large. The limit is " +
            std::to_string(INT_MAX))
               .c_str());
      return -1;
    }

    const TypedArray<double> n_frames_array = inputs[2];
    double n_frames_double = n_frames_array[0];
    if (n_frames_double != floor(n_frames_double) || n_frames_double <= 0) {
      report_error(matlabPtr,
             "Input argument 'n_frames' must be a positive integer");
      return -1;
    }

    if (n_frames_double > INT_MAX) {
      report_error(matlabPtr,
             ("Input argument 'n_frames' is too large. The limit is " +
            std::to_string(INT_MAX))
               .c_str());
      return -1;
    }

    if (outputs.size() != 1) {
      report_error(matlabPtr, "Function 'dcimg_read_frame_range' returns 1 value");
      return -1;
    }

    return 0;
  }
};