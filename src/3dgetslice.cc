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
  LatexBeginProgramDescription{Miscellaneous programs}

  \subsection{mia-3dgetslice}
  \label{mia-3dgetslice}
  
  \begin{description}
  \item [Description:] Get a number of slices from a 3D image and store them as 2D images. 
  \
  \begin{lstlisting}
mia-3dgetslice -i <input> -o <output> -s <start> -n <number> -d <direction>
  \end{lstlisting}
  \item [Options:] $\:$

  \optiontable{
  \optinfile
  \cmdopt{out-file}{o}{string}{Base name of the output file(s), slice number(s) and type suffix will be added.}
  \opttypetwod
  \cmdopt{start}{s}{int}{start slice to write out}
  \cmdopt{number}{n}{int}{Number of slices to get (0=all starting from \texttt{-{}-start})}
  \cmdopt{dir}{d}{string}{Slice direction (xy|axial|xz|coronal|yz|saggital)}
  }

  \item [Example:] Store 5 coronal slices starting at slice 120 coronalXXXX.png from  image.v: 
   \
  \begin{lstlisting}
mia-3dgetslice -i image.v -o coronal -t png -n 5 -s 120 
  \end{lstlisting}
  \end{description}

  LatexEnd
*/


#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>


#include <mia/3d/3dimageio.hh>
#include <mia/2d/2dimageio.hh>
#include <mia/core.hh>

using namespace mia;

const SProgramDescrption g_description = {
	"Miscellaneous programs", 
	
	"This program is used to extract 2D slices from a 3D data set and store them "
	"in separate files. Output files will be numbered according to their slice index."
	
	"Store 5 coronal slices starting at slice 120 coronalXXXX.png from  image.v."

	"-i image.v -o coronal -t png -n 5 -s 120" 
}; 

enum EDirection {dir_unkown, dir_xy, dir_xz, dir_yz};

const TDictMap<EDirection>::Table GDirectionmapTable[] = {
	{"xy", dir_xy, "get xy (axial) slice"},
	{"axial", dir_xy, "get xy (axial) slice"},
	{"xz", dir_xz, "get xz (coronal) slice"},
	{"coronal", dir_xz, "get xz (coronal) slice"},
	{"yz", dir_yz, "get yz (saggital) slice"},
	{"saggital", dir_yz, "get xz (saggital) slice"},
	{NULL, dir_unkown, ""}
};

const TDictMap<EDirection> GDirectionmap(GDirectionmapTable);

using namespace std;
NS_MIA_USE;

template <typename T, EDirection s_dir>
struct __dispatch {
};

template <typename T>
struct __dispatch<T, dir_xy> {
	static size_t get_end(size_t start, size_t n, const C3DBounds& size) {
		size_t end = n > 0 ? start + n : start + size.z;
		return ( end < size.z ) ? end : size.z;
	}
	static T2DImage<T> get_slice(size_t i, const T3DImage<T>& image) {
		return image.get_data_plane_xy(i);
	}
};

template <typename T>
struct __dispatch<T, dir_xz> {
	static size_t get_end(size_t start, size_t n, const C3DBounds& size) {
		size_t end = n > 0 ? start + n : start + size.y;
		return ( end < size.z ) ? end : size.y;
	}
	static T2DImage<T> get_slice(size_t i, const T3DImage<T>& image) {
		return image.get_data_plane_xz(i);
	}
};

template <typename T>
struct __dispatch<T, dir_yz> {
	static size_t get_end(size_t start, size_t n, const C3DBounds& size) {
		size_t end = n > 0 ? start + n : start + size.x;
		return ( end < size.z ) ? end : size.x;
	}
	static T2DImage<T> get_slice(size_t i, const T3DImage<T>& image) {
		return image.get_data_plane_yz(i);
	}
};


template <EDirection s_dir>
class TGetter : public TFilter<bool> {
public:
	TGetter(size_t start, size_t n, const string& fname, const string& type):
		m_start(start),
		m_n(n),
		m_fname(fname),
		m_type(type)
	{
	}

	template <typename T>
	bool operator ()(const T3DImage<T>& image) const
	{
		size_t end  = __dispatch<T, s_dir>::get_end(m_start, m_n, image.get_size());

		bool retval = true;
		for(size_t i = m_start; i < end; ++i) {
			P2DImage pimage(new  T2DImage<T>(__dispatch<T, s_dir>::get_slice(i, image)));
			stringstream out_name;
			out_name << m_fname << setw(4) << setfill('0') << i << "." << m_type;
			retval &= save_image(out_name.str(), pimage);
		}
		return retval;
	}
private:
	size_t m_start;
	size_t m_n;
	string m_fname;
	string m_type;
};

int main( int argc, const char *argv[] )
{

	string in_filename;
	string out_filename;
	string out_type("png");
	size_t start_slice = 0;
	size_t slice_number = 0;
	EDirection direction = dir_xy;

	try {
		const C2DImageIOPluginHandler::Instance& imageio2d = C2DImageIOPluginHandler::instance();

		CCmdOptionList options(g_description);
		options.add(make_opt( in_filename, "in-file", 'i', 
					    "input image(s) to be filtered", CCmdOption::required));
		options.add(make_opt( out_filename, "out-file", 'o', 
					    "output image(s) that have been filtered", CCmdOption::required));
		options.add(make_opt( out_type, imageio2d.get_set(), "type", 't',
					    "output file type"));
		options.add(make_opt( start_slice, "start", 's',"start slice number"));
		options.add(make_opt( slice_number, "number", 'n',
					    "number of slices (all=0)"));
		options.add(make_opt( direction, GDirectionmap, "dir", 'd', 
					    "slice direction (xy=axial, xz=coronal, yz=saggital)"));

		if (options.parse(argc, argv) != CCmdOptionList::hr_no)
			return EXIT_SUCCESS; 

		string out_suffix = imageio2d.get_preferred_suffix(out_type); 

		// read image
		auto in_image = load_image3d(in_filename);


		bool result = false;
                if (in_image) {
			switch (direction) {
			case dir_xy:
				result = mia::filter(TGetter<dir_xy>(start_slice, slice_number, 
								     out_filename, out_suffix), 
						     *in_image);
				break;
			case dir_xz:
				result = mia::filter(TGetter<dir_xz>(start_slice, slice_number, 
								     out_filename, out_suffix), 
						     *in_image);
				break;
			case dir_yz:
				result = mia::filter(TGetter<dir_yz>(start_slice, slice_number, 
								     out_filename, out_suffix), 
						     *in_image);
				break;
			default:
				assert(!"impossible slice direction");
				throw invalid_argument( "impossible slice direction");
			}
		}

		return result ? EXIT_SUCCESS : EXIT_FAILURE;

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


