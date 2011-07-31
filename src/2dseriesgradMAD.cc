/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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


/*

  LatexBeginProgramDescription{Myocardial Perfusion Analysis}
  
  \subsection{mia-2dseriesgradMAD}
  \label{mia-2dseriesgradMAD}

  \begin{description} 
  \item [Description:] 
           Given a set of images of temporal sucession, evaluates the pixel-wise 
	   temporal gradient and then its \emph{median average distance} (MAD) 
           and stores the result in an image. 
	   Spacial pre-filtering may be applied as additional plugin(s). 

  The program is called like 
  \begin{lstlisting}
mia-2dseriesgradMAD -i <input set> -o <output image> [options] [<filter>] ... 
  \end{lstlisting}
  with the filters given as extra parameters as additional command line parameters. 

  \item [Options:] $\:$

  \optiontable{
  \optinfile
  \optoutfile
  \cmdopt{skip}{k}{int}{Skip a number of frames at the beginning of the series}
  \cmdopt{crop}{c}{}{Crop the images before evaluating the MAD. Cropping is done by evaluating a bounding box 
                         that contains the segmentation given in the images. 
                         If no segmentation is available then the result is undefined.}
  \cmdopt{enlarge-boundary}{e}{int}{Enlarge the boundary of the obtained crop-box}
  }

  Additional 2D filters may be given at the command line to run a slice-wise filtering (section \ref{sec:filter2d}). 

  \item [Example:]Evaluate the MAD-image of the bounding box surrounding the segmentation 
                  from a series segment.set. No spacial filtering will be applied. 
		  The bounding box will be enlarged by 3 pixels in all directions.
		  Store the image in OpenEXR format.  
  \begin{lstlisting}
mia-2dseriesgradMAD -i segment.set -o mad.exr -c -e 3 
  \end{lstlisting}
  \item [Remark:] The MAD image has float-valued pixels and thereby requires an output format that supports 
                  this pixel type. 
  \item [See also:] \sa{mia-2dmultiimagevar}, \sa{mia-2dseriessmoothgradMAD}, 
                    \sa{mia-2dseriesgradvariation}
  \end{description}
  
  LatexEnd
*/


#define VSTREAM_DOMAIN "SERGRADVAR"

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <boost/algorithm/minmax_element.hpp>

#include <mia/2d/filterchain.hh>
#include <mia/2d/2dimageio.hh>
#include <mia/2d/SegSetWithImages.hh>
#include <mia/core.hh>

using namespace std;
using namespace mia;

static const char *program_info = 
	"Given a set of images of temporal sucession, evaluate the temporal \n"
	"pixel-wise gradient MAD.\n" 
	"A spacial pre-filtering may be applied as additional plugins\n"
	"Usage:\n"
	"  mia-2dseriesgradMAD -i <input set> -o <output image> [<options>] [<spacial filter(s)>]\n"; 


template <typename T>
struct fabsdelta {
	T operator () (T x, T y) const {
		return fabs(x - y);
	}
};
struct C2DVarAccumulator : public TFilter<bool> {
	typedef vector<float> CBuffer;

	C2DVarAccumulator(size_t n):
		m_min(numeric_limits<float>::max()),
		m_max(-numeric_limits<float>::max()),
		m_n(n - 1),
		m_initialized(false)
	{
	}

	template <typename T>
	bool operator ()(const T2DImage<T>& image)
	{


		if (m_initialized) {
			if (image.get_size() != m_old.get_size())
				throw invalid_argument("Input image and mask differ in size");

			transform(image.begin(), image.end(), m_old.begin(), m_delta.begin(),
				  fabsdelta<float>());
			C2DFImage::const_iterator i = m_delta.begin();
			C2DFImage::const_iterator e = m_delta.end();
			vector<CBuffer>::iterator v = m_field.begin();

			for(; i != e; ++i, ++v)
				v->push_back(*i);

			pair<typename T2DImage<T>::const_iterator, typename T2DImage<T>::const_iterator>
				src_minmax = ::boost::minmax_element(image.begin(), image.end());
			if (m_min > *src_minmax.first)
				m_min = *src_minmax.first;
			if (m_max < *src_minmax.second)
				m_max = *src_minmax.second;

		}else{
			m_old = C2DFImage(image.get_size());
			m_delta = C2DFImage(image.get_size());
			m_field.resize(image.size());
			m_initialized = true;
		}
		copy(image.begin(), image.end(), m_old.begin());

		return true;
	}
	static float median( CBuffer::iterator begin,
			     CBuffer::iterator end, size_t len)
	{
		if (len & 1) {
			CBuffer::iterator i = begin + (len - 1) / 2;
			std::nth_element(begin, i, end);
			return *i;
		}else {
			CBuffer::iterator i1 = begin + len / 2 - 1;
			CBuffer::iterator i2 = begin + len / 2;
			std::nth_element(begin, i1, end);
			std::nth_element(begin, i2, end);
			return (*i1 + *i2) / 2.0;
		}
	}

	P2DImage  result() {
		float range = 255.0 / (m_max - m_min);

		C2DFImage *variation = new C2DFImage(m_delta.get_size());

		C2DFImage::iterator ii = variation->begin();
		vector<CBuffer>::iterator iv = m_field.begin();
		vector<CBuffer>::iterator ev = m_field.end();
		while (iv != ev) {
			*ii++ = median(iv->begin(), iv->end(), m_n);
			++iv;
		}

		ii = variation->begin();
		iv = m_field.begin();

		while (iv != ev) {
			CBuffer::iterator ip = iv->begin();
			CBuffer::iterator ep = iv->end();

			while (ip != ep) {
				*ip = fabs(*ip - *ii);
				++ip;
			}
			++iv;
			++ii;
		}
		ii = variation->begin();
		iv = m_field.begin();
		while (iv != ev) {
			*ii++ = median(iv->begin(), iv->end(), m_n) * range;
			++iv;
		}
		return P2DImage(variation);
	}

private:
	float m_min;
	float m_max;
	C2DFImage m_old;
	C2DFImage m_delta;
	vector<CBuffer> m_field;
	size_t m_n;
	bool m_initialized;
};


/* Revision string */
const char revision[] = "not specified";

int main( int argc, const char *argv[] )
{

	string in_filename;
	string out_filename;
	string out_type;
	bool crop;
	size_t skip = 0;
	size_t enlarge_boundary = 5;

	const C2DImageIOPluginHandler::Instance& imageio = C2DImageIOPluginHandler::instance();

	CCmdOptionList options(program_info);
	options.add(make_opt( in_filename, "in-file", 'i', "input segmentation set", CCmdOption::required));
	options.add(make_opt( out_filename, "out-file", 'o', "output file name", CCmdOption::required));
	options.add(make_opt( skip, "skip", 'k', "Skip files at the beginning"));
	options.add(make_opt( enlarge_boundary,  "enlarge-boundary", 'e', 
				    "Enlarge cropbox by number of pixels"));
	options.add(make_opt( crop, "crop", 'c', "crop image before running statistics"));

	try {

		if (options.parse(argc, argv) != CCmdOptionList::hr_no)
			return EXIT_SUCCESS; 
		
		C2DFilterChain filter_chain(options.get_remaining());

		cvdebug() << "IO supported types: " << imageio.get_plugin_names() << "\n";


		if ( in_filename.empty() )
			throw runtime_error("'--in-file' ('i') option required");

		if ( out_filename.empty() )
			throw runtime_error("'--out-base' ('o') option required");

		CSegSetWithImages  segset(in_filename, true);

		if (crop) {
			C2DBoundingBox box = segset.get_boundingbox();
			box.enlarge(enlarge_boundary);
			stringstream crop_descr;
			crop_descr << "crop:"
				   << "start=[" << box.get_grid_begin()
				   << "],end=[" << box.get_grid_end() << "]";
			cvdebug() << "Crop with " << crop_descr.str() << "\r";


			filter_chain.push_front(C2DFilterPluginHandler::instance().
						produce(crop_descr.str().c_str()));
		}


		if (skip >= segset.get_images().size())
			throw invalid_argument("Skip is equal or larger then image series");

		C2DImageSeries::const_iterator iimages = segset.get_images().begin();
		C2DImageSeries::const_iterator eimages = segset.get_images().end();
		advance(iimages, skip);

		C2DVarAccumulator acc(distance(iimages, eimages));
		for (; iimages != eimages; ++iimages) {

			P2DImage in_image = *iimages;
			if (!filter_chain.empty() )
				in_image = filter_chain.filter(*in_image);
			mia::accumulate(acc, *in_image);
		}

		if (save_image(out_filename, acc.result()))
		    return  EXIT_SUCCESS;

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
