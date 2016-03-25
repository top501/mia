/* -*- mia-c++  -*-
 *
 * This file is part of MIA - a toolbox for medical image analysis 
 * Copyright (c) Leipzig, Madrid 1999-2015 Gert Wollny
 *
 * MIA is free software; you can redistribute it and/or modify
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
 * along with MIA; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __pixel_h
#define __pixel_h

template <class T> class TPixel {
	T value;
public:
	TPixel():value(0){}
	TPixel(T _value):value(_value){};
	TPixel(const TPixel& org):value(org.value){}
	operator T() { return value;}
};

#endif
	

/* CVS LOG

   $Log: Pixel.hh,v $
   Revision 1.2  2002/06/20 09:59:47  gerddie
   added cvs-log entry


*/

