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


#include <climits>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/unit_test.hpp>


#include <mia/core.hh>
#include <mia/3d/3DDatafield.hh>

NS_MIA_USE
using namespace std; 

static void test_3ddatafield()
{

	const float values[80]=
		{ 0,  1,  2,  3,  4, /**/  2,  3,  4,  5,  6, /**/ 4,  5,  6,  7,  8, /**/  6,  7,  8,  9, 10,  // 100
		  1,  3,  5,  5,  5, /**/  6,  6,  6,  6,  6, /**/ 7,  7,  7,  7,  7, /**/  8,  8,  8,  8,  8,  // 124
		  2,  1,  2,  3,  4, /**/  2,  3,  4,  5,  6, /**/ 4,  5,  6,  7,  8, /**/  6,  7,  8,  9, 10,  // 102
		  1,  2,  1,  2,  3, /**/  4,  2,  3,  4,  5, /**/ 6,  4,  5,  6,  7, /**/  8,  6,  7,  9,  9 };//  94 420


	C3DFDatafield data(C3DBounds(5,4, 4), values);

	BOOST_CHECK(data.get_size()  == C3DBounds(5, 4, 4));
	BOOST_CHECK(data.size() == 80);
	BOOST_CHECK(data(0,0,0) == 0);
	BOOST_CHECK(data(2,2,3) == 5);

	C3DFVector grad_22 = data.get_gradient<float>(2,2,2);
	cvdebug() << grad_22 << "\n";
	BOOST_CHECK(fabs(grad_22.x - 1.0) < 0.0001 &&
	       fabs(grad_22.y - 2.0) < 0.0001 &&
	       fabs(grad_22.z + 1.0) < 0.0001);

	float ip = data.get_interpol_val_at(C3DFVector(2.25, 1.5, 1.5));
	BOOST_CHECK(fabs(ip - 5.875) < 0.0001);

	std::vector<float> xbuffer;
	data.get_data_line_x(2, 1, xbuffer);
	BOOST_CHECK(xbuffer.size() == 5);
	BOOST_CHECK(xbuffer[0] == 7 && xbuffer[2] == 7);

	std::vector<float> ybuffer;
	data.get_data_line_y(1, 3, ybuffer);
	BOOST_CHECK(ybuffer.size()== 4);
	BOOST_CHECK(ybuffer[0] == 2 && ybuffer[2] == 4);

	std::vector<float> zbuffer;
	data.get_data_line_z(1, 3, zbuffer);
	BOOST_CHECK(zbuffer.size()== 4);
	BOOST_CHECK(zbuffer[0] == 7 && zbuffer[3] == 6);


	float avg = data.strip_avg();
	BOOST_CHECK(avg == 5.25f);
	BOOST_CHECK(data(0,0,0) == -5.25);
	BOOST_CHECK(data(2,2,2) == 0.75);

	data.put_data_line_x(0, 0, xbuffer);
	BOOST_CHECK(data(0, 0, 0) == 7);
	BOOST_CHECK(data(2, 0, 0) == 7);

	data.put_data_line_y(2, 1, ybuffer);
	BOOST_CHECK(data(2, 0, 1) == 2);
	BOOST_CHECK(data(2, 3, 1) == 6);

	C3DFDatafield data2(data);
	BOOST_CHECK(data2(2, 0, 1) == 2);
	BOOST_CHECK(data2(2, 3, 1) == 6);

	data2.clear();
	BOOST_CHECK(data(2, 0, 1) == 2);
	BOOST_CHECK(data(2, 3, 1) == 6);
	BOOST_CHECK(data2(2, 0, 1) == 0);
	BOOST_CHECK(data2(2, 3, 1) == 0);

	C3DUIDatafield data_big_z(C3DBounds(5, 4, 20));
	C3DUIDatafield::iterator p = data_big_z.begin();
	for (size_t z = 0; z < data_big_z.get_size().z; ++z)
		for (size_t y = 0; y < data_big_z.get_size().y; ++y)
			for (size_t x = 0; x < data_big_z.get_size().x; ++x, ++p)
				*p = x + y + z;

	std::vector<unsigned int> zibuffer;
	data_big_z.get_data_line_z(0, 0, zibuffer);
	BOOST_CHECK_EQUAL(zibuffer.size(), data_big_z.get_size().z);
	for (unsigned int i = 0; i < zibuffer.size(); ++i)
		BOOST_CHECK_EQUAL(zibuffer[i] , i);




}

static C3DFDatafield create_field(const C3DBounds& size)
{
	C3DFDatafield data(size);
	int i = 0;
	for (C3DFDatafield::iterator k = data.begin(); k != data.end(); ++k, ++i)
		*k = i;
	return data;

}


static void test_3ddatafield_get_put_xy()
{
	C3DBounds size(2,3,4);
	C3DFDatafield data = create_field(size);

	C2DFDatafield plane_xy = data.get_data_plane_xy(0);

	BOOST_CHECK(plane_xy.get_size() == C2DBounds(2,3));
	BOOST_CHECK(equal(plane_xy.begin(), plane_xy.end(), data.begin()));

	data.put_data_plane_xy(1, plane_xy);

	BOOST_CHECK(equal(plane_xy.begin(), plane_xy.end(), data.begin_at(0,0,1)));
}

static void test_3ddatafield_zslice_flat()
{
	C3DBounds size(2,3,4);
	C3DFDatafield data = create_field(size);

	vector<float> slice(6); 
	
	data.read_zslice_flat(1, slice); 
	BOOST_CHECK(equal(slice.begin(), slice.end(), data.begin_at(0,0,1)));

	data.write_zslice_flat(2, slice); 
	BOOST_CHECK(equal(slice.begin(), slice.end(), data.begin_at(0,0,2)));
}


static void test_3ddatafield_get_put_xz()
{
	C3DBounds size(2,3,4);
	C3DFDatafield data = create_field(size);

	C2DFDatafield plane_xz = data.get_data_plane_xz(0);

	BOOST_CHECK(plane_xz.get_size() == C2DBounds(2,4));

	for (size_t z = 0; z < size.z; ++z)
		for (size_t x = 0; x < size.x; ++x)
			BOOST_CHECK(plane_xz(x, z) == data(x,0,z));

	data.put_data_plane_xz(1, plane_xz);

	for (size_t z = 0; z < size.z; ++z)
		for (size_t x = 0; x < size.x; ++x)
			BOOST_CHECK(plane_xz(x, z) == data(x,1,z));

}

static void test_3ddatafield_yslice_flat()
{
	C3DBounds size(2,3,4);
	C3DFDatafield data = create_field(size);

	vector<float> slice(8); 
	
	data.read_yslice_flat(1, slice); 
	auto i = slice.begin(); 
	for (size_t z = 0; z < size.z; ++z)
		for (size_t x = 0; x < size.x; ++x, ++i)
			BOOST_CHECK_EQUAL(*i, data(x,1,z));

	data.write_yslice_flat(2, slice); 
	i = slice.begin(); 
	for (size_t z = 0; z < size.z; ++z)
		for (size_t x = 0; x < size.x; ++x, ++i)
			BOOST_CHECK_EQUAL(*i, data(x,2,z));
}

static void test_3ddatafield_xslice_flat()
{
	C3DBounds size(2,3,4);
	C3DFDatafield data = create_field(size);

	vector<float> slice(12); 
	
	data.read_xslice_flat(1, slice); 
	auto i = slice.begin(); 
	for (size_t z = 0; z < size.z; ++z)
		for (size_t y = 0; y < size.y; ++y, ++i)
			BOOST_CHECK_EQUAL(*i, data(1,y,z));

	data.write_xslice_flat(0, slice); 
	i = slice.begin(); 
	for (size_t z = 0; z < size.z; ++z)
		for (size_t y = 0; y < size.y; ++y, ++i)
			BOOST_CHECK_EQUAL(*i, data(0,y,z));
}



static void test_3ddatafield_get_put_yz()
{
	C3DBounds size(2,3,4);
	C3DFDatafield data = create_field(size);

	C2DFDatafield plane_yz = data.get_data_plane_yz(0);

	BOOST_CHECK(plane_yz.get_size() == C2DBounds(3,4));

	for (size_t z = 0; z < size.z; ++z)
		for (size_t y = 0; y < size.x; ++y)
			BOOST_CHECK(plane_yz(y, z) == data(0, y, z));

	data.put_data_plane_yz(1, plane_yz);

	for (size_t z = 0; z < size.z; ++z)
		for (size_t y = 0; y < size.x; ++y)
			BOOST_CHECK(plane_yz(y, z) == data(1, y, z));
}

void add_3ddatafield_tests( boost::unit_test::test_suite* suite)
{
	suite->add( BOOST_TEST_CASE( &test_3ddatafield));
	suite->add( BOOST_TEST_CASE( &test_3ddatafield_get_put_xy));
	suite->add( BOOST_TEST_CASE( &test_3ddatafield_get_put_xz));
	suite->add( BOOST_TEST_CASE( &test_3ddatafield_get_put_yz));
	suite->add( BOOST_TEST_CASE( &test_3ddatafield_zslice_flat)); 
	suite->add( BOOST_TEST_CASE( &test_3ddatafield_yslice_flat)); 
	suite->add( BOOST_TEST_CASE( &test_3ddatafield_xslice_flat)); 
}
