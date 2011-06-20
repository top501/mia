/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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

#include <mia/internal/autotest.hh>
#include <mia/3d/landmarklist.hh>
#include <stdexcept>

NS_MIA_USE
using std::string; 
using std::invalid_argument; 

BOOST_AUTO_TEST_CASE ( test_landmark ) 
{
	string n("name"); 
	C3DFVector pos(1,2,3); 
	C3DLandmark lm(n, pos); 

	BOOST_CHECK_EQUAL(n , lm.get_name()); 
	BOOST_CHECK_EQUAL(pos, lm.get_position()); 

}


BOOST_AUTO_TEST_CASE ( test_landmark_list ) 
{
	C3DLandmarklist lml; 
	
	string n1("name1"); 
	C3DFVector pos1(1,2,3); 
	P3DLandmark lm1(new C3DLandmark(n1, pos1));

	string n2("name2"); 
	C3DFVector pos2(2,4,6); 
	P3DLandmark lm2(new C3DLandmark(n2, pos2));
	
	lml.add(lm1); 
	lml.add(lm2); 
	
	BOOST_CHECK_THROW(lml.get("name3"), invalid_argument); 
	BOOST_CHECK_EQUAL(lml.get(n1)->get_position(), pos1); 
	BOOST_CHECK_EQUAL(lml.get(n2)->get_position(), pos2); 
	
	P3DLandmark lm3(new C3DLandmark(n2, pos1));
	lml.add(lm3); 
	BOOST_CHECK_EQUAL(lml.get(n2)->get_position(), pos1); 
}