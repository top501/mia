/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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

#include <iostream>
#include <fstream>
#include <mia/core/msgstream.hh>

NS_MIA_BEGIN

const TDictMap<vstream::Level>::Table verbose_dict[] = {
#ifndef NDEBUG
	{"trace", vstream::ml_trace},
	{"debug", vstream::ml_debug},
#endif
	{"info", vstream::ml_info},
	{"message", vstream::ml_message},
	{"warning", vstream::ml_warning},
	{"error", vstream::ml_error},
	{"fail", vstream::ml_fail},
	{"fatal", vstream::ml_fatal},
	{NULL, vstream::ml_undefined},
};

const TDictMap<vstream::Level> g_verbose_dict(verbose_dict);

vstream::vstream(std::ostream& output, Level l):
	m_output_level(l)
{
	m_output = &output; 
}

void vstream::set_verbosity(Level l)
{
	m_output_level = l;
}

vstream& vstream::instance()
{
	static vstream verb(m_output_target ? *m_output_target :
			    std::cerr, vstream::ml_fail);
	return verb;
}

std::ostream *vstream::m_output_target = 0;

void vstream::set_output_target(std::ostream* os)
{
	m_output_target = os;
}

vstream& vstream::operator << (Level l)
{
	m_message_level = l;
	if (m_message_level >= m_output_level) {

		switch (m_message_level) {
		case ml_debug:  *m_output << "DEBUG:"; break;
		case ml_info:  *m_output << "INFO:"; break;
		case ml_message:break;
		case ml_warning:*m_output << "WARNING:"; break;
		case ml_fail:   *m_output << "FAILED:"; break;
		case ml_error:  *m_output << "ERROR:"; break;
		case ml_fatal:  *m_output << "FATAL:"; break;
		default:        *m_output << "TRACE:";
		}
	}

	return *this;
}

std::ostream&  vstream::set_stream(std::ostream& os)
{
	std::ostream& old_os = *m_output;
	m_output = &os;
	return old_os;
}

vstream & vstream::operator<<(std::ostream& (*f)(std::ostream&))
{
	if (m_message_level >= m_output_level) {
		*m_output << f;
	}
	return *this;
}


void set_verbose(bool verbose)
{
	vstream::instance().set_verbosity(verbose ? vstream::ml_message : vstream::ml_error);
}

#ifndef NDEBUG
__thread size_t CTrace::m_depth = 0;
#endif

__thread std::ostream* vstream::m_output;
__thread vstream::Level vstream::m_message_level = vstream::ml_fatal; 

NS_MIA_END
