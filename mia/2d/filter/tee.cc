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

#include <mia/2d/filter/tee.hh>
#include <mia/2d/2dimageio.hh>

NS_BEGIN( tee_2dimage_filter)

NS_MIA_USE; 

C2DTee::C2DTee(const std::string& name):
	m_name(name)
{
}

template <class T>
C2DTee::result_type C2DTee::operator () (const T2DImage<T>& data) const
{
	TRACE_FUNCTION; 
	P2DImage result(new T2DImage<T>(data)); 
	save_image(m_name, result); 
	return result; 
}


mia::P2DImage C2DTee::do_filter(const mia::C2DImage& image) const
{
	return mia::filter(*this, image); 
	
}

mia::P2DImage C2DTee::do_filter(mia::P2DImage image) const
{
	save_image(m_name, image); 
	return image; 
}


C2DTeeFilterPluginFactory::C2DTeeFilterPluginFactory(): 
	C2DFilterPlugin("tee")
{
	add_parameter("file", new CStringParameter(m_filename, true,
						   "name of the output file to save the image too."));
}

mia::C2DFilter *C2DTeeFilterPluginFactory::do_create()const
{
	return new C2DTee(m_filename); 
}

const std::string C2DTeeFilterPluginFactory::do_get_descr()const
{
	return "Save the input image to a file and also pass it through to the next filter"; 
}

extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	return new C2DTeeFilterPluginFactory();
}
NS_END