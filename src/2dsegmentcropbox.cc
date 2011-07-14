/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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


/*

  LatexBeginProgramDescription{Myocardial Perfusion Analysis}
  
  \subsection{mia-2dsegmentcropbox}
  \label{mia-2dsegmentcropbox}

  \begin{description} 
  \item [Description:] 
        This program is used on a segmentation set and evaluates a bounding box 
          that encloses the segmentation in all slices. 
	This bounding box is then used to crop the original images, correct 
          the segmentation and store a new segmentation set with the cropped images. 
        The cropped images will be of the same type as the original images. 
	If no segmentation is given in the set, the result is undefined. 
 
  The program is called like 
  \begin{lstlisting}
mia-2dsegmentcropbox -i <original set> -o <cropped set> [options]
  \end{lstlisting}

  \item [Options:] $\:$

  \optiontable{
  \optinset
  \optoutset
  \cmdopt{override-imagepath}{p}{}{Instead of using the path of the image files as given in the 
                                   segmentation set, assume the files are located in the current directory}
  \cmdopt{cropped-base}{c}{string}{Base name of the cropped images, a number and the file type suffix will be added}
  \cmdopt{enlarge}{e}{int}{Enlarge the area around the obtained sbounding box by this number of 
                           pixels in each direction}
  }

  \item [Example:]Evaluate the optimal cropping for a segmentation set segment.set and enlarge it by 
                  3 pixels. Store the resulting set in the file cropped.set. 
  \begin{lstlisting}
mia-2dsegmentcropbox -i segment.set -o cropped.set -e 3
  \end{lstlisting}
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


struct 	CSegFrameCropper {
	CSegFrameCropper(const C2DIVector& shift,
			 P2DFilter filter,
			 const string& image_name);

	CSegFrame operator()(const CSegFrame& frame, const C2DImage& image) const;

private:
	C2DIVector m_shift;
	P2DFilter m_filter;
	bfs::path m_image_outpath;
};

const char *g_description = 
	"This program is used to crop the images of a segmented image series to contain "
	"the segmentation over the full image series. An boundary enlargement factor can be given."
	;

int do_main(int argc, const char *argv[])
{
	string src_filename;
	string out_filename;
	string image_name = "crop";
	bool override_src_imagepath = false;

	float  enlarge_boundary = 5;

	CCmdOptionList options(g_description);
	options.add(make_opt( src_filename, "in-file", 'i', "input segmentation set", CCmdOption::required));
	options.add(make_opt( override_src_imagepath, "override-imagepath", 'p',
				    "overide image path given in the segmentation set"));
	options.add(make_opt( out_filename, "out-file", 'o', "output segmentation set", CCmdOption::required));
	options.add(make_opt( image_name, "cropped-base", 'c', "Base name for the cropped image files"));
	options.add(make_opt( enlarge_boundary, "enlarge", 'e',
				    "enlarge boundary by number of pixels"));
	if (options.parse(argc, argv) != CCmdOptionList::hr_no)
		return EXIT_SUCCESS; 


	CSegSetWithImages  segset(src_filename, override_src_imagepath);
	C2DBoundingBox box = segset.get_boundingbox();
	box.enlarge(enlarge_boundary);

	stringstream crop_descr;
	crop_descr << "crop:"
		   << "start=[" << box.get_grid_begin()
		   << "],end=[" << box.get_grid_end() << "]";

	cvdebug() << "Crop with " << crop_descr.str() << "\r";

	auto filter = C2DFilterPluginHandler::instance().produce(crop_descr.str().c_str());

	CSegSet cropped;

	CSegSet::Frames::const_iterator iframe = segset.get_frames().begin();
	CSegSet::Frames::const_iterator eframe = segset.get_frames().end();
	C2DImageSeries::const_iterator iimages = segset.get_images().begin();

	CSegFrameCropper crop_frame(box.get_grid_begin(), filter, out_filename);

	size_t i = 0;
	while (iframe != eframe) {
		cvmsg() << "Frame " << i++ << "\n";

		cropped.add_frame(crop_frame(*iframe, **iimages));
		++iframe;
		++iimages;
	}

	// save output
	auto_ptr<xmlpp::Document> outset(cropped.write());

	ofstream outfile(out_filename.c_str(), ios_base::out );
	if (outfile.good())
		outfile << outset->write_to_string_formatted();

	return outfile.good() ? EXIT_SUCCESS : EXIT_FAILURE;
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



CSegFrameCropper::CSegFrameCropper(const C2DIVector& shift,
				   P2DFilter filter,
				   const string& image_name):
	m_shift(shift),
	m_filter(filter),
	m_image_outpath(image_name)
{
	m_image_outpath.remove_filename();

}


CSegFrame CSegFrameCropper::operator()(const CSegFrame& frame, const C2DImage& image) const
{
	P2DImage cropped = m_filter->filter(image);
	const string out_filename = (m_image_outpath.string() / bfs::path(frame.get_imagename())).string();

	if (!save_image(out_filename, cropped))
		cvwarn() << "Could not write cropped file '" << out_filename << "'\n";

	CSegFrame result = frame;
	result.shift(m_shift, frame.get_imagename());
	return result;

}
