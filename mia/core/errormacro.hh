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


#ifndef mia_core_errormacro_hh
#define mia_core_errormacro_hh


#include <sstream>
#include <cassert>
#include <stdexcept>

/** 
    \ingroup helpers
    \brief  helper macro to create exceptions using stream output for its message 
 */
#define THROW(EXCEPTION, MESSAGE)		\
	do {					\
		std::stringstream msg;		\
		msg << MESSAGE;			\
		throw EXCEPTION(msg.str());	\
	} while (0)

#ifdef NDEBUG

// throw if compiled in release mode but assert if compiled in debug mode
#define DEBUG_ASSERT_RELEASE_THROW(cond, msg) \
	if (!cond)		   \
		throw std::logic_error(msg);
#else
#define DEBUG_ASSERT_RELEASE_THROW(cond, msg) \
	assert(cond && msg); 
#endif 

#endif
