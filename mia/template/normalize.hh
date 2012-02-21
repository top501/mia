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

#include <cmath>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/if.hpp>
#include <mia/core/filter.hh>
#include <mia/core/msgstream.hh>

NS_MIA_BEGIN

template <typename A, typename B> 
void __assert_type_equal (A& a, B& b) 
{
// if the compiler wants to instanciate this functions, A and B are different types
	BOOST_STATIC_ASSERT(sizeof(A) == 0); 
}

template <typename A> 
void __assert_type_equal (A& /*a*/, A& /*b*/) 
{
}

#define ASSERT_TYPE_EQUAL(A, B)		  \
	{ 				  \
		A *a; B *b; 		  \
		__assert_type_equal(a,b); \
	}

template <template <typename> class  Data, typename T> 
struct __eval {
	static Data<float> *apply(const Data<T> &input, double m, double v) {
		Data<float> *result = new Data<float>(input.get_size()); 
		double invv = 1.0/v; 
		transform(input.begin(), input.end(), result->begin(), 
			  [invv,m](T x){(x - m) * invv;}); 
		return result; 
	}
}; 


struct __lambda_doesnt_work {
	__lambda_doesnt_work(float _t, float _f): 
		t(_t), f(_f) {}; 
	float operator ()(bool b) const {
		return b ? t : f; 
	}
private: 
	float t,f; 
}; 


template <template <typename> class  Data> 
struct __eval<Data, bool> {
	static Data<float> *apply(const Data<bool> &input, double m, double v) {

		Data<float> *result = new Data<float>(input.get_size()); 
		float rtrue = (1.0 - m) / v; 
		float rfalse =  - m / v; 
		
		transform(input.begin(), input.end(), result->begin(),
			  [rtrue, rfalse](bool x){b ? rtrue : rfalse;});
		return result; 
	}
}; 


template <class Image> 
struct FNormalizer: public TFilter<Image *>
{
	template <typename T, template <typename> class  Data>
	typename FNormalizer::result_type operator ()(const Data<T> &image) const {
		ASSERT_TYPE_EQUAL(Image, typename Data<T>::Super); 
		double sum = 0.0; 
		double sum2 = 0.0; 
		typename Data<T>::const_iterator i = image.begin(); 
		typename Data<T>::const_iterator e = image.end(); 
		while ( i != e ) {
			sum += *i; 
			sum2 += *i * *i; 
			++i; 
		}
		double n = image.size(); 
		double m = sum / n; 
		double v = sqrt((sum2 - n * m * m) / (n - 1)); 

		mia::cvdebug() << "FNormalizer: avg = " << m << " var = " << v << "\n"; 

		if (v < 0.000001) 
			v = 1.0;
		
		return __eval<Data, T>::apply(image, m, v); 
		
	}
}; 

template <class Image>
std::shared_ptr<Image > normalize(const Image& image)
{
	FNormalizer<Image> n; 
	return std::shared_ptr<Image >(mia::filter(n, image)); 
}

NS_MIA_END
