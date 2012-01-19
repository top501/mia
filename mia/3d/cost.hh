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

#ifndef mia_3d_cost_hh
#define mia_3d_cost_hh

#include <mia/core/cost.hh>
#include <mia/3d/3DImage.hh>

NS_MIA_BEGIN

typedef TCost<C3DImage, C3DFVectorfield> C3DImageCost;
typedef std::shared_ptr<C3DImageCost > P3DImageCost;
typedef TFactory<C3DImageCost> C3DImageCostPlugin;
typedef THandlerSingleton<TFactoryPluginHandler<C3DImageCostPlugin> > C3DImageCostPluginHandler;

struct EXPORT_3D C3DImageCostPluginHandlerPathInit {
	C3DImageCostPluginHandlerPathInit(); 
}; 

NS_MIA_END

#endif
