///
/// \file Chris.h
///
///	\author Chris Cummins <chrisc.101@gmail.com>
///
/// This file is used to help segregate Chris' work from the main
/// SkelCL codebase, and contains code revelant to his work on
/// autotuning SkelCL.

#ifndef CHRIS_H_
#define CHRIS_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include <CL/cl.h>
#pragma GCC diagnostic pop  //  -Weffc++

#include "detail/Program.h"

namespace chris {


///
/// Return the number of partitions to use for a given inputBuffer and
/// work group size.
///
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
template <typename Tin>
cl_uint get_num_partitions(Tin &inputBuffer, const cl_uint workGroupSize) {
  return static_cast<cl_uint>(2);
}
#pragma GCC diagnostic pop //  -Wunused-parameter

///
/// Returns a string of OpenCL options to build a given program for a
/// given device with.
///
const char *get_cl_flags(skelcl::detail::Program *const program,
                         cl::Device device);

} // namespace chris

#endif // CHRIS_H_
