/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2011
 *
 * BIT, ETSI Telecomunicacion, UPM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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
 *
 */

#ifndef mia_2d_combiner_plugin_hh
#define mia_2d_combiner_plugin_hh

NS_MIA_BEGIN

template <typename Combiner, const char * const name> 
class T2DImageCombinerPlugin: public C2DImageCombinerPlugin {
public:
	T2DImageCombinerPlugin(); 
private:
	virtual mia::C2DImageCombinerPlugin::ProductPtr do_create()const;
	virtual const std::string do_get_descr() const;
}; 


template <typename Combiner, const char * const name> 
T2DImageCombinerPlugin<Combiner, name>::T2DImageCombinerPlugin():
	C2DImageCombinerPlugin(name) 
{
}
	
template <typename Combiner, const char * const name> 
C2DImageCombinerPlugin::ProductPtr 
T2DImageCombinerPlugin<Combiner, name>::do_create()const
{
	return C2DImageCombinerPlugin::ProductPtr (new Combiner); 
}

template <typename Combiner, const char name[]> 
const std::string T2DImageCombinerPlugin<Combiner, name>::do_get_descr() const
{
	return string("Image combiner '") +  string(name) + string("'"); 
}


NS_MIA_END

#endif