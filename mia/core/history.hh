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

// $Id: miaHistory.hh 937 2006-07-11 11:56:52Z write1 $

#ifndef HISTORY_HH
#define HISTORY_HH

#include <list>
#include <string>
#include <map>
#include <vector>

#include <mia/core/cmdlineparser.hh>

NS_MIA_BEGIN


/** This class holds the complete history list of a data set */


class EXPORT_CORE CHistory: public std::list<CHistoryEntry> {
public:
	/** append a new history record
	    \param program name of the calling program
	    \param version version of the program run
	*/
	void append(const std::string& program, const std::string& version, const CCmdOptionList& options);

	/**
	    \returns the complete history as a string.
	*/
	std::string as_string()const;

	/** returns the reference to the singelton history object */
	static CHistory& instance();
private:
	CHistory();
};
NS_MIA_END

#endif
