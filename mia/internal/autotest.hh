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

#ifndef mia_internal_autotest_hh
#define mia_internal_autotest_hh

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <climits>

#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
//#define BOOST_TEST_ALTERNATIVE_INIT_API
#define BOOST_TEST_MAIN
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <miaconfig.h>
#include <mia/core/msgstream.hh>
#include <mia/core/cmdlineparser.hh>

const mia::SProgramDescription description = {
	"Tests", 
	"Unit test.", 
	"This program runs a set of tests.", 
	"Example text", 
	"Example"
};



int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
	// set the plug-in path for tests 
	setenv("MIA_PLUGIN_TESTPATH", MIA_BUILD_ROOT "/plugintest/" PLUGIN_INSTALL_PATH ,1);
#ifdef WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif
	if (mia::CCmdOptionList(description).parse(argc, argv, "boost-test-option") != mia::CCmdOptionList::hr_no) 
		return 0; 
	mia::cvdebug() << "Initialize test ...\n"; 
	return ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}

#endif 
