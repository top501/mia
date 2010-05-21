/* -*- mia-c++  -*-
 *
 * Copyright (c) Madrid 2010
 * BIT, ETSI Telecomunicacion, UPM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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
 */


#include <mia/internal/autotest.hh>

#include <mia/2d/divcurlmatrix.hh>

NS_MIA_USE; 


const int dsize = 32; 
const int dsize2 = dsize/2; 
struct TransformSplineFixtureFieldBase {
	TransformSplineFixtureFieldBase():
		size(dsize + 1,dsize + 1),
		field(size),
		ipf(new C2DInterpolatorFactory(C2DInterpolatorFactory::ip_spline,
					       SHARED_PTR(CBSplineKernel) (new CBSplineKernel3())))
	{

	}
	void init() {
		C2DFVectorfield::iterator i = field.begin();
		for (int y = 0; y < (int)size.y; ++y)
			for (int x = 0; x < (int)size.x; ++x, ++i) {
				float sx = (x - dsize2) / 2.0;
				float sy = (y - dsize2) / 2.0;
				*i = C2DFVector( fx(sx, sy), fy(sx, sy)); 
				cvdebug() << sx << ", " << sy << " = " << *i << "\n"; 
			}

		SHARED_PTR(T2DInterpolator<C2DFVector> ) source(ipf->create(field));
	}
	C2DBounds size;
	C2DFVectorfield field;
	P2DInterpolatorFactory ipf;

	C2DBounds range;
protected:
	virtual float fx(float x, float y) = 0;
	virtual float fy(float x, float y) = 0;
private: 
	C2DFVector scale; 
};



struct TransformSplineFixtureConstDivergence: public TransformSplineFixtureFieldBase {
	virtual float fx(float x, float y);
	virtual float fy(float x, float y);
}; 


BOOST_FIXTURE_TEST_CASE( test_get_index_Lower_boundary_bspline3, TransformSplineFixtureConstDivergence )
{

	C2DDivCurlMatrix divcurl(ipf->get_kernel());	
	BOOST_CHECK_EQUAL(divcurl.get_index(0,0,10), 0); 
	BOOST_CHECK_EQUAL(divcurl.get_index(0,1,10), 1);
	BOOST_CHECK_EQUAL(divcurl.get_index(0,2,10), 2); 
	BOOST_CHECK_EQUAL(divcurl.get_index(0,3,10), 3);

	BOOST_CHECK_EQUAL(divcurl.get_index(1,1,10), 8);
	BOOST_CHECK_EQUAL(divcurl.get_index(1,2,10), 9);
	BOOST_CHECK_EQUAL(divcurl.get_index(1,3,10), 10);
	BOOST_CHECK_EQUAL(divcurl.get_index(1,4,10), 11);


	BOOST_CHECK_EQUAL(divcurl.get_index(2,2,10), 16);
	BOOST_CHECK_EQUAL(divcurl.get_index(2,3,10), 17);
	BOOST_CHECK_EQUAL(divcurl.get_index(2,4,10), 18);
	BOOST_CHECK_EQUAL(divcurl.get_index(2,5,10), 19);

	BOOST_CHECK_EQUAL(divcurl.get_index(3,3,10), 24);
	BOOST_CHECK_EQUAL(divcurl.get_index(3,4,10), 25);
	BOOST_CHECK_EQUAL(divcurl.get_index(3,5,10), 26);
	BOOST_CHECK_EQUAL(divcurl.get_index(3,6,10), 27);
	BOOST_CHECK_EQUAL(divcurl.get_index(3,7,10), -1);
	BOOST_CHECK_EQUAL(divcurl.get_index(3,8,10), -1);



	BOOST_CHECK_EQUAL(divcurl.get_index(19,19,20), 0); 
	BOOST_CHECK_EQUAL(divcurl.get_index(19,18,20), 1);
	BOOST_CHECK_EQUAL(divcurl.get_index(19,17,20), 2); 
	BOOST_CHECK_EQUAL(divcurl.get_index(19,16,20), 3);
	BOOST_CHECK_EQUAL(divcurl.get_index(19,15,20), -1);

	BOOST_CHECK_EQUAL(divcurl.get_index(18,18,20), 8);
	BOOST_CHECK_EQUAL(divcurl.get_index(18,17,20), 9);
	BOOST_CHECK_EQUAL(divcurl.get_index(18,16,20),10);
	BOOST_CHECK_EQUAL(divcurl.get_index(18,15,20), 11);


	BOOST_CHECK_EQUAL(divcurl.get_index(17,17,20), 16);
	BOOST_CHECK_EQUAL(divcurl.get_index(17,16,20), 17);
	BOOST_CHECK_EQUAL(divcurl.get_index(17,15,20), 18);
	BOOST_CHECK_EQUAL(divcurl.get_index(17,14,20), 19);

	BOOST_CHECK_EQUAL(divcurl.get_index(16,16,20), 24);
	BOOST_CHECK_EQUAL(divcurl.get_index(16,15,20), 25);
	BOOST_CHECK_EQUAL(divcurl.get_index(16,14,20), 26);
	BOOST_CHECK_EQUAL(divcurl.get_index(16,13,20), 27);
	BOOST_CHECK_EQUAL(divcurl.get_index(16,12,20), -1);


	BOOST_CHECK_EQUAL(divcurl.get_index(31,31,32), 0);
}


BOOST_FIXTURE_TEST_CASE( test_get_index_symetry, TransformSplineFixtureConstDivergence )
{
	C2DDivCurlMatrix divcurl(ipf->get_kernel());
	for (size_t k = 0; k < 20; ++k) 
		for (size_t l = 0; l < 20; ++l) {
			BOOST_CHECK_EQUAL(divcurl.get_index(k,l,20), 
					  divcurl.get_index(l,k,20)); 
		}
}



BOOST_FIXTURE_TEST_CASE( test_divergence_X_x_Y_x_bspline3, TransformSplineFixtureConstDivergence )
{
	init(); 
	C2DDivCurlMatrix divcurl(ipf->get_kernel());
	BOOST_CHECK_CLOSE(1.0, 1.0 + divcurl.multiply(field), 1.0);
}

float TransformSplineFixtureConstDivergence::fx(float x, float /*y*/)
{
	return 0.01 * x; 
}

float TransformSplineFixtureConstDivergence::fy(float  /*x*/, float y)
{
	return 0.01 * y; 
}

struct TransformSplineFixtureDivergence2: public TransformSplineFixtureFieldBase {
	virtual float fx(float x, float y);
	virtual float fy(float x, float y);
}; 


#if 0
BOOST_FIXTURE_TEST_CASE( test_divergence_xsq_ysq_bspline3, TransformSplineFixtureDivergence2 )
{
	init(); 
	C2DDivCurlMatrix divcurl(ipf->get_kernel());
	BOOST_CHECK_CLOSE(16, divcurl.multiply(field), 1.0);
}


float TransformSplineFixtureDivergence2::fx(float x, float /*y*/)
{
	return 0.01 * x * x; 
}

float TransformSplineFixtureDivergence2::fy(float  /*x*/, float y)
{
	return 0.01 * y * y; 
}

struct TransformSplineFixtureOnlyXConstDivField: public TransformSplineFixtureFieldBase {
	virtual float fx(float x, float /*y*/) {return x;};
	virtual float fy(float /*x*/, float /*y*/) {return 0.0;};
}; 



BOOST_FIXTURE_TEST_CASE( test_divergence_X_x_Y_0_bspline3, TransformSplineFixtureOnlyXConstDivField )
{
	init(); 
	C2DDivCurlMatrix divcurl(ipf->get_kernel());
	BOOST_CHECK_CLOSE(1.0, 1.0 + divcurl.multiply(field), 1.0);
}

struct TransformSplineFixtureOnlyYConstDivField: public TransformSplineFixtureFieldBase {
	virtual float fx(float /*x*/, float /*y*/) {return 0.0;};
	virtual float fy(float /*x*/, float y) {return y;};
}; 

BOOST_FIXTURE_TEST_CASE( test_divergence_X_0_Y_y_bspline3, TransformSplineFixtureOnlyYConstDivField )
{
	init(); 
	C2DDivCurlMatrix divcurl(ipf->get_kernel());
	BOOST_CHECK_CLOSE(1.0, 1.0 + divcurl.multiply(field), 1.0);
}

struct TransformSplineFixtureInvSqField: public TransformSplineFixtureFieldBase {
	TransformSplineFixtureInvSqField():
		r(sqrt(size.x * size.x + size.y * size.y)){}
	virtual float fx(float x, float y);
	virtual float fy(float x, float y);
private: 
	double r; 
}; 

BOOST_FIXTURE_TEST_CASE( test_divergence_InvSq_bspline3, TransformSplineFixtureInvSqField )
{
	init(); 
	C2DDivCurlMatrix divcurl(ipf->get_kernel());
	BOOST_CHECK_CLOSE(14.44161007839438, divcurl.multiply(field), 1.0);
}

float TransformSplineFixtureInvSqField::fx(float x, float y)
{
	return 1.0 / (x*x + y * y + 1); 
}

float TransformSplineFixtureInvSqField::fy(float x, float y)
{
	return 1.0 / (2 * x * x + 2 * y*y + 1); 
}

#endif

struct TransformSplineFixtureexpm2Field: public TransformSplineFixtureFieldBase {
	TransformSplineFixtureexpm2Field(){}
	virtual float fx(float x, float y);
	virtual float fy(float x, float y);

	float dfx_xx(float x, float y);
	float dfx_xy(float x, float y);
	float dfx_yy(float x, float y);

	float dfy_xx(float x, float y);
	float dfy_xy(float x, float y);
	float dfy_yy(float x, float y);

	float integrate_divcurl(float x1, float x2, float y1, float y2, int xinterv, int yinterv); 
}; 

BOOST_FIXTURE_TEST_CASE( test_divergence_expm2_bspline3, TransformSplineFixtureexpm2Field )
{
	init(); 

	// evaluated using maxima
	const double testvalue = 6.0 * M_PI; 

	C2DDivCurlMatrix divcurl(ipf->get_kernel());
	float spline = divcurl.multiply(field); 
	
	// this test is just to compare the maxima value to a approximate integration 
	   float manual = integrate_divcurl(-8, 8, -8, 8, 128, 128); 
	   BOOST_CHECK_CLOSE(manual, testvalue, 0.1); 
	
	
	BOOST_CHECK_CLOSE(spline, testvalue,  1.0);

}

float TransformSplineFixtureexpm2Field::fx(float x, float y)
{
	return exp(-x*x-y*y); 
}

float TransformSplineFixtureexpm2Field::fy(float x, float y)
{
	return exp(-x*x-y*y); 
}

float TransformSplineFixtureexpm2Field::dfx_xx(float x, float y)
{
	return (4 * x * x  - 2) * fx(x,y); 
}


float TransformSplineFixtureexpm2Field::dfx_xy(float x, float y)
{
	return 4 * x * y * fx(x,y); 
}

float TransformSplineFixtureexpm2Field::dfx_yy(float x, float y)
{
	return (4 * y * y  - 2) * fx(x,y); 
}

float TransformSplineFixtureexpm2Field::dfy_xx(float x, float y)
{
	return (4 * x * x  - 2) * fy(x,y); 
}

float TransformSplineFixtureexpm2Field::dfy_xy(float x, float y)
{
	return 4 * x * y * fy(x,y); 
}

float TransformSplineFixtureexpm2Field::dfy_yy(float x, float y)
{
	return (4 * y * y  - 2) * fy(x,y); 
}

float TransformSplineFixtureexpm2Field::integrate_divcurl(float x1, float x2, float y1, float y2, int xinterv, int yinterv)
{
	float hx = (x2 - x1) / xinterv; 
	float hy = (y2 - y1) / yinterv; 

	double sum = 0.0; 
	for (float y = y1 + hx / 2.0; y < y2; y += hy) 
		for (float x = x1 + hx / 2.0; x < x2; x += hx) {
			double vfx_xx = dfx_xx(x,y); 
			double vfx_xy = dfx_xy(x,y); 

			double vfy_xy = dfy_xy(x,y); 
			double vfy_yy = dfy_yy(x,y); 
			double v = vfx_xx  + vfx_xy  + vfy_xy + vfy_yy; 
			double v2 =  4*(y+x-1)*(y+x+1)*exp(-y*y-x*x); 
			cvinfo()<< x << "," << y << ":" << v << " vs "<< v2 << "\n"; 
			sum += v * v; 
		}
	sum *= hx * hy; 
	return sum; 
}

