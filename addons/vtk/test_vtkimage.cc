/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2012 Gert Wollny
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

#define VSTREAM_DOMAIN "vtkImageIOtest"

#include <mia/internal/autotest.hh>

#include <boost/mpl/vector.hpp>
#include <boost/test/test_case_template.hpp>


#include <vtk/vtkimage.hh>
#include <unistd.h>

using namespace mia; 
using namespace std; 
using namespace vtkimage; 
namespace bmpl=boost::mpl;

// LONG_64BIT seems to be buggy in vtkDataReader
#ifdef LONG_64BIT
#undef LONG_64BIT
#endif 


typedef bmpl::vector<
	unsigned char,
	signed short,
	unsigned short,
	signed int,
	unsigned int,
	float,
	double
#ifdef LONG_64BIT
	,long, unsigned long
#endif
		     > type;


BOOST_AUTO_TEST_CASE_TEMPLATE( test_simple_write_read, T, type ) 
{
        C3DBounds size(2,3,4);
	T3DImage<T> *image = new T3DImage<T>(size); 
        P3DImage pimage(image); 

        auto iv = image->begin(); 
	auto ev = image->end();
        int i = 0; 

	while (iv != ev)
		*iv++ = i++;
       

	CVtk3DImageIOPlugin io; 
        CVtk3DImageIOPlugin::Data images;
        images.push_back(pimage); 

	stringstream filename; 
	filename << "testimage-" << __type_descr<T>::value << ".vtk"; 

	cvdebug() << "test with " << filename.str() << "\n"; 

	BOOST_REQUIRE(io.save(filename.str(), images)); 
	
	auto loaded = io.load(filename.str()); 
	BOOST_REQUIRE(loaded); 
	
	BOOST_REQUIRE(loaded->size() == 1u); 
        const auto& ploaded = dynamic_cast<const T3DImage<T>&>(*(*loaded)[0]); 	
	iv = image->begin(); 


	auto il = ploaded.begin(); 
	
	while (iv != ev) {
		BOOST_CHECK_EQUAL(*il, *iv); 
		++iv; 
		++il; 
	}
        unlink(filename.str().c_str()); 
}


