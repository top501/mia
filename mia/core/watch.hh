/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2010
 * Max-Planck-Institute for Human Cognitive and Brain Science
 * Max-Planck-Institute for Evolutionary Anthropology
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

#ifndef mia_core_watch_h
#define mia_core_watch_h


#include <mia/core/defines.hh>

NS_MIA_BEGIN
/** \file watch.hh
    A milli-second watch to measure running times.
*/

/** A class to count time */
class EXPORT_CORE CWatch {
	static int overlaps;
public:
	/** construct a watch
	    \todo Make this a singelton, since no second instance is needed
	 */
	CWatch();

	/// \returns  get the current time value
	double get_seconds();
private:
	static void overlap_handler(int p_sig);
};

NS_MIA_END

#endif



