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


#define VSTREAM_DOMAIN "3dtransformio"
#include <mia/core/export_handler.hh>

#include <mia/core/errormacro.hh>
#include <mia/3d/transformio.hh>
#include <mia/core/ioplugin.cxx>
#include <mia/core/iohandler.cxx>

NS_MIA_BEGIN

template class TIOPlugin<C3DTransformation>;
template class THandlerSingleton<TIOPluginHandler<C3DTransformationIO> >;
template class TIOHandlerSingleton<TIOPluginHandler<C3DTransformationIO> >;
template class TIOPluginHandler<C3DTransformationIO>;
template class TPluginHandler<C3DTransformationIO>;

NS_MIA_END


