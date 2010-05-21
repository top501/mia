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

#include <cassert>
#include <iomanip>

#include <mia/2d/divcurlmatrix.hh>


NS_MIA_BEGIN
using namespace std; 

struct C2DDivCurlMatrixImpl {
	C2DDivCurlMatrixImpl(const CBSplineKernel* kernel); 

	double multiply(const C2DFVectorfield& coefficients) const; 
	C2DFVectorfield multiply_for_gradient(const C2DFVectorfield& coefficients) const; 
	int get_index(int n1, int n2, int size) const; 
	double value_at(const C2DFVectorfield& coefficients, size_t m, size_t n)const; 
private: 

	size_t ksize;
	int hsupport_size; 
	vector<double> r20; 
	vector<double> r11; 
	vector<double> r02; 
	
}; 




C2DDivCurlMatrix::C2DDivCurlMatrix(const CBSplineKernel* kernel):
	impl(new C2DDivCurlMatrixImpl(kernel))
{
	
}

C2DDivCurlMatrix::~C2DDivCurlMatrix() 
{
	delete impl; 
}

double C2DDivCurlMatrix::multiply(const C2DFVectorfield& coefficients) const
{
	return impl->multiply(coefficients); 
}

C2DFVectorfield C2DDivCurlMatrix::multiply_for_gradient(const C2DFVectorfield& coefficients) const
{
	return impl->multiply_for_gradient(coefficients); 
}

double C2DDivCurlMatrix::value_at(const C2DFVectorfield& coefficients, size_t m, size_t n)const
{
	return impl->value_at(coefficients, m, n); 
}

C2DDivCurlMatrixImpl::C2DDivCurlMatrixImpl(const CBSplineKernel* kernel)
{
	assert(kernel); 

	ksize = 2*kernel->size()-1; 
	hsupport_size = kernel->size() - 1; 
	
	r20.resize(ksize); 
	r11.resize(ksize); ; 
	r02.resize(ksize); ; 

	double nx = 1.0; // field_size.x; 
	double ny = 1.0; // field_size.y; 

	size_t idx = 0; 

	// this part is wrong 
	for(size_t i = 0; i < ksize; ++i) {
		r20[i] = integrate2(*kernel, ksize, ksize + i, 2, 0, nx, 0, 2*ksize);
		r11[i] = integrate2(*kernel, ksize, ksize + i, 1, 1, nx, 0, 2*ksize);
		r02[i] = integrate2(*kernel, ksize, ksize + i, 0, 2, nx, 0, 2*ksize);
		cvdebug() << setw(15) << r20[i] 
			  << setw(15) << r11[i] 
			  << setw(15) << r02[i] << "\n"; 
	}
	
}

double C2DDivCurlMatrixImpl::value_at(const C2DFVectorfield& coefficients, size_t m, size_t n)const
{
	double v = 0.0;
	for(int l = -hsupport_size; l <= hsupport_size; ++l) {
		const size_t nl = l + n; 
		if (nl >= coefficients.get_size().y) 
			continue; 
		for(int k = -hsupport_size; k <= hsupport_size; ++k) {
			const size_t km = k + m; 
			if (km < coefficients.get_size().x) {
				const C2DFVector& cmn = coefficients(km,nl); 
				const int hk = abs(k -l);
				
				cvdebug() << cmn << km << ", " << nl << ": " << hk << ": " << r20[hk] << ", " 
					  << r11[hk] << ", " << r02[hk] << "\n"; 

				v +=    cmn.x * (r20[hk] + r11[hk]) + 
					cmn.y * (r02[hk] + r11[hk]); 
			}
		}
		
	}
	return v; 	
}

/*
  The spline approximation of this evaluation generally breaks if the function 
  has an artificial non-zero second order derivative because of the cut-off at the 
  boundary. 
  In non-rigid image registration this is usually nota big deal, since here the cost function
  is normally also close to zero, 
*/

double C2DDivCurlMatrixImpl::multiply(const C2DFVectorfield& coefficients) const
{
	double sum = 0.0; 

	for(size_t n = 0; n < coefficients.get_size().y; ++n) {
		for(size_t m = 0; m < coefficients.get_size().x; ++m) {
			double v = value_at(coefficients, m, n); 
			sum += v*v; 
		}
	}
	// there should be some scaling with the integration area here
	return sum; 
}


C2DFVectorfield C2DDivCurlMatrixImpl::multiply_for_gradient(const C2DFVectorfield& coefficients) const
{

#if 0
	C2DFVectorfield result(coefficients.get_size()); 
	
	C2DFVectorfield::iterator r   = result.begin();
	C2DFVectorfield::const_iterator cmn = coefficients.begin();
	for(size_t n = 0; n < coefficients.get_size().y; ++n) {
		for(size_t m = 0; m < coefficients.get_size().x; ++m, ++cmn, ++r) {
			*r = C2DFVector(0,0); 
			C2DFVectorfield::const_iterator ckl = coefficients.begin();
			for(size_t l = 0; l < coefficients.get_size().y; ++l)
				for(size_t k = 0; k < coefficients.get_size().x; ++k, ++ckl) {

					int km = get_index(k, m, coefficients.get_size().x);
					int ln = get_index(l, n, coefficients.get_size().y);
					if (km < 0 || ln < 0 ) 
						continue; 
					const double r1111 = r11x[km] * r11y[ln]; 
					
					const double q11 =      r20x[km] * r20y[ln] + r1111;
					const double q12 = 2 * (r20x[km] * r11y[ln] + r11x[km] * r02y[ln]);
					const double q22 =      r02x[km] * r02y[ln] + r1111;

					r->x += (n == l && m == k) ? 2 * ckl->x * q11 : ckl->x * q11; 
					r->x += ckl->y * q12;
					
					r->y += ckl->x * q12;
					r->y += (n == l && m == k) ? 2 * ckl->y * q11 : ckl->y * q22; 
				}
		}

	}
	return result; 
#endif 
}

NS_MIA_END
