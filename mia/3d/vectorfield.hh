/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2012 Gert Wollny
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef mia_3d_3dvectorfield_hh
#define mia_3d_3dvectorfield_hh

#include <mia/core/attributes.hh>
#include <mia/3d/datafield.hh>

NS_MIA_BEGIN

/**
   @ingroup basic 
   @brief a 3D vector field 
   
   This class provides a field of 3D vectors,  
*/

template <typename T>
class T3DVectorfield: public T3DDatafield<T>, public CAttributedData {
public:
	T3DVectorfield(){};
	T3DVectorfield(const C3DBounds& size):
		T3DDatafield<T>(size) {};

	T3DVectorfield(const CAttributedData& data, const C3DBounds& size):
		T3DDatafield<T>(size),
		CAttributedData(data)
	{
	}
};
/**
   @ingroup basic 
   @brief a 3D field of floating point single accuracy 3D vectors 
*/
typedef T3DVectorfield<C3DFVector>  C3DFVectorfield;

/**
   @ingroup basic 
   @brief a 3D field of floating point single accuracy 3D vectors 
*/
typedef std::shared_ptr<C3DFVectorfield > P3DFVectorfield;

/**
   @ingroup basic 
   @brief a 3D field of floating point double accuracy 3D vectors 
*/
typedef T3DVectorfield<C3DDVector>  C3DDVectorfield;

/**
   @ingroup basic 
   Concat two vector fields assuming these define transformations 
   A(x) = x - a(x) and B(x) = x - b(x), hence c(x) = a(x-b(x)) + b(x) 
   \param[in,out] lhs left input vector field and output 
   \param[in] rhs right input vector field and output 
   \returns lhs after processing 
 */
EXPORT_3D C3DFVectorfield& operator += (C3DFVectorfield& lhs, const C3DFVectorfield& rhs);

NS_MIA_END

#endif