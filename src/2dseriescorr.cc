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


/*

  LatexBeginProgramDescription{Myocardial Perfusion Analysis}
  
  \subsection{mia-2dseriescorr}
  \label{mia-2dseriescorr}

  \begin{description} 
  \item [Description:] 
           Given a set of images of temporal sucession, evaluates images that represent 
           the time-intensity correlation in horizontal and vertical direction as 
	   well as average correlation of each pixel with its neighbors. 
	   All input images must be of the same pixel type and size. 
  The program is called like 
  \begin{lstlisting}
mia-2dseriescorr -i <input images> -z <horizontal> \
      -t <vertical> -a <average> [options]
  \end{lstlisting}

  \item [Options:] $\:$

  \optiontable{
  \cmdopt{in-base}{i}{string}{Name of one of the input image of the series. 
      All images need to be numbered consecutivly like imageXXXX.ext with the digits XXXX.}
  \cmdopt{horizontal}{z}{string}{output file name for horizontal correlation}
  \cmdopt{vertical}{t}{string}{output file name for vertical correlation}
  \cmdopt{skip}{k}{int}{Skip a number of frames at the beginning of the series}
  \cmdopt{end}{e}{int}{Last image index}
  }

  \item [Example:]Evaluate the time-intensity correaltions for an image series 
                imageXXXX.png starting at image 2 and stop at image 30. 
		Store the results in horizontal.exr, and vertical.exr. 
  \begin{lstlisting}
mia-2dseriescorr -i image0000.png -k 2 -e 30 -z horizontal.exr \
                             -t vertical.exr
  \end{lstlisting}
  \item [Remark:] The correlations are given as floating point values and thereby 
                  requires an output format that supports this pixel type. 
  \end{description}
  
  LatexEnd
*/


#define VSTREAM_DOMAIN "2dseriescorr"
#include <iomanip>
#include <ostream>
#include <fstream>
#include <map>
#include <mia/core.hh>
#include <queue>

#include <mia/internal/main.hh>
#include <mia/2d/2dimageio.hh>
#include <mia/2d/2dfilter.hh>
#include <mia/2d/ica.hh>


NS_MIA_USE;


const SProgramDescrption g_description = {
	"Myocardial Perfusion Analysis", 

	"Given a set of images of temporal sucession, evaluates images that represent "
	"the time-intensity correlation in horizontal and vertical direction as "
	"well as average correlation of each pixel with its neighbors. "
	"All input images must be of the same pixel type and size.", 

	"Evaluate the time-intensity correaltions for an image series "
	"imageXXXX.png starting at image 2 and stop at image 30. "
	"Store the results in horizontal.exr, and vertical.exr.", 
	
	"-i image0000.png -k 2 -e 30 -z horizontal.exr -t vertical.exr"
}; 


struct FCorrelationAccumulator : public TFilter<bool> {

	FCorrelationAccumulator(const C2DBounds & size);

	template <typename T>
	bool operator ()(const T2DImage<T>& image);

	P2DImage get_horizontal_corr() const;
	P2DImage get_vertical_corr() const;
private:
	C2DDImage sx2;

	C2DDImage sxy_horizontal;
	C2DDImage sxy_vertical;

	C2DDImage sx;
	C2DDImage sy;
	C2DBounds size;
	size_t len;
};

int do_main( int argc, char *argv[] )
{
	string src_name("data0000.exr");
	string out_hor_name("horizontal.exr");
	string out_ver_name("vertical.exr");
	string out_type("exr");
	size_t first =  2;
	size_t last  = 60;



	CCmdOptionList options(g_description);
	options.add(make_opt( src_name, "in-base", 'i', "input file name base"));
	options.add(make_opt( out_hor_name, "horizontal", 'z', "horiZontal correlation output file name"));
	options.add(make_opt( out_ver_name, "vertical", 't', "verTical  correlation output file name"));
	options.add(make_opt( first, "skip", 'k', "skip images at beginning of series"));
	options.add(make_opt( last, "end", 'e', "last image in series"));

	if (options.parse(argc, argv) != CCmdOptionList::hr_no)
		return EXIT_SUCCESS; 


	size_t start_filenum = 0;
	size_t end_filenum  = 0;
	size_t format_width = 0;

	string src_basename = get_filename_pattern_and_range(src_name, start_filenum, end_filenum, format_width);

	if (start_filenum < first)
		start_filenum = first;
	if (end_filenum > last)
		end_filenum = last;

	// load images
	vector<P2DImage> series;
	for (size_t i = start_filenum; i < end_filenum; ++i) {
		string src_name = create_filename(src_basename.c_str(), i);
		P2DImage image = load_image<P2DImage>(src_name);
		series.push_back(image );
	}

	// evaluate all series correlation coefficients
	cvmsg()<< "Got series of " << series.size() << " images\n";
	FCorrelationAccumulator acc(series[0]->get_size());
	for (auto i = series.begin(); i != series.end(); ++i)
		::mia::accumulate(acc, **i);

	P2DImage hor = acc.get_horizontal_corr();
	P2DImage ver = acc.get_vertical_corr();

	if (!save_image(out_hor_name, hor))
		THROW(runtime_error, "unable to save horizontal correlation to '"<<out_hor_name<<"'");

	if (!save_image(out_ver_name, ver))
		THROW(runtime_error, "unable to save vertical correlation to '"<<out_ver_name<<"'");


	return EXIT_SUCCESS;

};



FCorrelationAccumulator::FCorrelationAccumulator(const C2DBounds & _size):
	sx2(_size),
	sxy_horizontal(_size),
	sxy_vertical(_size),
	sx(_size),
	size(_size),
	len(0)
{
}

template <typename T>
bool FCorrelationAccumulator::operator ()(const T2DImage<T>& image)
{
	if (image.get_size() != size)
		THROW(invalid_argument, "Input image size " << size << " expected, but got " <<
		      image.get_size());
	// sum x
	transform(image.begin(), image.end(), sx.begin(), sx.begin(), 
		  [](T x, double y){return x + y;}); 

	// sum x^2
	transform(image.begin(), image.end(), sx2.begin(), sx2.begin(), 
		  [](double x, double y){return x*x + y;}); 

	// sum horizontal
	for (size_t y = 0; y < size.y; ++y) {
		auto irow = image.begin_at(0,y);
		auto orow = sxy_horizontal.begin_at(0,y);
		for (size_t x = 0; x < size.x-1; ++x, ++irow, ++orow) {
			*orow += irow[0] * irow[1];
		}
	}

	// sum vertical
	for (size_t y = 1; y < size.y; ++y) {
		auto irow0 = image.begin_at(0,y-1);
		auto irow1 = image.begin_at(0,y);
		auto orow = sxy_vertical.begin_at(0,y-1);
		for (size_t x = 0; x < size.x; ++x, ++irow0, ++irow1,++orow) {
			*orow += *irow0 * *irow1;
		}
	}
	++len;
	return true;
}

P2DImage FCorrelationAccumulator::get_horizontal_corr() const
{
	if (!len)
		THROW(invalid_argument, "No input images");


	C2DFImage *result = new C2DFImage(C2DBounds(size.x-1, size.y));
	P2DImage presult(result);
	for (size_t y = 0; y < size.y; ++y) {
		auto irow_xy = sxy_horizontal.begin_at(0,y);
		auto irow_xx = sx2.begin_at(0,y);
		auto irow_yy = sx2.begin_at(1,y);
		auto irow_x  = sx.begin_at(0,y);
		auto irow_y  = sx.begin_at(1,y);
		auto orow    = result->begin_at(0,y);

		for (size_t x = 1; x < size.x;
		     ++x, ++irow_xy, ++irow_xx, ++irow_yy, ++irow_x, ++irow_y, ++orow) {

			const float ssxy = *irow_xy - *irow_x * *irow_y / len;
			const float ssxx = *irow_xx - *irow_x * *irow_x / len;
			const float ssyy = *irow_yy - *irow_y * *irow_y / len;

			if (fabs(ssxx) < 1e-10 && fabs(ssyy) < 1e-10)
				*orow =  1.0;
			else if (fabs(ssxx) < 1e-10 || fabs(ssyy) < 1e-10)
				*orow =  0.0;
			else
				*orow = (ssxy * ssxy) /  (ssxx * ssyy);

		}
		++irow_xy; ++irow_xx; ++irow_yy; ++irow_x; ++irow_y;
	}
	return presult;
}

P2DImage FCorrelationAccumulator::get_vertical_corr() const
{
	if (!len)
		THROW(invalid_argument, "No input images");

	C2DFImage *result= new C2DFImage(C2DBounds(size.x, size.y-1));
	P2DImage presult(result);

	for (size_t y = 0; y < size.y-1; ++y) {
		auto irow_xy = sxy_vertical.begin_at(0,y);
		auto irow_xx = sx2.begin_at(0,y);
		auto irow_yy = sx2.begin_at(0,y+1);
		auto irow_x  = sx.begin_at(0,y);
		auto irow_y  = sx.begin_at(0,y+1);
		auto orow    = result->begin_at(0,y);

		for (size_t x = 0; x < size.x;
		     ++x, ++irow_xy, ++irow_xx, ++irow_yy, ++irow_x, ++irow_y, ++orow) {

			const float ssxy = *irow_xy - *irow_x * *irow_y / len;
			const float ssxx = *irow_xx - *irow_x * *irow_x / len;
			const float ssyy = *irow_yy - *irow_y * *irow_y / len;

			if (ssxx == 0 && ssyy == 0)
				*orow =  1.0;
			else if (ssxx == 0 || ssyy == 0)
				*orow = 0.0;
			else
				*orow = (ssxy * ssxy) /  (ssxx * ssyy);
		}
	}
	return presult;
}

MIA_MAIN(do_main); 
