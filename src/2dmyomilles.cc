/* -*- mona-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004 - 2010
 *
 * Max-Planck-Institute for Human Cognitive and Brain Science	
 * Max-Planck-Institute for Evolutionary Anthropology 
 * BIT, ETSI Telecomunicacion, UPM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#define VSTREAM_DOMAIN "2dmilles"

#include <fstream>
#include <libxml++/libxml++.h>
#include <boost/filesystem.hpp>

#include <mia/core/msgstream.hh>
#include <mia/core/cmdlineparser.hh>
#include <mia/core/errormacro.hh>
#include <mia/2d/rigidregister.hh>
#include <mia/2d/perfusion.hh>
#include <mia/2d/SegSetWithImages.hh>
#include <mia/2d/transformfactory.hh>

using namespace std;
using namespace mia;

namespace bfs=boost::filesystem; 

const TDictMap<EMinimizers>::Table g_minimizer_table[] = {
	{"simplex", min_nmsimplex},
	{"cg-fr", min_cg_fr},
	{"cg-pr", min_cg_pr},
	{"bfgs", min_bfgs},
	{"bfgs2", min_bfgs2},
	{"gd", min_gd},
	{NULL, min_undefined}
};


class C2DFImage2PImage {
public: 
	P2DImage operator () (const C2DFImage& image) const {
		return P2DImage(new C2DFImage(image)); 
	}
}; 

class Convert2Float {
public: 
	C2DFImage operator () (P2DImage image) const; 
private: 
	FConvert2DImage2float _M_converter; 
}; 

int do_main( int argc, const char *argv[] )
{
	// IO parameters 
	string in_filename;
	string out_filename;
	string registered_filebase("reg");

	// debug options: save some intermediate steps 
	string cropped_filename;
	string save_crop_feature; 

	// this parameter is currently not exported - reading the image data is 
	// therefore done from the path given in the segmentation set 
	bool override_src_imagepath = true;

	// registration parameters
	string cost_function("ssd"); 
	EMinimizers minimizer = min_nmsimplex;
	string transform_type("translate");
	EInterpolation interpolator = ip_bspline3;
	size_t mg_levels = 3; 

	// ICA parameters 
	size_t components = 0;
	bool no_normalize = false; 
	bool no_meanstrip = false; 
	float box_scale = 1.4;
	size_t skip_images = 0; 
	size_t max_ica_iterations = 400; 
	C2DPerfusionAnalysis::EBoxSegmentation segmethod=C2DPerfusionAnalysis::bs_features; 

	size_t current_pass = 0; 
	size_t pass = 2; 

	CCmdOptionList options;
	options.push_back(make_opt( in_filename, "in-file", 'i', "input perfusion data set", "input", true));
	options.push_back(make_opt( out_filename, "out-file", 'o', "output perfusion data set", "output", true));
	options.push_back(make_opt( registered_filebase, "registered", 'r', "file name base for registered fiels", 
				    "registered", false)); 
	
	options.push_back(make_opt( cropped_filename, "save-cropped", 0, "save cropped set to this file", NULL)); 
	options.push_back(make_opt( save_crop_feature, "save-feature", 0, "save segmentation feature images", NULL)); 

	options.push_back(make_opt( cost_function, "cost", 'c', "registration criterion", "cost", false)); 
	options.push_back(make_opt( minimizer, TDictMap<EMinimizers>(g_minimizer_table),
				    "optimizer", 'O', "Optimizer used for minimization", "optimizer", false));
	options.push_back(make_opt( transform_type, "transform", 'f', "transformation typo", "transform", false));
	options.push_back(make_opt( interpolator, GInterpolatorTable ,"interpolator", 'p',
				    "image interpolator", NULL));
	options.push_back(make_opt( mg_levels, "mg-levels", 'l', "multi-resolution levels", "mg-levels", false));

	options.push_back(make_opt( pass, "passes", 'P', "registration passes", "passes")); 


	options.push_back(make_opt( components, "components", 'C', "ICA components 0 = automatic estimation", NULL));
	options.push_back(make_opt( no_normalize, "no-normalize", 0, "don't normalized ICs", NULL));
	options.push_back(make_opt( no_meanstrip, "no-meanstrip", 0, 
				    "don't strip the mean from the mixing curves", NULL));
	options.push_back(make_opt( box_scale, "segscale", 's', 
				    "segment and scale the crop box around the LV (0=no segmentation)", "segscale"));
	options.push_back(make_opt( skip_images, "skip", 'k', "skip images at the beginning of the series "
				    "as they are of other modalities", "skip")); 
	options.push_back(make_opt( max_ica_iterations, "max-ica-iter", 'm', "maximum number of iterations in ICA", 
				    "ica-iter", false)); 

	options.push_back(make_opt(segmethod , C2DPerfusionAnalysis::segmethod_dict, "segmethod", 'E', 
				   "Segmentation method", "segmethod")); 
				    

	options.parse(argc, argv, false);

	// prepare registration class
	
	unique_ptr<C2DInterpolatorFactory>   ipfactory(create_2dinterpolation_factory(interpolator));
	C2DRigidRegister rigid_register(C2DImageCostPluginHandler::instance().produce("ssd"), 
					minimizer, transform_type, *ipfactory); 
					
					
	cvwarn() << "save_crop_feature:" << save_crop_feature << "\n"; 
	
	
	// load input data set
	CSegSetWithImages  input_set(in_filename, override_src_imagepath);
	C2DImageSeries input_images = input_set.get_images(); 
	
	cvmsg() << "skipping " << skip_images << " images\n"; 
	vector<C2DFImage> series(input_images.size() - skip_images); 
	transform(input_images.begin() + skip_images, input_images.end(), 
		  series.begin(), Convert2Float()); 
	

	// run ICA
	C2DPerfusionAnalysis ica(components, !no_normalize, !no_meanstrip); 
	if (max_ica_iterations) 
		ica.set_max_ica_iterations(max_ica_iterations); 
	if (!ica.run(series)) 
		throw runtime_error("ICA analysis didn't result in usable components"); 
	vector<C2DFImage> references_float = ica.get_references(); 
	
	C2DImageSeries references(references_float.size()); 
	transform(references_float.begin(), references_float.end(), references.begin(), C2DFImage2PImage()); 

	// crop if requested
	if (box_scale) {
		C2DBounds crop_start; 
		auto cropper = ica.get_crop_filter(box_scale, crop_start, segmethod, save_crop_feature); 
		if (!cropper) {
			THROW(runtime_error, "Cropping was requested, but segmentation failed"); 
		}
		
		for(auto i = input_images.begin(); i != input_images.end(); ++i)
			*i = cropper->filter(**i); 

		for (auto i = references.begin(); i != references.end(); ++i) 
			*i = cropper->filter(**i); 

		auto tr_creator = C2DTransformCreatorHandler::instance().produce("translate");
		P2DTransformation shift = tr_creator->create(C2DBounds(1,1)); 
		auto p = shift->get_parameters(); 
		p[0] = (float)crop_start.x; 
		p[1] = (float)crop_start.y; 
		shift->set_parameters(p); 
		
		input_set.transform(*shift);
		input_set.set_images(input_images);  
	}
	
	if (!cropped_filename.empty()) {
		bfs::path cf(cropped_filename);
		cf.replace_extension(); 
		input_set.rename_base(cf.filename()); 
		input_set.save_images(cropped_filename);

		unique_ptr<xmlpp::Document> test_cropset(input_set.write());
		ofstream outfile(cropped_filename, ios_base::out );
		if (outfile.good())
			outfile << test_cropset->write_to_string_formatted();
		else 
			THROW(runtime_error, "unable to save to '" << cropped_filename << "'"); 

	}
	

	CSegSetWithImages::Frames& frames = input_set.get_frames();

	for (size_t i = 0; i < input_images.size() - skip_images; ++i) {
		cvmsg() << "Register 1st pass, frame " << i << "\n"; 
		P2DTransformation transform = rigid_register.run(input_images[i + skip_images] , 
							     references[i], mg_levels);
		input_images[i + skip_images] = (*transform)(*input_images[i + skip_images], *ipfactory);
		P2DTransformation inverse(transform->invert()); 
		frames[i + skip_images].transform(*inverse);
	}

	// run the specified number of passes 
	// break early if ICA fails
	while (++current_pass < pass) {
		C2DPerfusionAnalysis ica2(components, !no_normalize, !no_meanstrip); 
		if (max_ica_iterations) 
			ica2.set_max_ica_iterations(max_ica_iterations); 
	
		transform(input_images.begin() + skip_images, 
			  input_images.end(), series.begin(), Convert2Float()); 
		if (ica2.run(series) ) {
			references_float = ica2.get_references(); 
			transform(references_float.begin(), references_float.end(), 
				  references.begin(), C2DFImage2PImage()); 
			
			for (size_t i = 0; i < input_images.size() - skip_images; ++i) {
				cvmsg() << "Register " << current_pass + 1 <<  " pass, frame " << i << "\n"; 
				P2DTransformation transform = rigid_register.run(input_images[i + skip_images] , 
										 references[i], mg_levels); 
				input_images[i + skip_images] = (*transform)(*input_images[i + skip_images], *ipfactory);
				P2DTransformation inverse(transform->invert()); 
				frames[i + skip_images].transform(*inverse);
			}
		} else {
			cvmsg() << "Stopping registration in pass " << current_pass 
				<< " because ICA didn't return useful results\n"; 
			break; 
		}
	}


	input_set.set_images(input_images); 
	input_set.rename_base(registered_filebase); 

	input_set.save_images(out_filename); 
	
	unique_ptr<xmlpp::Document> outset(input_set.write());
	ofstream outfile(out_filename.c_str(), ios_base::out );
	if (outfile.good())
		outfile << outset->write_to_string_formatted();
	
	return outfile.good() ? EXIT_SUCCESS : EXIT_FAILURE;

}



int main( int argc, const char *argv[] )
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

inline C2DFImage Convert2Float::operator () (P2DImage image) const
{
	return ::mia::filter(_M_converter, *image); 
}
