#ifndef DCIMG_MEX_MEX_INTERFACE_UTIL_H
#define DCIMG_MEX_MEX_INTERFACE_UTIL_H

#include <mex.hpp>
#include <mexAdapter.hpp>

using namespace matlab::data;

inline void report_error(std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr,
                         const char *msg) {
  ArrayFactory factory;
  matlabPtr->feval(u"error", 0, std::vector<Array>({factory.createScalar(msg)}));
}

#endif