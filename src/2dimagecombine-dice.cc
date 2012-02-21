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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>

#include <mia/core/cmdlineparser.hh>
#include <mia/internal/main.hh>
#include <mia/core/msgstream.hh>
#include <mia/2d/2dfilter.hh>
#include <mia/2d/2dimageio.hh>

using namespace std;
NS_MIA_USE


const SProgramDescription g_general_help = {
	"Miscellaneous programs", 
	"This program evaluate the dice index of two binary masks given as binary images. "
	"The result is written to stdout.", 
	"Evaluate the dice index of maks1.png and mask2.png", 
	"-1 mask1.png -2 mask2.png", 
}; 

template <typename I1, typename I2, typename F>
void for_each_pair(I1 begin1, I1 end1, I2 begin2, F function) 
{
	while ( begin1 != end1 ) {
		function(*begin1, *begin2); 
		++begin1; 
		++begin2; 
	}
}

float get_dice_index(const C2DBitImage& mask1, const C2DBitImage& mask2)
{
	

	int n1 = 0; 
	int n2 = 0; 
	int s = 0; 

	for_each_pair(mask1.begin(), mask1.end(), mask2.begin(), 
		      [&n1, &n2, &s](bool b1, bool b2){
			      if (b1)  ++n1; 
			      if (b2)  ++n2; 
			      if (b1 && b2) ++s; 
		      }); 
	
	int sum = n1 + n2; 
	if (sum) 
		return (2.0 * s) / sum; 
	else 
		return 1.0; 
}

int do_main( int argc, char *argv[] )
{

	string in_filename1;
	string in_filename2;
	
	CCmdOptionList options(g_general_help);
	options.add(make_opt( in_filename1, "in-file-1", '1', "input image 1", CCmdOption::required)); 
	options.add(make_opt( in_filename2, "in-file-2", '2', "input image 1", CCmdOption::required)); 

	
	if (options.parse(argc, argv) != CCmdOptionList::hr_no)
		return EXIT_SUCCESS; 
	
	
	auto in_image1 = load_image2d(in_filename1);
	auto in_image2 = load_image2d(in_filename2);

	if (in_image1->get_pixel_type() != it_bit) 
		throw invalid_argument("Image 1 is not of bit pixel type"); 

	if (in_image2->get_pixel_type() != it_bit) 
		throw invalid_argument("Image 2 is not of bit pixel type"); 
	
	if (in_image1->get_size() != in_image2->get_size()) 
		throw invalid_argument("Images are of different size"); 

	
	auto mask1 = dynamic_cast<const C2DBitImage&>(*in_image1); 
	auto mask2 = dynamic_cast<const C2DBitImage&>(*in_image2); 
	
	cout << get_dice_index(mask1, mask2) <<"\n";
	return EXIT_SUCCESS; 
}

MIA_MAIN(do_main); 

