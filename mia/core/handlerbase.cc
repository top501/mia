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

#include <mia/core/handlerbase.hh>

NS_MIA_BEGIN

CPluginHandlerBase::CPluginHandlerBase(const std::string& descriptor):
	m_descriptor(descriptor)
{
}

CPluginHandlerBase::~CPluginHandlerBase() 
{
	
}

void CPluginHandlerBase::print_short_help(std::ostream& os) const
{
	do_print_short_help(os); 
}

void CPluginHandlerBase::print_help(std::ostream& os) const
{
	do_print_help(os); 
}

void CPluginHandlerBase::get_xml_help(xmlpp::Element *root) const
{
	do_get_xml_help(root); 
}

const std::string& CPluginHandlerBase::get_descriptor() const
{
	return m_descriptor; 
}

void CPluginHandlerBase::add_dependend_handlers(HandlerHelpMap& handler_map) const
{
	if (handler_map.find(m_descriptor) != handler_map.end()) 
		return; 
	handler_map[m_descriptor] = this; 	
	do_add_dependend_handlers(handler_map);
}

NS_MIA_END
