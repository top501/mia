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


#define VSTREAM_DOMAIN "3dtransformcreator"
#include <mia/core/export_handler.hh>

#include <mia/3d/transformfactory.hh>
#include <mia/template/transformfactory.cxx>

NS_MIA_BEGIN

C3DTransformCreatorHandlerTestPath::C3DTransformCreatorHandlerTestPath()
{
	CPathNameArray kernelsearchpath;
	kernelsearchpath.push_back(bfs::path(MIA_BUILD_ROOT"/mia/3d/transform"));
	C3DTransformCreatorHandler::set_search_path(kernelsearchpath);
}



template <> const char *  const 
TPluginHandler<C3DTransformCreatorPlugin>::m_help =  
   "These plug-ins define creators for 3D transformations.";

template class TTransformCreator<C3DTransformation>; 
template class TTransformCreatorPlugin<C3DTransformation>; 

EXPLICIT_INSTANCE_DERIVED_FACTORY_HANDLER(C3DTransformCreator, C3DTransformCreatorPlugin); 

NS_MIA_END
