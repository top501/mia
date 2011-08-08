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
  Don't document this in the guide but trick grep 
  LatexEnd
*/

#include <mia/core.hh>
#include <mia/2d.hh>
#include <sstream>
#include <iomanip>
#include <boost/algorithm/minmax_element.hpp>

NS_MIA_USE
using namespace boost; 
using namespace std; 

const char *g_description = 
	"This program implements 2D gray scale image registration. "
	"Depending on the available plug-ins it can accomodate various models and cost-functions. "
	"So far input images can be given as PNG, TIF, BMP and OpenEXR (depending on the installed plug-ins). "
	"The input images must be of the same dimensions and gray scale (whatever bit-depth). "
	"The vector field will be saved as a EXR image with two frames of float values, one for the X "
	"and one for the Y values of the vectors. "
	"Other vector field output plug-ins might be supported depending on the installed plug-ins. "; 


// set op the command line parameters and run the registration 
int do_main(int argc, const char **argv)
{

	CCmdOptionList options(g_description);
	string out_filename; 
	string regmodel("navier"); 
	string timestep("fluid"); 
	int start_size = 16; 
	EInterpolation interpolator = ip_bspline3; 
	int max_iter = 200; 
	float epsilon = 0.01; 

	options.add(make_opt( out_filename, "out-file", 'o', "output vector field", "output", true));
	options.add(make_opt( regmodel, "regmodel", 'm', "registration model", "model", false));
	options.add(make_opt( timestep, "timestep", 't', "time setp", "timestep", false));
	options.add(make_opt( start_size, "mgsize", 's', "multigrid start size", "start", false));
	options.add(make_opt( max_iter, "max-iter", 'n', ",maximum number of iterations", "iter", false));
	options.add(make_opt( interpolator, GInterpolatorTable ,"interpolator", 'p', 
					"image interpolator", "interpolator", false)); 
	options.add(make_opt( epsilon, "epsilon", 'e', "relative accuracy to stop registration at a multi-grid level", NULL, false)); 

	options.parse(argc, argv);

	vector<const char *> cost_chain = options.get_remaining(); 

	if (cost_chain.empty()) {
		cerr << "require cost functions given as extra parameters\n";
		return EXIT_FAILURE;
	}

	P2DRegModel model = C2DRegModelPluginHandler::instance().produce(regmodel.c_str()); 
	P2DRegTimeStep time_step = C2DRegTimeStepPluginHandler::instance().produce(timestep.c_str()); 
	
	if (!model || !time_step)
		return EXIT_FAILURE;

	
	std::shared_ptr<C2DInterpolatorFactory > ipf(create_2dinterpolation_factory(interpolator)); 
	if (!ipf)
		throw invalid_argument("unknown interpolator requested"); 

	C2DImageFatCostList cost_list; 
	for(vector<const char *>::const_iterator i = cost_chain.begin(); i != cost_chain.end(); ++i) {
		P2DImageFatCost c = C2DFatImageCostPluginHandler::instance().produce(*i); 
		if (c) 
			cost_list.push_back(c);
	}
	if (cost_list.empty()) {
		cerr << "Could not create a single cost function\n"; 
		return EXIT_FAILURE; 
	}

	C2DMultiImageRegister reg(start_size, max_iter, *model,  *time_step, epsilon); 

	if (!reg(cost_list, ipf)->save(out_filename, "")) {
		cerr << "Unable to save result vector field to " << out_filename << "\n"; 
		return EXIT_FAILURE; 
	}
	
	return EXIT_SUCCESS; 
}

// for readablility the real main function encapsulates the do_main in a try-catch block
int main(int argc, const char **argv)
{
	try {
		return do_main(argc, argv); 
	}
	catch (invalid_argument& err) {
		cerr << "invalid argument: " << err.what() << "\n"; 
	}
	catch (runtime_error& err) {
		cerr << "runtime error: " << err.what() << "\n"; 
	}
	catch (std::exception& err) {
		cerr << "exception: " << err.what() << "\n"; 
	}
	catch (...) {
		cerr << "unknown exception\n";
	}
	return EXIT_FAILURE; 
}
