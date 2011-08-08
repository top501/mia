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

#include <sstream> 
#include <mia/internal/autotest.hh>
#include <mia/core/labelmap.hh>

NS_MIA_USE

using std::istringstream; 
using std::ostringstream; 

BOOST_AUTO_TEST_CASE ( test_labelmap ) 
{
	CLabelMap map; 

	map[2] = 3; 
	map[4] = 2; 
	map[9] = 7; 

	ostringstream os; 
	map.save(os); 

	istringstream is(os.str()); 

	CLabelMap new_map(is); 
	
	BOOST_CHECK_EQUAL(new_map.size(), map.size()); 

	BOOST_CHECK_EQUAL(map[2], 3); 
	BOOST_CHECK_EQUAL(map[4], 2); 
	BOOST_CHECK_EQUAL(map[9], 7); 
}

