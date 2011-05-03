/* -*- mona-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2009-2011
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

#include <mia/internal/autotest.hh>
#include <mia/2d/similarity_profile.hh>
#include <mia/2d/2dimageio.hh>
#include <mia/2d/cost.hh>
#include <mia/2d/2dfilter.hh>


namespace bfs=::boost::filesystem;
NS_MIA_USE; 

struct SimityProfileFixture {
	SimityProfileFixture(); 

	P2DFullCost cost; 
	C2DImageSeries series; 

}; 


BOOST_FIXTURE_TEST_CASE (test_C2DSimilarityProfile_ref10, SimityProfileFixture) 
{
	
	C2DSimilarityProfile sp(cost, series, 10); 
	// test value obtained by using octave 
	BOOST_CHECK_CLOSE(sp.get_peak_frequency(), 108.98704, 0.1);
	
}



SimityProfileFixture::SimityProfileFixture()
{
	list< bfs::path> cost_plugpath;
	cost_plugpath.push_back(bfs::path("cost"));
	C2DImageCostPluginHandler::set_search_path(cost_plugpath);

	list< bfs::path> filter_plugpath;
	filter_plugpath.push_back(bfs::path("filter"));
	C2DFilterPluginHandler::set_search_path(filter_plugpath);

	list< bfs::path> io_plugpath;
	io_plugpath.push_back(bfs::path("io"));
	C2DImageIOPluginHandler::set_search_path(io_plugpath);

	list< bfs::path> fullcost_plugpath;
	fullcost_plugpath.push_back(bfs::path("fullcost"));
	C2DFullCostPluginHandler::set_search_path(fullcost_plugpath);


	cost = C2DFullCostPluginHandler::instance().produce("image");

	C2DBounds size(1,1); 
	float values[40] = {
		-2, -3, -4, -1, 1, 4, 2, -1, -3, -2, 
		-2, -3, -4, -1, 1, 4, 2, -1, -3, -2, 
		-2, -3, -4, -1, 1, 4, 2, -1, -3, -2, 
		-2, -3, -4, -1, 1, 4, 2, -1, -3, -2 }; 
 
	for(size_t i = 0; i < 40; ++i) {
		C2DFImage *img = new C2DFImage(size); 
		(*img)(0,0) = values[i]; 
		series.push_back(P2DImage(img)); 
	}
}

