/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2010
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


#ifndef mia_core_property_flags_hh
#define mia_core_property_flags_hh

#include <set>
#include <mia/core/defines.hh>

NS_MIA_BEGIN

extern EXPORT_CORE const char *property_gradient;


/**
   This class holds a set of properties. 
   \remark These properties are stored as plain text 
   strings, but in the end only the pointer is compared. 
*/
class  EXPORT_CORE CPropertyFlagHolder {
public:
	typedef std::set<const char *> Set;

	/**
	   Ensure evirtual destructor 
	 */
	virtual ~CPropertyFlagHolder(); 
	/** check if a certain property is available 
	    \param property
	    \returns true if the property is available
	*/
	bool has(const char *property) const;

	/** add  a certain property
	    \param property proprety to add 
	*/
	void add(const char *property);

	/**
	   Tests if all properties in the testset are available in this instance 
	   \param testset
	 */
	bool has_all_in(const CPropertyFlagHolder& testset)const;

	Set get_missing_properties(const CPropertyFlagHolder& testset)const; 
private:
	virtual bool do_has(const char *property) const;
	Set _M_properties;
};

NS_MIA_END
#endif
