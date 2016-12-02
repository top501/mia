/* -*- mia-c++  -*-
 *
 * This file is part of MIA - a toolbox for medical image analysis 
 * Copyright (c) Leipzig, Madrid 1999-2016 Gert Wollny
 *
 * MIA is free software; you can redistribute it and/or modify
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
 * along with MIA; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <fstream>
#include <libxml++/libxml++.h>
#include <boost/filesystem.hpp>

#include <mia/internal/main.hh>
#include <mia/core/msgstream.hh>
#include <mia/core/cmdlineparser.hh>
#include <mia/core/errormacro.hh>
#include <mia/core/minimizer.hh>
#include <mia/core/ica.hh>
#include <mia/core/attribute_names.hh>
#include <mia/2d/nonrigidregister.hh>
#include <mia/2d/perfusion.hh>
#include <mia/2d/imageio.hh>
#include <mia/2d/segsetwithimages.hh>
#include <mia/2d/transformfactory.hh>


using namespace std;
using namespace mia;

namespace bfs=boost::filesystem; 

const char *g_program_group = "Registration of series of 2D images"; 
const char *g_general_help = 
	"This program implements the motion compensation algorithm described in "
	"Wollny G, Kellman P, Santos A, Ledesma-Carbayo M-J, \"Automatic Motion Compensation of "
	"Free Breathing acquired Myocardial Perfusion Data by using Independent Component Analysis\" "
	"Medical Image Analysis, 2012, DOI:10.1016/j.media.2012.02.004."; 
const char *g_program_example_descr = 
	"Register the perfusion series given in 'segment.set' by using automatic ICA estimation. " 
        "Skip two images at the beginning and otherwiese use the default parameters. "
	"Store the result in 'registered.set'."; 
const char *g_program_example_code = 
	"  -i segment.set -o registered.set -k 2"; 

const SProgramDescription description = {
        {pdi_group, g_program_group}, 
	{pdi_short, "Run a registration of a series of 2D images."}, 
	{pdi_description, g_general_help}, 
	{pdi_example_descr, g_program_example_descr}, 
	{pdi_example_code, g_program_example_code}
}; 

C2DFullCostList create_costs(P2DFullCost imagecost)
{
	C2DFullCostList result; 
	result.push(imagecost); 
	return result; 
}

P2DTransformationFactory create_transform_creator(size_t c_rate, float penalty)
{
	stringstream transf; 
	transf << "spline:rate=" << c_rate 
	       << ",imgboundary=mirror,imgkernel=[bspline:d=3],penalty=[divcurl:weight="
	       << penalty << "]"; 
	cvinfo() << "Transform:" <<  transf.str() << "\n"; 
	return C2DTransformCreatorHandler::instance().produce(transf.str()); 
}
	

void segment_and_crop_input(CSegSetWithImages&  input_set, 
			    const C2DPerfusionAnalysis& ica, 
			    float box_scale, 
			    C2DPerfusionAnalysis::EBoxSegmentation segmethod, 
			    C2DImageSeries& references, 
			    const string& save_crop_feature)
{
	C2DBounds crop_start; 
	auto cropper = ica.get_crop_filter(box_scale, crop_start, 
					   segmethod, save_crop_feature); 
	if (!cropper) {
		cvwarn() << "Cropping was requested, but segmentation failed - continuing at full image size\n";
		return; 
	}
	
	C2DImageSeries input_images = input_set.get_images(); 
	for(auto i = input_images.begin(); i != input_images.end(); ++i)
		*i = cropper->filter(**i); 
	
	for (auto i = references.begin(); i != references.end(); ++i) 
		*i = cropper->filter(**i); 
	
	auto tr_creator = C2DTransformCreatorHandler::instance().produce("translate");
	P2DTransformation shift = tr_creator->create(C2DBounds(1,1)); 
	auto p = shift->get_parameters(); 
	p[0] = crop_start.x; 
	p[1] = crop_start.y; 
	shift->set_parameters(p); 
	
	input_set.transform(*shift);
	input_set.set_images(input_images);  
}

void run_registration_pass(CSegSetWithImages&  input_set, 
			   const C2DImageSeries& references, 
			   int skip_images, PMinimizer minimizer, size_t mg_levels, 
			   double c_rate, double divcurlweight, P2DFullCost imagecost, 
			   PMinimizer refinement_minimizer)
{
	CSegSetWithImages::Frames& frames = input_set.get_frames();
	C2DImageSeries input_images = input_set.get_images(); 
	auto costs  = create_costs(imagecost); 
	auto transform_creator = create_transform_creator(c_rate, divcurlweight); 
	C2DNonrigidRegister nrr(costs, minimizer,  transform_creator, mg_levels);
	if (refinement_minimizer)
		nrr.set_refinement_minimizer(refinement_minimizer); 


	// this loop could be parallized 
	for (size_t i = 0; i < input_images.size() - skip_images; ++i) {
		cvmsg() << "Register frame " << i << "\n"; 
		P2DTransformation transform = nrr.run(input_images[i + skip_images], 
						      references[i]);
		input_images[i + skip_images] = 
			(*transform)(*input_images[i + skip_images]);
		frames[i + skip_images].inv_transform(*transform);
	}
	input_set.set_images(input_images); 
}

void save_references(const string& save_ref, int current_pass, int skip_images, const C2DImageSeries& references)
{
	for(size_t i = 0 ; i < references.size(); ++i) {
		stringstream filename; 
		filename << save_ref << current_pass << "-" 
			 << setw(4) << setfill('0') << skip_images + i << ".v"; 
		save_image(filename.str(), references[i]);
	}
}

float get_relative_min_breathing_frequency(const C2DImageSeries& images, int skip, float min_breathing_frequency)
{
	if (min_breathing_frequency <= 0) 
		return -1; 
	int n_heartbeats = images.size() - skip; 
	auto image_begin =  images[skip]; 
	auto image_end = images[images.size() - 1]; 

	if (image_begin->has_attribute("AcquisitionTime") && image_end->has_attribute(IDAcquisitionTime)) {
		double aq_time = image_end->get_attribute_as<double>(IDAcquisitionTime) - 
			image_begin->get_attribute_as<double>(IDAcquisitionTime);
		if (aq_time < 0) 
			throw create_exception<runtime_error>("Got non-postive acquisition time range ", aq_time, 
							      ", can't handle this");  
							      
		double heart_rate = 60 * n_heartbeats / aq_time; 
		cvmsg() << "Read a heartbeat rate of " << heart_rate << " beats/min\n";
		return heart_rate / min_breathing_frequency; 
	}else 
		return -1; 
}

int do_main( int argc, char *argv[] )
{
	// IO parameters 
	string in_filename;
	string out_filename;
	string registered_filebase("reg");

	// debug options: save some intermediate steps 
	string cropped_filename;
	string save_crop_feature; 
	string save_ref_filename;
	string save_reg_filename;

	// this parameter is currently not exported - reading the image data is 
	// therefore done from the path given with the segmentation set 
	bool override_src_imagepath = true;

	// registration parameters
	PMinimizer minimizer;
	PMinimizer refinement_minimizer;
	P2DFullCost imagecost;
	double c_rate = 16; 
	double c_rate_divider = 2; 
	double divcurlweight = 10.0; 
	double divcurlweight_divider = 2.0; 

	size_t mg_levels = 3; 

	// ICA parameters 
	size_t components = 0;
	bool normalize = false; 
	bool no_meanstrip = false; 
	float box_scale = 0.0;
	size_t skip_images = 0; 
	size_t max_ica_iterations = 400; 
	C2DPerfusionAnalysis::EBoxSegmentation 
		segmethod=C2DPerfusionAnalysis::bs_features; 
	PIndepCompAnalysisFactory icatool;
	float min_breathing_frequency = -1.0f; 

	size_t current_pass = 0; 
	size_t pass = 5; 

	CCmdOptionList options(description);
	
	options.set_group("File-IO"); 
	options.add(make_opt( in_filename, "in-file", 'i', 
				    "input perfusion data set", CCmdOptionFlags::required_input));
	options.add(make_opt( out_filename, "out-file", 'o', 
				    "output perfusion data set", CCmdOptionFlags::required_output));
	options.add(make_opt( registered_filebase, "registered", 'r', 
				    "file name base for registered fiels")); 
	
	options.add(make_opt( cropped_filename, "save-cropped", 0, 
			      "save cropped set to this file")); 
	options.add(make_opt( save_crop_feature, "save-feature", 0, "save the features images resulting from the ICA and "
			      "some intermediate images used for the RV-LV segmentation with the given file name base to PNG files. "
			      "Also save the coefficients of the initial best and the final IC mixing matrix.")); 
	
	options.add(make_opt( save_ref_filename, "save-refs", 0, 
			      "save synthetic reference images")); 
	options.add(make_opt( save_reg_filename, "save-regs", 0, 
			      "save intermediate registered images")); 
	
	
	
	options.set_group("Registration"); 
	options.add(make_opt( minimizer, "gsl:opt=gd,step=0.1", "optimizer", 'O', "Optimizer used for minimization"));
	options.add(make_opt( refinement_minimizer, "", "refiner", 'R',
			      "optimizer used for refinement after the main optimizer was called"));
	options.add(make_opt( c_rate, "start-c-rate", 'a', 
			      "start coefficinet rate in spines,"
			      " gets divided by --c-rate-divider with every pass"));
	options.add(make_opt( c_rate_divider, "c-rate-divider", 0, 
			      "cofficient rate divider for each pass"));
	options.add(make_opt( divcurlweight, "start-divcurl", 'd',
			      "start divcurl weight, gets divided by"
			      " --divcurl-divider with every pass")); 
	options.add(make_opt( divcurlweight_divider, "divcurl-divider", 0,
			      "divcurl weight scaling with each new pass")); 
	options.add(make_opt( imagecost, "image:weight=1,cost=ssd", "imagecost", 'w', "image cost")); 
	options.add(make_opt( mg_levels, "mg-levels", 'l', "multi-resolution levels"));
	options.add(make_opt( pass, "passes", 'P', "registration passes")); 
	
	options.set_group("ICA");
	options.add(make_opt( icatool, "internal", "fastica", 0, "FastICA implementationto be used"));
	options.add(make_opt( components, "components", 'C', "ICA components 0 = automatic estimation"));
	options.add(make_opt( normalize, "normalize", 0, "normalized ICs"));
	options.add(make_opt( no_meanstrip, "no-meanstrip", 0, 
			      "don't strip the mean from the mixing curves"));
	options.add(make_opt( box_scale, "segscale", 's', 
			      "segment and scale the crop box around the LV (0=no segmentation)"));
	options.add(make_opt( skip_images, "skip", 'k', "skip images at the beginning of the series "
			      "e.g. because as they are of other modalities")); 
	options.add(make_opt( max_ica_iterations, "max-ica-iter", 'm', "maximum number of iterations in ICA")); 
	
	options.add(make_opt(segmethod , C2DPerfusionAnalysis::segmethod_dict, "segmethod", 'E', 
				   "Segmentation method")); 
	options.add(make_opt(min_breathing_frequency, "min-breathing-frequency", 'b', 
			     "minimal mean frequency a mixing curve can have to be considered to stem from brething. "
			     "A healthy rest breating rate is 12 per minute. A negative value disables the test.")); 
 	
	if (options.parse(argc, argv) != CCmdOptionList::hr_no) 
		return EXIT_SUCCESS; 

	// load input data set
	CSegSetWithImages  input_set(in_filename, override_src_imagepath);
	C2DImageSeries input_images = input_set.get_images(); 

	float rel_min_bf = get_relative_min_breathing_frequency(input_images,  skip_images, min_breathing_frequency); 
	
	cvmsg() << "skipping " << skip_images << " images\n"; 
	vector<C2DFImage> series(input_images.size() - skip_images); 
	transform(input_images.begin() + skip_images, input_images.end(), 
		  series.begin(), FCopy2DImageToFloatRepn()); 
	

	// run ICA
	unique_ptr<C2DPerfusionAnalysis> ica(new C2DPerfusionAnalysis(components, normalize, !no_meanstrip)); 
	if (max_ica_iterations) 
		ica->set_max_ica_iterations(max_ica_iterations); 

	if (rel_min_bf > 0) 
		ica->set_min_movement_frequency(rel_min_bf); 

    ica->set_approach(CIndepCompAnalysis::appr_defl);
    if (!ica->run(series, *icatool)) {
		ica.reset(new C2DPerfusionAnalysis(components, normalize, !no_meanstrip)); 
        ica->set_approach(CIndepCompAnalysis::appr_symm);
        if (!ica->run(series, *icatool))
			box_scale = false; 
	}
	
	vector<C2DFImage> references_float = ica->get_references(); 
	
	C2DImageSeries references(references_float.size()); 
	transform(references_float.begin(), references_float.end(), references.begin(), 
		  FWrapStaticDataInSharedPointer<C2DImage>()); 

	// crop if requested
	if (box_scale) {
		segment_and_crop_input(input_set, *ica, box_scale, segmethod, references, save_crop_feature); 
		input_images = input_set.get_images(); 
	}else if (!save_crop_feature.empty()) {
		stringstream cfile; 
		cfile << save_crop_feature << "-coeff.txt"; 
		ica->save_coefs(cfile.str()); 
	}

	// save cropped images if requested
	if (!cropped_filename.empty()) {
		bfs::path cf(cropped_filename);
		cf.replace_extension(); 
		input_set.rename_base(cf.filename().string()); 
		input_set.save_images(cropped_filename);

		unique_ptr<xmlpp::Document> test_cropset(input_set.write());
		ofstream outfile(cropped_filename, ios_base::out );
		if (outfile.good())
			outfile << test_cropset->write_to_string_formatted();
		else 
			throw create_exception<runtime_error>( "unable to save to '", cropped_filename, "'"); 

	}

	bool do_continue=true; 
	bool lastpass = false; 
	do {
		++current_pass; 
		cvmsg() << "Registration pass " << current_pass << "\n"; 

		if (!save_ref_filename.empty())
			save_references(save_ref_filename, current_pass, skip_images, references); 

		run_registration_pass(input_set, references,  skip_images,  minimizer, 
				      mg_levels, c_rate, divcurlweight, imagecost, refinement_minimizer); 
		

		if (!save_reg_filename.empty()) 
			save_references(save_reg_filename, current_pass, 0, input_set.get_images()); 

		C2DPerfusionAnalysis ica2(components, normalize, !no_meanstrip); 
		if (max_ica_iterations) 
			ica2.set_max_ica_iterations(max_ica_iterations); 
	
		transform(input_set.get_images().begin() + skip_images, 
			  input_set.get_images().end(), series.begin(), FCopy2DImageToFloatRepn()); 

        if (!ica2.run(series, *icatool)) {
            ica2.set_approach(CIndepCompAnalysis::appr_symm);
            ica2.run(series, *icatool);
		}
		if (lastpass) 
			break; 
		
		divcurlweight /= divcurlweight_divider; 
		if (c_rate > 1) 
			c_rate /= c_rate_divider; 
		references_float = ica2.get_references(); 
		transform(references_float.begin(), references_float.end(), 
			  references.begin(), FWrapStaticDataInSharedPointer<C2DImage>()); 
		do_continue =  (!pass || current_pass < pass) && ica2.has_movement(); 
		
		// run one more pass if the limit is not reached and no movement identified
		lastpass = (!do_continue && (!pass || current_pass < pass)); 
		
	} while (do_continue || lastpass); 


	// run a final ICA pass to update the RV/LV peak 
	C2DPerfusionAnalysis ica_final(4, normalize, !no_meanstrip); 
	if (max_ica_iterations) 
		ica_final.set_max_ica_iterations(max_ica_iterations); 
	
	transform(input_set.get_images().begin() + skip_images, 
		  input_set.get_images().end(), series.begin(), FCopy2DImageToFloatRepn()); 
	
    if (!ica_final.run(series, *icatool)) {
            ica_final.set_approach(CIndepCompAnalysis::appr_symm);
            ica_final.run(series, *icatool);
	}
	if( input_set.get_RV_peak() < 0) 
		input_set.set_RV_peak(ica_final.get_RV_peak_time() + skip_images); 
	if( input_set.get_LV_peak() < 0) 
		input_set.set_LV_peak(ica_final.get_LV_peak_time() + skip_images);

	if (!save_crop_feature.empty()) {
	

		stringstream cfile; 
		cfile << save_crop_feature << "-final.txt"; 
		ica_final.save_coefs(cfile.str()); 
		stringstream new_base; 
		new_base << save_crop_feature << "-p"<< pass << "-final"; 
		ica_final.save_feature_images(new_base.str()); 
	}
	
	input_set.rename_base(registered_filebase); 
	input_set.save_images(out_filename); 
	
	unique_ptr<xmlpp::Document> outset(input_set.write());
	ofstream outfile(out_filename.c_str(), ios_base::out );
	if (outfile.good())
		outfile << outset->write_to_string_formatted();
	
	return outfile.good() ? EXIT_SUCCESS : EXIT_FAILURE;

}

MIA_MAIN(do_main); 

