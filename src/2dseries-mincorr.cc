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


/*
  LatexBeginProgramDescription{Myocardial Perfusion Analysis}

  \subsection{mia-2dseries-mincorr}
  \label{mia-2dseries-mincorr}
  
  \begin{description} 
  \item [Description:] 
           Given a set of images of temporal sucession, this program evaluates the 
	   minimal correlation of the time-intensity curve between neighboring pixels. 

  The program is called like 
  \
  \begin{lstlisting}
mia-2dseriessmoothgradMAD -i <input set> -o <output image> [options] 
  \end{lstlisting}

  \item [Options:] $\:$

  \optiontable{
  \optinfile
  \optoutfile
  \cmdopt{skip}{k}{int}{Skip a number of frames at the beginning of the series}
  }
  \item [Example:]Evaluate the minimal correlation image of a series givemn in  segment.set and 
		  store the image in OpenEXR format.  
		  Skip two images at the beginning of the series. 
   \
  \begin{lstlisting}
mia-2dseriessmoothgradMAD -i segment.set -o mincorr.exr -k 2 
  \end{lstlisting}
  \item [Remark:] The minimal correlation image has float-valued pixels and thereby requires an output 
                  format that supports this pixel type. 
  \end{description}
  
  LatexEnd
*/


#define VSTREAM_DOMAIN "2dmyocard"
#include <iomanip>
#include <ostream>
#include <fstream>
#include <map>
#include <mia/core.hh>
#include <queue>

#include <mia/internal/main.hh>
#include <mia/2d/2dimageio.hh>
#include <mia/2d/correlation_weight.hh>
#include <mia/2d/SegSetWithImages.hh>

NS_MIA_USE;

const SProgramDescrption g_description = {
	"Myocardial Perfusion Analysis", 
	
	"Given a set of images of temporal sucession, this program evaluates the "
	"minimal correlation of the time-intensity curve between neighboring pixels.", 
	
	"Evaluate the minimal correlation image of a series givemn in  segment.set and "
	"store the image in OpenEXR format. Skip two images at the beginning of the series.", 

	"-i segment.set -o mincorr.exr -k 2"
}; 


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

int do_main( int argc, char *argv[] )
{
	string src_name("segment.set");
	string out_name("min-correlation.v");
	size_t skip = 2; 


	CCmdOptionList options(g_description);
	options.add(make_opt( src_name, "in", 'i', "input segmentation set"));
	options.add(make_opt( out_name, "out", 'o', "output image of minimal correlation"));
	options.add(make_opt( skip, "skip", 'k', "skip images at beginning of series"));

	if (options.parse(argc, argv) != CCmdOptionList::hr_no)
		return EXIT_SUCCESS; 


	CSegSetWithImages  input_set(src_name, true);
	CCorrelationEvaluator ce(0.0);

	auto horver = ce(input_set.get_images(), skip);
	
	P2DImage result = get_minimal_correlation(horver); 
	
	return save_image(out_name, result)? EXIT_SUCCESS : EXIT_FAILURE;

};

MIA_MAIN(do_main); 
