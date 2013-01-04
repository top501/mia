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

#define VSTREAM_DOMAIN "vista2dImageIOtest"

#include <mia/internal/autotest.hh>

#include <boost/mpl/vector.hpp>
#include <boost/test/test_case_template.hpp>


#include <vistaio/2dvistaio.hh>
#include <unistd.h>

using namespace mia; 
using namespace std; 
using namespace vista_2d_io; 
namespace bmpl=boost::mpl;

typedef bmpl::vector<
	bool,  
	unsigned char,
	signed short,
	unsigned short,
	signed int,
	unsigned int,
	float,
	double
	> type;


BOOST_AUTO_TEST_CASE_TEMPLATE( test_simple_write_read, T, type ) 
{
        C2DBounds size(2,3);
	T2DImage<T> *image = new T2DImage<T>(size); 
        P2DImage pimage(image); 

        auto iv = image->begin(); 
	auto ev = image->end();
        int i = 0; 

	while (iv != ev)
		*iv++ = i++;
       

	CVista2DImageIOPlugin io; 
        CVista2DImageIOPlugin::Data images;
        images.push_back(pimage); 

	stringstream filename; 
	filename << "testimage-" << __type_descr<T>::value << ".vista2d"; 

	cvdebug() << "test with " << filename.str() << "\n"; 

	BOOST_REQUIRE(io.save(filename.str(), images)); 
	
	auto loaded = io.load(filename.str()); 
	BOOST_REQUIRE(loaded); 
	
	BOOST_REQUIRE(loaded->size() == 1u); 
        const auto& ploaded = dynamic_cast<const T2DImage<T>&>(*(*loaded)[0]); 	
	iv = image->begin(); 


	auto il = ploaded.begin(); 
	
	while (iv != ev) {
		BOOST_CHECK_EQUAL(*il, *iv); 
		++iv; 
		++il; 
	}
        unlink(filename.str().c_str()); 
}
