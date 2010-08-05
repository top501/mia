/* -*- mia-c++  -*-
 * Copyright (c) Leipzig, Madrid 2004-2010
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

/*
\author Gert Wollny <wollny at die.upm.ed>

*/

#ifndef mia_2d_cost_ssd2_hh
#define mia_2d_cost_ssd2_hh

#include <mia/2d/imagecostbase.hh>


NS_BEGIN(mia_2dcost_ssd2)


class C2DSSDImageCost: public mia::C2DImageCostBase {
public:
	C2DSSDImageCost(const mia::C2DImageDataKey& src_key, const mia::C2DImageDataKey& ref_key,
			mia::P2DInterpolatorFactory ipf,
			float weight);
private:
	virtual double do_evaluate_with_images(const mia::C2DImage& floating, const mia::C2DImage& ref,
					       mia::C2DFVectorfield& force) const;


};

class C2DSSDCostPlugin: public mia::C2DImageCostBasePlugin {
private:
	virtual const std::string do_get_descr()const;
};

NS_END

#endif

