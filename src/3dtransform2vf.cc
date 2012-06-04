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

#define VSTREAM_DOMAIN "mia-3dtransform"
#include <mia/core/cmdlineparser.hh>
#include <mia/3d/transformio.hh>
#include <mia/3d/3dvfio.hh>
#include <mia/internal/main.hh>


NS_MIA_USE
using namespace std;

const SProgramDescription g_description = {
	"Registration, Comparison, and Transformation of 3D images", 
	"Create Vectorfield from a transformation.", 

	"Create a 3D vector field from a given transformation. The output vector field will "
	"have the dimesions as given in the transformation description.", 
	
	"Transform the input transformation trans.v3df to a vector field field.vtk.",
	
	"-i trans.v3df   -o field.vtk"
}; 

int do_main(int argc, char **argv)
{
	CCmdOptionList options(g_description);

	string src_filename;
	string out_filename;


	options.add(make_opt( src_filename, "in-file", 'i', "input transformation ", CCmdOption::required));
	options.add(make_opt( out_filename, "out-file", 'o', "output vector field ", CCmdOption::required));

	if (options.parse(argc, argv) != CCmdOptionList::hr_no)
		return EXIT_SUCCESS; 
	

	auto transformation = C3DTransformationIOPluginHandler::instance().load(src_filename);

	C3DIOVectorfield outfield(transformation->get_size());

	cvdebug() << transformation->get_size() << "\n"; 
	auto it = transformation->begin(); 
	auto et = transformation->end(); 
	auto ivf = outfield.begin_range(C3DBounds::_0, transformation->get_size()); 
	
	while (it != et) {
		*ivf = C3DFVector(ivf.pos()) - *it; 
		cvdebug() << *ivf << "\n"; 
		++it; 
		++ivf; 
	}

	if (!C3DVFIOPluginHandler::instance().save(out_filename, outfield)){
		cerr << "Unable to save result vector field to " << out_filename << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;	
}

MIA_MAIN(do_main);
