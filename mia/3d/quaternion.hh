/*  -*- mia-c++  -*-
 * Copyright (c) Leipzig, Madrid 2011
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


#ifndef mia_3d_quaternion_hh
#define mia_3d_quaternion_hh

#include <ostream>
#include <mia/3d/3DVector.hh>


NS_MIA_BEGIN 

class Quaternion {

public: 
	Quaternion(); 
	Quaternion(const Quaternion& other); 
	Quaternion(const C3DDVector& rot); 
	Quaternion(double  x, double y, double z, double w); 

	friend bool operator == (const Quaternion& a, const Quaternion& b); 

	double norm() const; 
	void normalize(); 

	Quaternion inverse() const;
	C3DDVector get_euler_angles() const;

	template <typename T> 
	T3DVector<T> rotate(const T3DVector<T>& x) const; 

	Quaternion& operator += (const Quaternion& other); 
	Quaternion& operator -= (const Quaternion& other); 
	Quaternion& operator *= (const Quaternion& other); 

	void print(std::ostream& os) const; 

	double w() const; 
	double x() const; 
	double y() const; 
	double z() const; 

private:
	C3DDVector m_v; 
	double m_w; 
}; 

template <typename T> 
T3DVector<T> Quaternion::rotate(const T3DVector<T>& x) const
{
	
}



inline double Quaternion::w() const
{
	return m_w; 
}

inline double Quaternion::x() const
{
	return m_v.x; 
}

inline double Quaternion::y() const
{
	return m_v.y; 
}

inline double Quaternion::z() const
{
	return m_v.z; 
}

bool operator != (const Quaternion& a, const Quaternion& b); 

inline std::ostream& operator << (std::ostream& os, const Quaternion& a) 
{
	a.print(os); 
	return os; 
}

NS_MIA_END

#endif