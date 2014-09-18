/*****************************************************************************
 * Copyright (c) 2011-2012 The SkelCL Team as listed in CREDITS.txt          *
 * http://skelcl.uni-muenster.de                                             *
 *                                                                           *
 * This file is part of SkelCL.                                              *
 * SkelCL is available under multiple licenses.                              *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * SkelCL is free software: you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * SkelCL is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at michel.steuwer@uni-muenster.de      *
 *                                                                           *
 *****************************************************************************/
 
///
/// \file Skeleton.cpp
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <stdexcept>
#include <vector>
#include <string>

#include "SkelCL/detail/Skeleton.h"

namespace {

const size_t defaultWorkGroupSize = 256;

} // namespace

namespace skelcl {

namespace detail {

Skeleton::Skeleton()
  : _events(), _workGroupSize(::defaultWorkGroupSize)
{
}

Skeleton::Skeleton(const unsigned workGroupSize)
  : _events(), _workGroupSize(workGroupSize)
{
}

Skeleton::~Skeleton()
{
  printEventTimings();
}

void Skeleton::printEventTimings() const
{
  auto i = 0;
  for (auto& e : _events) {
    e.wait();
    auto start = e.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    auto end = e.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    auto duration = (end - start) / 1000000.0;
    LOG_INFO("Event ", i++, " timing: ", duration, "ms");
  }
}

size_t Skeleton::workGroupSize() const
{
  return _workGroupSize;
}

void Skeleton::setWorkGroupSize(size_t size)
{
  _workGroupSize = size;
}

void Skeleton::prepareAdditionalInput() const
{
}

void Skeleton::updateModifiedStatus() const
{
}

} // namespace detail

} // namespace skelcl

