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


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#ifndef WIN32
#include <sys/time.h>
#endif

#include <mia/core/watch.hh>
#include <mia/core/msgstream.hh>

NS_MIA_BEGIN

const int TIMERSPAN=2000;

#ifndef WIN32
CWatch::CWatch()
{
	itimerval value;
	itimerval oldvalue;

	value.it_interval.tv_sec = TIMERSPAN;
	value.it_interval.tv_usec= 0;

	value.it_value.tv_sec  = TIMERSPAN;
	value.it_value.tv_usec = 0;

	if (signal(SIGVTALRM,CWatch::overlap_handler)==SIG_ERR)
		cvwarn() << "Unable to catch  signal:" << strerror(errno) << "\n"; 
	
	if (setitimer(ITIMER_VIRTUAL,&value,&oldvalue))
		cvwarn() << "setitimer failed:" << strerror(errno) << "\n"; 
}

double CWatch::get_seconds() const
{
	itimerval value;

	if (getitimer(ITIMER_VIRTUAL,&value))
		cvwarn() << "setitimer failed:" << strerror(errno) << "\n"; 

	double result = TIMERSPAN - value.it_value.tv_sec;
	double resultlow = value.it_value.tv_usec/1e+6;
	return (result - resultlow) + TIMERSPAN*double(overlaps);
}

void CWatch::overlap_handler(int p_sig)
{
	if (p_sig == SIGVTALRM) {
		overlaps++;
	}
	signal(SIGVTALRM,CWatch::overlap_handler);
}

#else
CWatch::CWatch()
{
	cvwarn() << "CWatch::CWatch: fake implementation on WIN32\n";
}
double CWatch::get_seconds() const
{
	return 0.0;
}
void CWatch::overlap_handler(int p_sig)
{
}
#endif
int CWatch::overlaps=0;


NS_MIA_END
