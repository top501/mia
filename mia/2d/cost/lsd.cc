/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2013 Gert Wollny
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

#include <mia/2d/cost/lsd.hh>


NS_BEGIN(mia_2dcost_lsd)

NS_MIA_USE;
using namespace std;
using namespace boost;

template class TLSDImageCost<mia::C2DImageCost>;
template class TLSDImageCostPlugin<mia::C2DImageCostPlugin, C2DLSDImageCost>; 

extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	return new C2DLSDCostPlugin();
}

NS_END
