/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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

#ifndef mia_2d_transformfactory_hh
#define mia_2d_transformfactory_hh

#include <set>
#include <mia/2d/transform.hh>
#include <mia/core/export_handler.hh>
#include <mia/internal/transformfactory.hh>
#include <mia/core/factory.hh>

NS_MIA_BEGIN

/** The base class for 2D transformation creators 
    \remark should the name be C2DTransformFactory? 
*/ 
typedef TTransformCreator<C2DTransformation>  C2DTransformCreator; 

/// Pointer type to the transformation factory
typedef std::shared_ptr<C2DTransformCreator > P2DTransformationFactory;

typedef TTransformCreatorPlugin<C2DTransformation>  C2DTransformCreatorPlugin; 

/// The plugin handler to manage the transformation creators
typedef THandlerSingleton<TFactoryPluginHandler<C2DTransformCreatorPlugin> > C2DTransformCreatorHandler;

struct EXPORT_2D C2DTransformCreatorHandlerTestPath {
	C2DTransformCreatorHandlerTestPath(); 
private: 
	CSplineKernelTestPath spktp; 
}; 

FACTORY_TRAIT(C2DTransformCreatorHandler); 

NS_MIA_END


#endif
