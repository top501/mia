/* -*- mona-c++  -*-
 *
 * Copyright (c) Madrid 2010 BIT, ETSI Telecomunicacion, UPM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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


#include <mia/core/problem.hh>

NS_MIA_BEGIN

void CProblem::setup()
{
	do_setup(); 
}

void CProblem::finalize()
{
	do_finalize(); 
}

bool CProblem::has_all_properties_in(const CPropertyFlagHolder& holder) const
{
	return do_has_all_properties_in(holder); 
}

const char *CProblem::get_name() const
{
	return do_get_name(); 
}

double CProblem::get_criterion(std::vector<float>& gradient) const 
{
	return do_get_criterion(gradient); 
}

void CProblem::update_params(const std::vector<float>& delta)
{
	do_update_params(delta); 
}

NS_MIA_END
