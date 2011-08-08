/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2011 Gert Wollny
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

#include <limits>
#include <mia/core/filter.hh>
#include <mia/core/msgstream.hh>
#include <mia/2d/2dfilter.hh>


NS_BEGIN(adaptmed_2dimage_filter)

class C2DAdaptMedian : public mia::C2DFilter {
public:
	C2DAdaptMedian(int hw);

	template <class Data2D>
	typename C2DAdaptMedian::result_type operator () (const Data2D& data) const ;

private:
	C2DAdaptMedian::result_type do_filter(const mia::C2DImage& image) const;
	int m_hw;
};

class C2DAdaptMedianImageFilterFactory: public mia::C2DFilterPlugin {
public:
	C2DAdaptMedianImageFilterFactory();
	virtual mia::C2DFilter *do_create()const;
	virtual const std::string do_get_descr() const;
private:
	int m_hw;
};

NS_END
