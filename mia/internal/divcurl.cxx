/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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

#include <limits>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

NS_MIA_BEGIN
template <typename T> 
TDivCurlFullCost<T>::TDivCurlFullCost(double weight_div, double weight_curl, double weight):
	TFullCost<T>(weight), 
	m_weight_div(weight_div), 
	m_weight_curl(weight_curl), 
	m_size_scale(1.0)
{
	this->add(::mia::property_gradient); 
}

template <typename T> 
double TDivCurlFullCost<T>::do_evaluate(const T& t, CDoubleVector& gradient) const
{
	assert(t.get_size() == this->get_current_size()); 
	double result = t.get_divcurl_cost(m_size_scale * m_weight_div, m_size_scale *m_weight_curl, gradient); 
	cvdebug() << "TDivCurlFullCost<T>::value = " << result << "\n"; 
	transform(gradient.begin(), gradient.end(), gradient.begin(), -1.0 * boost::lambda::_1); 
	return result; 
}

template <typename T> 
double TDivCurlFullCost<T>::do_value(const T& t) const
{
	double result = t.get_divcurl_cost(m_size_scale * m_weight_div, m_size_scale * m_weight_curl); 
	cvdebug() << "TDivCurlFullCost<T>::value = " << result << "\n"; 
	return result; 
}

template <typename T> 
double TDivCurlFullCost<T>::do_value() const
{
	cvwarn() << "Requesting DivCurl cost without a transformation doesn't make sense\n"; 
	return 0.0; 
}

template <typename T> 
void TDivCurlFullCost<T>::do_set_size()
{
	m_size_scale = 1.0 / (this->get_current_size().product()); 
}

template <typename T> 
bool TDivCurlFullCost<T>::do_get_full_size(Size& size) const
{
	return true; 
}


template <typename T> 
TDivcurlFullCostPlugin<T>::TDivcurlFullCostPlugin():
	TFullCostPlugin<T>("divcurl"), 
	m_div(1.0), 
	m_curl(1.0)
{
	this->add_parameter("div", new CFloatParameter(m_div, 0.0f, numeric_limits<float>::max(), 
						 false, "penalty weight on divergence"));
	this->add_parameter("curl", new CFloatParameter(m_curl, 0.0f, numeric_limits<float>::max(), 
						  false, "penalty weight on curl"));
}

template <typename T> 
TFullCost<T> *TDivcurlFullCostPlugin<T>::do_create(float weight) const
{
	cvdebug() << "create C2DDivCurlFullCost with weight= " << weight 
		  << " div=" << m_div << " curl=" << m_curl << "\n"; 
		
	return new TDivCurlFullCost<T>(this->m_div, this->m_curl, weight); 
}

template <typename T> 
const std::string TDivcurlFullCostPlugin<T>::do_get_descr() const
{
	return "divcurl penalty cost function"; 
}

NS_MIA_END



