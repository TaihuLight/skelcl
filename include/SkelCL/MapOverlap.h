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
/// \file MapOverlap.h
///
///	\author Stefan Breuer<s_breu03@uni-muenster.de>
///
#ifndef MapOverlap_H_
#define MapOverlap_H_

#include <istream>
#include <string>

#include "detail/Padding.h"
#include "detail/Skeleton.h"
#include "detail/Program.h"

namespace skelcl {

template<typename > class Matrix;
template<typename > class Out;

template<typename > class MapOverlap;

template<typename Tin, typename Tout>
class MapOverlap<Tout(Tin)> : public detail::Skeleton {

public:
	//Constructors
	MapOverlap<Tout(Tin)>(const Source& source, unsigned int overlap_range = 1,
			detail::Padding padding = detail::Padding::NEAREST,
			Tin neutral_element = Tin(),
			const std::string& func = std::string("func"));

	// Ausführungsoperator
	template<typename ... Args>
	Matrix<Tout> operator()(const Matrix<Tin>& in, Args&&... args);

	// Ausführungsoperator mit Referenz
	template<typename ... Args>
	Matrix<Tout>& operator()(Out<Matrix<Tout> > output, const Matrix<Tin>& in,
			Args&&... args);

private:
	// Ausführen
	template<typename ... Args>
	void execute(Matrix<Tout>& output, const Matrix<Tin>& in, Args&&... args);

	// Programm erstellen
	detail::Program createAndBuildProgram() const;

	// Eingabe vorbereiten
	void prepareInput(const Matrix<Tin>& in);

	// Ausgabe vorbereiten
	void prepareOutput(Matrix<Tout>& output, const Matrix<Tin>& in);

	/// Source code as defined by the application developer
	std::string _userSource;

	/// The range, in which the surrounding elements are to be included in the calculation
	unsigned int _overlap_range;

	/// Determines the padding method.
	detail::Padding _padding;

	/// The neutral element, which shall be used to fill the overlap region (only used if detail::Padding == NEUTRAL)
	Tin _neutral_element;

	/// Name of the main function defined in _userSource
	std::string _funcName;

	detail::Program _program;
};

} //namespace skelcl

#include "detail/MapOverlapDef.h"

#endif /* MAPOVERLAP_H_ */
