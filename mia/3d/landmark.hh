/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
 * Max-Planck-Institute for Human Cognitive and Brain Science	
 * Max-Planck-Institute for Evolutionary Anthropology 
 * BIT, ETSI Telecomunicacion, UPM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PUcRPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef mia_3d_landmarks_hh
#define mia_3d_landmarks_hh

#include <mia/3d/3DVector.hh>

NS_MIA_BEGIN

/**
   \brief named a 3D landmark

   This class provides the basis for named a 3D landmark 
 */

class CLandmark {
public: 
	CLandmark(const std::string& _name, const C3DFVector& _position); 

	const std::string& get_name() const; 
	const C3DFVector& get_position() const; 
	void set_position(const C3DFVector& pos); 
private: 
	std::string m_name; 
	C3DFVector m_position; 

}; 

NS_MIA_END

#endif

