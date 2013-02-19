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

#include <mia/core/export_handler.hh>
#include <mia/2d/splinetransformpenalty.hh>
#include <mia/core/plugin_base.cxx>
#include <mia/core/handler.cxx>


NS_MIA_BEGIN
using namespace std; 

const char *C2DSplineTransformPenalty::data_descr = "2dtransform";
const char *C2DSplineTransformPenalty::type_descr = "splinepenalty"; 

C2DSplineTransformPenalty::C2DSplineTransformPenalty(double weight):
m_weight(weight)
{
}

C2DSplineTransformPenalty::~C2DSplineTransformPenalty()
{
}

void C2DSplineTransformPenalty::initialize(const C2DBounds& size, const C2DFVector& range, PSplineKernel kernel)
{
	m_size = size;
	m_range = range; 
	m_kernel = kernel; 
	do_initialize(); 
}


double C2DSplineTransformPenalty::value(const C2DFVectorfield&  coefficients) const
{
	assert(coefficients.get_size() == get_size()); 
	return m_weight * do_value(coefficients); 
}


double C2DSplineTransformPenalty::value_and_gradient(const C2DFVectorfield&  coefficients, CDoubleVector& gradient) const
{
	assert(coefficients.get_size() == get_size()); 
	assert(coefficients.size() * 2 == gradient.size()); 

	double value =  m_weight * do_value_and_gradient(coefficients, gradient); 
	transform(gradient.begin(), gradient.end(), gradient.begin(), 
		  [this](double x) { return - m_weight * x;}); 
	return value; 
}

const C2DBounds& C2DSplineTransformPenalty::get_size() const
{
	return m_size; 
}

const C2DFVector& C2DSplineTransformPenalty::get_range() const
{
	return m_range; 
}

PSplineKernel C2DSplineTransformPenalty::get_kernel() const
{
	return m_kernel; 
}

double C2DSplineTransformPenalty::get_weight() const
{
	return m_weight; 
}

C2DSplineTransformPenalty *C2DSplineTransformPenalty::clone() const
{
	return do_clone(); 
}

C2DSplineTransformPenaltyPluginHandler::ProductPtr produce_2d_spline_transform_penalty(const string& descr)
{
	return C2DSplineTransformPenaltyPluginHandler::instance().produce(descr); 
}


C2DSplineTransformPenaltyPlugin::C2DSplineTransformPenaltyPlugin(char const * const  name):
	TFactory<C2DSplineTransformPenalty>(name), 
	m_weight(1.0)
{
	add_parameter("weight", new CFloatParameter(m_weight, 0.0f, std::numeric_limits<float>::max(), 
						    false, "weight of penalty energy"));
}

C2DSplineTransformPenaltyPlugin::Product *C2DSplineTransformPenaltyPlugin::do_create() const
{
	return do_create(m_weight); 
}


template<> const  char * const 
TPluginHandler<C2DSplineTransformPenaltyPlugin>::m_help = 
	"This class provides the penalty put on spline based transformations  "
	"when considering image registration. This penalty term ensures "
	"the smoothness of a transformation or the evaluation of a similarity measure "
	"between a transformed study data set and a refernce data set.";


EXPLICIT_INSTANCE_DERIVED_FACTORY_HANDLER(C2DSplineTransformPenalty, C2DSplineTransformPenaltyPlugin); 

C2DSplineTransformPenaltyPluginHandlerTest::C2DSplineTransformPenaltyPluginHandlerTest()
{
	CPathNameArray plugpath({bfs::path("splinepenalty")});
	C2DSplineTransformPenaltyPluginHandler::set_search_path(plugpath);

}

NS_MIA_END
