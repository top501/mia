/* -*- mona-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
 *
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

#ifndef mia_internal_dimtrait_hh
#define mia_internal_dimtrait_hh

NS_MIA_BEGIN

struct dimension_traits_placeholder {
	typedef std::shared_ptr<dimension_traits_placeholder> Pointer;
}; 

template <int Dimensions> 
struct dimension_traits {
	typedef dimension_traits_placeholder Transformation; 
	typedef dimension_traits_placeholder PTransformation; 
	typedef dimension_traits_placeholder Size; 
	typedef dimension_traits_placeholder Image; 
	typedef dimension_traits_placeholder PImage; 
	typedef dimension_traits_placeholder ImageSeries; 
	typedef dimension_traits_placeholder PTransformationFactory; 
	typedef dimension_traits_placeholder FullCostList; 
	typedef dimension_traits_placeholder PFullCost; 
	typedef dimension_traits_placeholder Filter; 
	typedef dimension_traits_placeholder FilterPluginHandler;
	typedef dimension_traits_placeholder InterpolatorFactory; 
}; 

NS_MIA_END

#endif
