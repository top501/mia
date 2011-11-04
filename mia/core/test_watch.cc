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

#include <miaconfig.h>
#include <ctime>
#include <mia/internal/autotest.hh>
#include <mia/core/watch.hh>

#ifndef WIN32	
#include <sys/time.h>
#endif 

void don_t_optimize_x_away(double /*x*/) 
{
}

BOOST_AUTO_TEST_CASE( test_watch ) 
{
	CWatch watch; 
#ifndef WIN32	
	timeval tv_start, tv_end; 
	double start = watch.get_seconds();
	gettimeofday(&tv_start, NULL); 
	
	double x = 0.1; 
	for (long int i = 0; i < 100000000L; ++i) 
		x += 0.1; 
	// keep this line, or the compiler will remove the loop above
	don_t_optimize_x_away(x); 

	
	double end = watch.get_seconds(); 
	gettimeofday(&tv_end, NULL); 
	double test_time = (tv_end.tv_sec - tv_start.tv_sec) + (tv_end.tv_usec - tv_start.tv_usec) / 1e+6; 

	BOOST_CHECK_CLOSE(end - start, test_time, 1); 
	
#else 
	      cvwarn() << "Watch is a stub on WIN32\n"; 
#endif 
}
