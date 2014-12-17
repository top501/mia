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

#include <climits>

#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

#include <mia/core/spacial_kernel.hh>
#include <mia/2d/filtertest.hh>
#include <mia/2d/filter.hh>



NS_MIA_BEGIN
using namespace std;
namespace bfs=::boost::filesystem;
using namespace boost::unit_test;

EXPORT_2DTEST void imagefiltertest2d_prepare_plugin_path()
{
	CPathNameArray filtersearchpath;
	filtersearchpath.push_back( bfs::path("2d") / bfs::path("filter"));
	filtersearchpath.push_back( bfs::path("filter"));
	C2DFilterPluginHandler::set_search_path(filtersearchpath);

	CPathNameArray kernelsearchpath;
	kernelsearchpath.push_back(bfs::path("core")/bfs::path("spacialkernel"));
	kernelsearchpath.push_back(bfs::path("..")/bfs::path("core")/bfs::path("spacialkernel"));

	C1DSpacialKernelPluginHandler::set_search_path(kernelsearchpath);
}

NS_MIA_END


