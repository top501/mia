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

#ifndef mia_internal_transformfactory_hh
#define mia_internal_transformfactory_hh

#include <set>
#include <mia/2d/transform.hh>
#include <mia/core/factory.hh>
#include <mia/core/import_handler.hh>

NS_MIA_BEGIN

/**
   This class is the base class for a Creater of transformations according to a 
   given model. 
   Derived from this class are all the plug-ins that may create transformations 
   of different types. 
 */
template <typename Transform> 
class EXPORT_HANDLER TTransformCreator: public CProductBase {
public:
	typedef typename Transform::Data plugin_data; 
	typedef Transform plugin_type; 
	typedef std::shared_ptr<TTransformCreator<Transform> > Pointer; 

	/** Standard constructor 
	    \remark it is empty, so why it is defined? 
	 */
	TTransformCreator();

	/**
	   Creates a transformation according to the given model and defined 
	   on a grid [(0,0), size}
	 */
	typename Transform::Pointer create(const typename Transform::Size& size) const;
	
	/**
	   This function checks for a given property of the transformation creator. 
	   \param property 
	   \returns \a true if property is supported
	 */
	bool has_property(const char *property) const;
protected:
	/**
	   Add a property 
	   \param property 
	 */
	void add_property(const char *property);
private:
	virtual	typename Transform::Pointer do_create(const typename Transform::Size& size) const = 0;

	std::set<std::string> m_properties;
};

NS_MIA_END

#endif
