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
  
  \subsection{mia-2dseries-segdistance}
  \label{mia-2dseries-segdistance}

  \begin{description} 
  \item [Description:] 
  Get the mean distance of a segmentation boundary to the reference boundary. 

  The program is called like 
  \begin{lstlisting}
mia-2dseries-segdistance -i <segmentation set> -r <reference frame> 
  \end{lstlisting}

  \item [Options:] $\:$

  \optiontable{
  \optinset
  \cmdopt{ref-frame}{r}{int}{refernce frame to compare the segmentation to}
  }

  \item [Example:]Evaluate the mean absolute border distanceof the segmentations of set segment.set with 
                  respect to the segmentation given in frame 20. 
  \begin{lstlisting}
mia-2dseries-segdistance -i segment.set -r 20 
  \end{lstlisting}
  \item [See also:] \sa{mia-2dsegcompare}
  \end{description}
  
  LatexEnd
*/


#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ostream>
#include <cmath>
#include <list>
#include <cassert>
#include <boost/filesystem.hpp>
#include <libxml++/libxml++.h>

#include <mia/core.hh>
#include <mia/2d/SegSetWithImages.hh>
#include <mia/2d/2dimageio.hh>
#include <mia/2d/2dfilter.hh>

using namespace std;
using namespace mia;
using xmlpp::DomParser;
namespace bfs=boost::filesystem;

const char *g_description = 
	"Get mean average distance of the myocardium border to the border of a given reference frame."; 


double mean_frame_border_distance(const C2DDImage& distance, const C2DBitImage& mask) 
{
	double result = 0.0; 
	int n = 0; 
	auto im = mask.begin(); 
	auto em = mask.end(); 
	auto id = distance.begin(); 

	while (im != em) {
		if (*im) {
			result += *id; 
			++n; 
		}
		++id; 
		++im; 
	}
	return n ? result / n : 0.0; 
}

C2DBitImage get_boundary(const C2DUBImage& mask) 
{
	auto orig_mask = run_filter(mask, "binarize:min=1"); 
	auto enlarged_mask = run_filter(*orig_mask, "dilate:shape=8n");
	
	const C2DBitImage& om = dynamic_cast<const C2DBitImage&>(*orig_mask); 
	const C2DBitImage& em = dynamic_cast<const C2DBitImage&>(*enlarged_mask); 
	

	C2DBitImage result(om.get_size());
	transform(em.begin(), em.end(), om.begin(), result.begin(), 
		  [](bool enlarged, bool orig) {
			  return enlarged && !orig;
		  }); 
	return result; 
}

P2DImage get_distance_transform(const C2DImage& image) 
{
	return run_filter(image, "distance"); 
}

int do_main(int argc, const char *argv[])
{
	string src_filename;
	size_t reference = 20;
	int  skip = 0; 

	CCmdOptionList options(g_description);
	options.add(make_opt( src_filename, "in-file", 'i', "input segmentation set", CCmdOption::required));
	options.add(make_opt( reference, "reference", 'r', "reference frame", CCmdOption::required));
	options.add(make_opt( skip, "skip", 'k', "skip images at the beginning"));
	if (options.parse(argc, argv) != CCmdOptionList::hr_no)
		return EXIT_SUCCESS; 

	CSegSetWithImages srcset(src_filename, true);

	if (skip < 0) {
                // if RV peak is given in the segmentation file, use it, otherwiese use 
		// absolue value of skip 
		int sk = srcset.get_RV_peak(); 
		skip = (sk < 0 ) ? -skip : sk; 
	}

	const auto& src_frames = srcset.get_frames();

	if (reference  >=  src_frames.size())
		throw invalid_argument("The requested reference frame is larger then the size of the set."); 
	
	if (skip >= src_frames.size()) 
		THROW(invalid_argument, "Try to skip " << skip << " frames, but series has only " << src_frames.size()); 

	auto isrc_frame = src_frames.begin() + skip;
	auto esrc_frame = src_frames.end();

	auto reference_boundary = get_boundary(src_frames[reference].get_section_masks(1)); 
	auto dtf = get_distance_transform(reference_boundary); 
		
	const C2DDImage& fdtf = dynamic_cast<const C2DDImage&>(*dtf); 
	
	vector<double> mean_frame_dist(src_frames.size() - skip); 
	auto im = mean_frame_dist.begin(); 
	
	while (isrc_frame != esrc_frame) {
		auto boundary = get_boundary(isrc_frame->get_section_masks(1));
		*im = mean_frame_border_distance(fdtf, boundary); 
		cvmsg() << *im << "\n"; 
		++isrc_frame; 
		++im; 
	}
	double mean = accumulate(mean_frame_dist.begin(), mean_frame_dist.end(), 0.0) / mean_frame_dist.size(); 
	cout << mean << "\n"; 
	return 0;
}

int main(int argc, const char *argv[] )
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

