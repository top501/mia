/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PUcRPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <mia/core/shared_ptr.hh>
#include <mia/internal/autotest.hh>
#include <mia/2d/filter/bandpass.hh>

NS_MIA_USE
using namespace std;
using namespace ::boost;
using namespace ::boost::unit_test;

BOOST_AUTO_TEST_CASE( test_bandpass )
{
	const size_t size = 5;
	const float src_data[25] = { 1.0f,  2.0f, 2.1f, -1.0f, 2.0f,
				     11.0f, 21.0f, 3.1f, -4.0f, 4.0f,
				     5.0f,  1.0f, 6.1f,-21.2f,22.0f,
				     11.0f, 12.2f, 8.1f, -5.0f,24.0f,
				     1.3f,  2.3f, 9.1f, -6.2f, 2.2f};

	const float ref_data[25] = { 1.0f,  2.0f, 2.1f, -1.0f, 2.0f,
				     0.0f,  0.0f, 3.1f, -4.0f, 4.0f,
				     5.0f,  1.0f, 0.0f,  0.0f, 0.0f,
				     0.0f,  0.0f, 0.0f, -5.0f, 0.0f,
				     1.3f,  2.3f, 0.0f,  0.0f, 2.2f};
	const float min = -6.0;
	const float max =  5.0;

	C2DFImage *src = new C2DFImage(C2DBounds(size, size));

	C2DFImage::iterator f = src->begin();
	for (size_t i = 0; i < 25; ++i, ++f)
		*f = src_data[i];

	P2DImage srcw(src);

	C2DImageBandPass filter(min, max);

	P2DImage res = filter.filter(*srcw);

	BOOST_CHECK_EQUAL(res->get_pixel_type(), it_float);

	C2DFImage *resi = dynamic_cast<C2DFImage *>(res.get());
	BOOST_REQUIRE(resi);

	BOOST_CHECK_EQUAL(resi->get_size(), src->get_size());

	f = resi->begin();
	for (size_t i = 0; i < 25; ++i, ++f)
		BOOST_CHECK_CLOSE(*f, ref_data[i], 0.1);
}
