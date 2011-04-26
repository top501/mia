/* -*- mia-c++  -*-
 *
 * Copyright (c) 2005-2011
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <limits>
#include <sstream>

#include <mia/2d/2dfilter.hh>
#include <mia/2d/filter/thresh.hh>

NS_BEGIN(thresh_2dimage_filter)

NS_MIA_USE;
using namespace std; 

C2DThreshNImageFilter::C2DThreshNImageFilter(mia::P2DShape shape, double thresh):
	m_shape(shape), 
	m_thresh(thresh)
{
	
}

template <typename T>
typename C2DFilter::result_type C2DThreshNImageFilter::operator () (const mia::T2DImage<T>& image)const
{
	T2DImage<T> *presult  = new T2DImage<T>(image); 
	P2DImage result(presult); 
	auto  ir = presult->begin(); 
	auto  ii = image.begin(); 
	
	for(size_t y = 0; y < image.get_size().y; ++y) 
		for(size_t x = 0; x < image.get_size().x; ++x, ++ir, ++ii) {
			bool clear = true; 
			for (auto sb = m_shape->begin(); clear && sb !=  m_shape->end(); ++sb) {
				C2DBounds loc(x + sb->x, y + sb->y);
				if (loc < image.get_size()) 
					clear = image(loc) < m_thresh; 
			}
			if (clear) 
				*ir =  T();
		}
	return result; 
}



P2DImage C2DThreshNImageFilter::do_filter(const C2DImage& src) const
{
	return mia::filter(*this, src); 
}

C2DThreshNImageFilterFactory::C2DThreshNImageFilterFactory():
	C2DFilterPlugin("thresh"),
	m_shape("n4"), 
	m_thresh(5.0)
{
	add_parameter("shape", new CStringParameter(m_shape, false, "neighborhood shape"));
	add_parameter("thresh", new CDoubleParameter(m_thresh, 
						     -numeric_limits<double>::max(), 
						     numeric_limits<double>::max(),
						     false, "The threshhold value"));
}

C2DFilterPlugin::ProductPtr C2DThreshNImageFilterFactory::do_create()const
{
	auto shape = C2DShapePluginHandler::instance().produce(m_shape); 
	return C2DFilterPlugin::ProductPtr(new C2DThreshNImageFilter(shape, m_thresh));
}

const std::string C2DThreshNImageFilterFactory::do_get_descr() const
{
	return  "This filter sets all pixels of an image to zero that fall below a certain threshhold and "
		"whose neighbours in a given neighborhood shape also fall below a this threshhold"; 
}

extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	return new C2DThreshNImageFilterFactory(); 
}

NS_END
