/*
** Copyright Madrid (c) 2010 BIT ETSIT UPM
**                    Gert Wollny <gw.fossdev @ gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


/* -*- mona-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
 *
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

#define VSTREAM_DOMAIN "2dmyocard"
#include <iomanip>
#include <ostream>
#include <fstream>
#include <map>
#include <boost/lambda/lambda.hpp>
#include <mia/core.hh>
#include <queue>

#include <mia/2d/2dimageio.hh>
#include <mia/2d/correlation_weight.hh>
#include <mia/2d/SegSetWithImages.hh>

NS_MIA_USE;

using boost::lambda::_1;
using boost::lambda::_2;

const char *g_description = 
	"This program is used to evaluate the temporal intensity correlation of neighboring pixels "
	"in a series of images."
	; 


P2DImage get_minimal_correlation(const CCorrelationEvaluator::result_type& correlation)
{
	C2DFImage *result = new C2DFImage(C2DBounds(correlation.vertical.get_size().x, 
						    correlation.horizontal.get_size().y)); 
	P2DImage presult(result); 

	copy(correlation.vertical.begin(), correlation.vertical.end(), result->begin()); 

	auto r = result->begin_at(0,1);  
	auto vb =  correlation.vertical.begin(); 
	auto ve =  correlation.vertical.end(); 
	
	while (vb != ve) {
		if (*r > *vb) 
			*r = *vb; 
		++r; 
		++vb; 
	}

	for (size_t y = 0; y < correlation.horizontal.get_size().y; ++y) {
		auto rl = result->begin_at(0,y);  
		auto h = correlation.horizontal.begin_at(0,y); 
		for (size_t x = 0; x < correlation.horizontal.get_size().x; ++x, ++rl, ++h){
			if (*rl > *h) 
				*rl = *h; 
		}
		rl = result->begin_at(1,y);  
		h = correlation.horizontal.begin_at(0,y); 
		for (size_t x = 0; x < correlation.horizontal.get_size().x; ++x, ++rl, ++h){
			if (*rl > *h) 
				*rl = *h; 
		     }
	}
	return presult; 
}

int do_main( int argc, const char *argv[] )
{
	string src_name("segment.set");
	string out_name("min-correlation.v");
	size_t skip = 2; 


	CCmdOptionList options(g_description);
	options.push_back(make_opt( src_name, "in", 'i', "input segmentation set"));
	options.push_back(make_opt( out_name, "out", 'o', "output image of minimal correlation"));
	options.push_back(make_opt( skip, "skip", 's', "skip images at beginning of series"));

	options.parse(argc, argv);

	CSegSetWithImages  input_set(src_name, true);
	CCorrelationEvaluator ce(0.0);

	auto horver = ce(input_set.get_images(), skip);
	
	P2DImage result = get_minimal_correlation(horver); 
	
	return save_image(out_name, result)? EXIT_SUCCESS : EXIT_FAILURE;

};

int main( int argc, const char *argv[] )
{


	try {
		return do_main(argc, argv);
	}
	catch (const runtime_error &e){
		cerr << argv[0] << " runtime: " << e.what() << endl;
	}
	catch (const invalid_argument &e){
		cerr << argv[0] << " error: " << e.what() << endl;
	}
	catch (const exception& e){
		cerr << argv[0] << " error: " << e.what() << endl;
	}
	catch (...){
		cerr << argv[0] << " unknown exception" << endl;
	}

	return EXIT_FAILURE;
}

