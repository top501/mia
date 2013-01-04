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


#include <mia/3d/creator.hh>
#include <mia/core/handler.cxx>
#include <mia/core/plugin_base.cxx>


NS_MIA_BEGIN


template <> const char *  const 
TPluginHandler<TFactory<TImageCreator<C3DImage>>>::m_help =  
   "These plug-ins implement creators for 3D test images.";

INSTANCIATE_Image_Creator(C3DImage);

NS_MIA_END

