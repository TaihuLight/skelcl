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
/// \file Util.h
///
/// \author Michel Steuwer <michel.steuwer@uni-muenster.de>
///

#include <string>

#include <cxxabi.h>

#ifndef UTILITIES_H_
#define UTILITIES_H_

namespace skelcl {

namespace detail {

namespace util {

#if 0
#ifdef __APPLE__
std::string getResourcePath(const std::string& name,
                            const std::string& extension);
#endif
#endif

std::string envVarValue(const std::string& envVar);

std::string hash(const std::string& text);

size_t devideAndRoundUp(size_t i, size_t j);

size_t devideAndAlign(size_t i, size_t j, size_t a);

size_t ceilToMultipleOf(size_t i, size_t j);

bool isPowerOfTwo(size_t n);

int floorPow2(int n);

int ceilPow2(int n);

template<typename T>
std::string typeToString() {
  char* cName = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, NULL);
  std::string name(cName);
  free(cName);
  return name;
}

} // namespace util

} // namespace detail

} // namespace skelcl

#endif // UTILITIES_H_
