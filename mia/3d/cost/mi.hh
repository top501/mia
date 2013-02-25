/* -*- mia-c++  -*-
 *
 * This file is part of MIA - a toolbox for medical image analysis 
 * Copyright (c) Leipzig, Madrid 1999-2013 Gert Wollny
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

#ifndef mia_3d_cost_mi_hh
#define mia_3d_cost_mi_hh

#include <mia/3d/cost.hh>

#define NS mia_3dcost_mi
#include <mia/template/mi.hh>

NS_BEGIN(NS)

typedef TMIImageCost<mia::C3DImageCost> C3DMIImageCost;
typedef TMIImageCostPlugin<mia::C3DImageCostPlugin, mia::C3DImageCost> C3DMICostPlugin; 

NS_END


#endif
