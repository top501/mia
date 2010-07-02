/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2010
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef mia_2d_filter_fifiof_median_hh
#define mia_2d_filter_fifiof_median_hh

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <memory>
#include <mia/3d/2dimagefifofilter.hh>

NS_BEGIN(median_2dstack_filter)



class C2DMedianFifoFilter : public mia::C2DImageFifoFilter {
public:
	C2DMedianFifoFilter(size_t hwidth);

	typedef mia::C2DImage *result_type;

	template <typename T>
	mia::C2DImage *operator()(const mia::T3DImage<T>& buffer) const;

	template <typename T>
	mia::C2DImage *operator()(const mia::T2DImage<T>& input);
private:
	void do_push(::boost::call_traits<mia::P2DImage>::param_type x);
	void do_initialize(::boost::call_traits<mia::P2DImage>::param_type x);
	mia::P2DImage do_filter();
	void do_shift();
	void shift_buffer();
	mia::C2DBounds _M_slice_size;
	std::auto_ptr<mia::C3DImage> _M_buffer;
	size_t _M_hw;

	C2DImageFifoFilter::CShiftSlices _M_ss;
};

NS_END
#endif
