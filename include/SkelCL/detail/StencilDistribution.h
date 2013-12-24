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
/// \file StencilDistribution.h
///
/// Works with the matrix.
///
///	\author Stefan Breuer <s_breu03@uni-muenster.de>
///
#ifndef STENCILDISTRIBUTION_H_
#define STENCILDISTRIBUTION_H_

#include "Distribution.h"
#include "Padding.h"

#include <climits>

namespace skelcl {

template<typename > class Matrix;
template<typename > class Vector;
template<typename > class StencilShare;

namespace detail {

class DeviceList;

template<typename > class StencilDistribution;

template<template<typename > class C, typename T>
class StencilDistribution<C<T>> : public Distribution<C<T>> {
public:
    StencilDistribution(unsigned int north, unsigned int west, unsigned int south, unsigned int east,
            unsigned int initialIterationsBeforeFirstSwap = 1, const DeviceList& deviceList = globalDeviceList);

	template<typename U>
	StencilDistribution(const StencilDistribution<C<U>>& rhs);

	~StencilDistribution();

	bool isValid() const;

	void startUpload(C<T>& container, Event* events) const;

    void swap(const C<T>& container, unsigned int iterations) const;

	void startDownload(C<T>& container, Event* events) const;

	size_t sizeForDevice(const C<T>& container,
			const std::shared_ptr<detail::Device>& devicePtr) const;

	bool dataExchangeOnDistributionChange(Distribution<C<T>>& newDistribution);

    const unsigned int& getNorth() const;

    const unsigned int& getWest() const;

    const unsigned int& getSouth() const;

    const unsigned int& getEast() const;

private:
	bool doCompare(const Distribution<C<T>>& rhs) const;

    unsigned int _north;

    unsigned int _west;

    unsigned int _south;

    unsigned int _east;

    unsigned int _initialIterationsBeforeFirstSwap;

};

namespace stencil_distribution_helper {

template<typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
		const typename Vector<T>::size_type size, const DeviceList& devices,
        unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap);

template<typename T>
size_t sizeForDevice(const std::shared_ptr<Device>& devicePtr,
		const typename Matrix<T>::size_type size, const DeviceList& devices,
        unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap);

template<typename T>
void startUpload(Vector<T>& vector, Event* events, unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap, detail::DeviceList devices);

template<typename T>
void startUpload(Matrix<T>& vector, Event* events, unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap, detail::DeviceList devices);

template<typename T>
void swap(const Vector<T>& vector, Event* events, unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int iterations, unsigned int initialIterationsBeforeFirstSwap, detail::DeviceList devices);

template<typename T>
void swap(const Matrix<T>& vector, Event* events, unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int iterations, unsigned int initialIterationsBeforeFirstSwap, detail::DeviceList devices);

template<typename T>
void startDownload(Vector<T>& vector, Event* events, unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap, detail::DeviceList devices);

template<typename T>
void startDownload(Matrix<T>& vector, Event* events, unsigned int north, unsigned int west, unsigned int south, unsigned int east, unsigned int initialIterationsBeforeFirstSwap, detail::DeviceList devices);



} // namespace stencil_distribution_helper

} // namespace detail

} // namespace skelcl

#include "StencilDistributionDef.h"

#endif // STENCILDISTRIBUTION_H_

