/* -*- mia-c++  -*-
 *
 * This file is part of MIA - a toolbox for medical image analysis 
 * Copyright (c) Leipzig, Madrid 1999-2014 Gert Wollny
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

#include <mia/internal/autotest.hh>
#include <mia/3d/fifof/gauss.hh>
#include <mia/3d/fifotestfixture.hh>

NS_USE(gauss_2dstack_filter);
NS_MIA_USE;

namespace bfs=::boost::filesystem;

struct GaussFixture: public fifof_Fixture {
	GaussFixture() {
		CPathNameArray kernelsearchpath;
		kernelsearchpath.push_back(bfs::path("..")/bfs::path("..")/
					   bfs::path("core")/bfs::path("spacialkernel"));
		C1DSpacialKernelPluginHandler::set_search_path(kernelsearchpath);

		CPathNameArray selfpath;
		selfpath.push_back(bfs::path("..")/bfs::path("..")/
				   bfs::path("2d")/bfs::path("filter"));
		C2DFilterPluginHandler::set_search_path(selfpath);

	}
};


BOOST_FIXTURE_TEST_CASE( test_fifof_gauss , GaussFixture )
{
	const size_t slices = 4;
	const C2DBounds size(3,3);

	float input_data[slices][9] = {
		{  0, 0, 0, 0, 0, 0, 0, 0, 0},
		{  0, 0, 0, 0, 0, 0, 0, 0, 0},
		{  0, 0, 0, 0, 64.0, 0, 0, 0, 0},
		{  0, 0, 0, 0, 0, 0, 0, 0, 0}
	};

	float test_data[slices][9] = {
		{  0, 0, 0, 0, 0, 0, 0, 0, 0},
		{  1, 2, 1, 2, 4, 2, 1, 2, 1},
		{  2, 4, 2, 4, 8, 4, 2, 4, 2},
		{  1, 2, 1, 2, 4, 2, 1, 2, 1}
	};

	C2DGaussFifoFilter f(1);


	for (size_t i = 0; i < slices; ++i) {
		m_in_data.push_back(P2DImage(new C2DFImage(size, input_data[i])));
		m_test_data.push_back(P2DImage(new C2DFImage(size, test_data[i])));
	}

	call_test(f);
}

