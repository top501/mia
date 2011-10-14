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
  LatexBeginProgramDescription{2D image processing}

  \subsection{mia-2davgmasked}
  \label{mia-2davgmasked}
  
  \begin{description}
  \item [Description:] This program avaluates the average and variation of intensity values 
                       over an image series within labeled regions and writes the result 
		       to stdout. 

  The program is called like 
  \
  \begin{lstlisting}
mia-2davgmasked -i <input image> -m <mask>
  \end{lstlisting}

  \item [Options:] $\:$

  \optiontable{
  \optinfile
  \cmdopt{mask}{m}{string}{The masks defining image - must be of pixel type unsigned byte. 
                           Each distinct value of the mask image comprises an image region 
			   over which the statistics will be evaluated.}
  }

  \item [Example:]Evaluate the mean and the variance of images inputXXXX.png using the mask mask.png. 
   \
  \begin{lstlisting}
mia-2davgmasked -i input0000.png -m mask.png 
  \end{lstlisting}
  \end{description}
  
  LatexEnd
*/


#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>

#include <mia/core.hh>
#include <mia/2d/2dimageio.hh>



NS_MIA_USE
using namespace std;
using namespace boost;

const SProgramDescrption g_description = {
	"2D image processing", 
	"This program is used to evaluate the average intensity and its variation of a series "
	"of images in a given masked region.", 
	NULL, 
	NULL
}; 

struct C2DStat : public TFilter<bool> {

	typedef pair<size_t, double> TCollector;
	typedef map<size_t, TCollector> TSliceStat;

	C2DStat(const C2DUBImage& mask):
		m_mask(mask)
	{
	}

	template <typename T>
	bool operator ()(const T2DImage<T>& image)
	{
		if (image.get_size() != m_mask.get_size())
			throw invalid_argument("Input image and mask differ in size");

		typename T2DImage<T>::const_iterator i = image.begin();
		typename T2DImage<T>::const_iterator e = image.end();

		C2DUBImage::const_iterator m = m_mask.begin();

		TSliceStat slice;

		while (i != e) {
			if (*m) {
				size_t idx = *m - 1;
				++slice[idx].first;
				slice[idx].second += *i;
			}
			++i;
			++m;
		}

		m_counter.push_back(slice);
		cvmsg() << "Got " << m_counter.size() << "slices\n";
		return true;
	}

	const vector<TSliceStat>&  result() const {
		return m_counter;
	}

private:
	C2DUBImage m_mask;
	vector<TSliceStat>  m_counter;
};

/* Revision string */
const char revision[] = "not specified";

int do_main( int argc, char *argv[] )
{

	string in_filename;
	string mask_filename;

	const C2DImageIOPluginHandler::Instance& image2dio = C2DImageIOPluginHandler::instance();

	CCmdOptionList options(g_description);
	options.add(make_opt( in_filename, "in-files", 'i', "input image(s)", 
				    "in-files", CCmdOption::required));
	options.add(make_opt( mask_filename, "mask-file", 'm', 
				    "mask image, must be of type byte", 
			      "mask-file", CCmdOption::required));

	if (options.parse(argc, argv, "image") != CCmdOptionList::hr_no) 
		return EXIT_SUCCESS; 

	if (!options.get_remaining().empty())
		throw runtime_error("unknown option given ...");


	CHistory::instance().append(argv[0], revision, options);

	size_t start_filenum = 0;
	size_t end_filenum  = 0;
	size_t format_width = 0;

	std::string src_basename = get_filename_pattern_and_range(in_filename, start_filenum, end_filenum, format_width);
	if (start_filenum >= end_filenum)
		throw invalid_argument(string("no files match pattern ") + src_basename);

	char new_line = cverb.show_debug() ? '\n' : '\r';

	C2DImageIOPluginHandler::Instance::PData  mask_image_list = image2dio.load(mask_filename);
	if (!mask_image_list.get() || mask_image_list->empty())
		throw invalid_argument("no mask found");

	const C2DUBImage *mask = dynamic_cast<const C2DUBImage *>( mask_image_list->begin()->get() );

	C2DStat stat(*mask);

	for (size_t i = start_filenum; i < end_filenum; ++i) {

		string src_name = create_filename(src_basename.c_str(), i);
		cvmsg() << new_line << "Read: " << i <<" out of "<< "[" 
			<< start_filenum<< "," << end_filenum << "] = " << src_name ;
		C2DImageIOPluginHandler::Instance::PData  in_image_list = image2dio.load(src_name);

		if (in_image_list.get() && in_image_list->size()) {
			accumulate(stat, **in_image_list->begin());
		}
	}
	cvmsg() << "\n";

	const vector<C2DStat::TSliceStat>& ss = stat.result();

	for (vector<C2DStat::TSliceStat>::const_iterator i = ss.begin(); i != ss.end(); ++i) {
		for (C2DStat::TSliceStat::const_iterator k = i->begin(); k != i->end(); ++k) {
			cout << k->second.second / k->second.first << " ";
		}
		cout << "\n";
	}

	return EXIT_SUCCESS;
}

#include <mia/internal/main.hh>
MIA_MAIN(do_main); 
