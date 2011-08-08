/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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

#include <mia/3d/landmark.hh>

NS_MIA_BEGIN

C3DLandmark::C3DLandmark(const std::string& _name, const C3DFVector& _position):
	m_name(_name), 
	m_position(_position) 
{
}

const std::string& C3DLandmark::get_name() const
{
	return m_name; 
}

const C3DFVector& C3DLandmark::get_position() const
{
	return m_position; 
}

void C3DLandmark::set_position(const C3DFVector& pos)
{
	m_position = pos; 
}

	
NS_MIA_END
