/* -*- mia-c++  -*-
 *
 * This file is part of MIA - a toolbox for medical image analysis 
 * Copyright (c) Leipzig, Madrid 1999-2014 Gert Wollny
 *
 * MIA is free software; you can redistribute it and/or modify
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
 * along with MIA; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <mia/core/export_handler.hh>

#include <mia/core/errormacro.hh>
#include <mia/2d/transformio.hh>
#include <mia/2d/defines2d.hh>
#include <mia/core/ioplugin.cxx>
#include <mia/core/iohandler.cxx>

NS_MIA_BEGIN

C2DTransformIOPluginHandlerImpl::C2DTransformIOPluginHandlerImpl()
{
	TTranslator<C2DFVector>::register_for(C2DTransformation::input_spacing_attr);
	TTranslator<C2DFVector>::register_for(C2DTransformation::output_spacing_attr);
}


template <> const char *  const 
	TPluginHandler<C2DTransformationIO>::m_help =  
       "These plug-ins implement the support for loading and storing 2D transformations to various file types.";

template class EXPORT_2D TIOPlugin<C2DTransformation>;
template class EXPORT_2D THandlerSingleton<C2DTransformIOPluginHandlerImpl>;
template class EXPORT_2D TIOPluginHandler<C2DTransformationIO>;
template class EXPORT_2D TPluginHandler<C2DTransformationIO>;

NS_MIA_END


