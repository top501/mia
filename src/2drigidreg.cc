/* -*- mona-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2010
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


#include <sstream>
#include <mia/core.hh>
#include <mia/2d.hh>
#include <mia/2d/transformfactory.hh>
#include <gsl++/multimin.hh>

NS_MIA_USE; 
using namespace std;
using namespace gsl; 

enum EMinimizers {
	min_nmsimplex, 
	min_cg_fr, 
	min_cg_pr, 
	min_bfgs, 
	min_bfgs2, 
	min_gd, 
	min_undefined}; 

const TDictMap<EMinimizers>::Table g_minimizer_table[] = {
	{"simplex", min_nmsimplex}, 
	{"cg-fr", min_cg_fr}, 
	{"cg-pr", min_cg_pr}, 
	{"bfgs", min_bfgs}, 
	{"bfgs2", min_bfgs2}, 
	{"gd", min_gd},  
	{NULL, min_undefined}
}; 


bool minimizer_need_gradient(EMinimizers m) 
{
	switch (m) {
	case min_nmsimplex: return false; 
	case min_undefined: throw invalid_argument("Try to use minimizer 'undefined'"); 
	default: return true; 
	}
}

struct  UMinimzer{
		const gsl_multimin_fminimizer_type *fmin; 
		const gsl_multimin_fdfminimizer_type *fdfmin; 
}; 

UMinimzer minimizers[min_undefined] = {
	{ gsl_multimin_fminimizer_nmsimplex2, NULL }, 
	{ NULL, gsl_multimin_fdfminimizer_conjugate_fr }, 
	{ NULL, gsl_multimin_fdfminimizer_conjugate_pr }, 
	{ NULL, gsl_multimin_fdfminimizer_vector_bfgs }, 
	{ NULL, gsl_multimin_fdfminimizer_vector_bfgs2 }, 
	{ NULL, gsl_multimin_fdfminimizer_steepest_descent }
}; 

class CGradientProblem: public gsl::CFDFMinimizer::Problem {
public: 
	CGradientProblem(const C2DImage& model, const C2DImage& reference, C2DTransformation& transf, 
			 const C2DImageCost& _M_cost, const C2DInterpolatorFactory& _M_ipf); 
private: 
	double  do_f(const DoubleVector& x); 
	void    do_df(const DoubleVector& x, DoubleVector&  g); 
	double  do_fdf(const DoubleVector& x, DoubleVector&  g); 

	P2DImage apply(const DoubleVector& x); 
	const C2DImage& _M_model;
	const C2DImage& _M_reference;
	C2DTransformation& _M_transf; 
	const C2DImageCost& _M_cost; 
	const C2DInterpolatorFactory& _M_ipf; 
}; 
typedef shared_ptr<CGradientProblem> PGradientProblem; 

class CRegProblem: public gsl::CFMinimizer::Problem {
public: 
	CRegProblem(const C2DImage& model, const C2DImage& reference, C2DTransformation& transf, 
			 const C2DImageCost& _M_cost, const C2DInterpolatorFactory& _M_ipf); 
private: 
	double  do_f(const DoubleVector& x); 
	const C2DImage& _M_model;
	const C2DImage& _M_reference;
	C2DTransformation& _M_transf; 
	const C2DImageCost& _M_cost; 
	const C2DInterpolatorFactory& _M_ipf; 
}; 
typedef shared_ptr<CGradientProblem> PGradientProblem; 
typedef shared_ptr<CRegProblem> PRegProblem; 


template <typename O> 
struct register_level {
	static void apply(const C2DImage& model, const C2DImage& reference, C2DTransformation& transf, 
			  const C2DImageCost& cost, const C2DInterpolatorFactory& ipf, const O *optimizer)
	{
		if (!cost.has(property_gradient))
			throw invalid_argument("requested optimizer needs gradient, but cost functions doesn't prvide one"); 
		PGradientProblem gp(new CGradientProblem(model, reference, transf, cost, ipf)); 
		CFDFMinimizer minimizer(gp, optimizer );
		
		auto x = transf.get_parameters(); 
		minimizer.run(x); 
		transf.set_parameters(x); 
	}
}; 

template <> 
struct register_level<gsl_multimin_fminimizer_type> {
	static void apply(const C2DImage& model, const C2DImage& reference, C2DTransformation& transf, 
			  const C2DImageCost& cost, const C2DInterpolatorFactory& ipf, 
			  const gsl_multimin_fminimizer_type *optimizer)
	{
		PRegProblem gp(new CRegProblem(model, reference, transf, cost, ipf)); 
		CFMinimizer minimizer(gp, optimizer );
		
		auto x = transf.get_parameters(); 
		minimizer.run(x); 
		transf.set_parameters(x); 
		
	}
}; 

void register_l(const C2DImage& model, const C2DImage& reference, C2DTransformation& transf, 
		const C2DImageCost& cost, const C2DInterpolatorFactory& ipf, EMinimizers minimizer)
{
	if (minimizer_need_gradient(minimizer))
		register_level<gsl_multimin_fdfminimizer_type>::apply(model, reference, transf, cost, ipf, 
								    minimizers[minimizer].fdfmin); 
	
	else
		register_level<gsl_multimin_fminimizer_type>::apply(model, reference, transf, cost, ipf, 
								    minimizers[minimizer].fmin); 
}

int do_main( int argc, const char *argv[] )
{
	string src_filename; 
	string ref_filename; 
	string out_filename; 
	string trans_filename; 
	string transform_type("translate"); 
	EMinimizers minimizer = min_nmsimplex; 

	size_t mg_levels = 3;
	
	CCmdOptionList options;
	options.push_back(make_opt( src_filename, "in", 'i', "test image", "input", true)); 
	options.push_back(make_opt( ref_filename, "ref", 'r', "reference image", "input", true)); 
	options.push_back(make_opt( out_filename, "out", 'o', "registered output image", "output", true)); 
	options.push_back(make_opt( trans_filename, "trans", 't', "transformation", "transformation", false)); 
	options.push_back(make_opt( mg_levels, "levels", 'l', "multigrid levels", "levels", false));
	options.push_back(make_opt( minimizer, TDictMap<EMinimizers>(g_minimizer_table), 
				    "optimizer", 'O', "Optimizer used for minimization", "optimizer", false));
	options.push_back(make_opt( transform_type, "transForm", 'f', "transformation typo", "transform", false));

	options.parse(argc, argv); 
	
	P2DImage Model = load_image2d(src_filename); 
	P2DImage Reference = load_image2d(ref_filename); 
	
	C2DBounds GlobalSize = Model->get_size(); 
	if (GlobalSize != Reference->get_size()){
		throw std::invalid_argument("Images have different size");
	}
			
	unsigned int x_shift = mg_levels;
	unsigned int y_shift = mg_levels;

	auto tr_creator = C2DTransformCreatorHandler::instance().produce(transform_type); 
	auto cost = C2DImageCostPluginHandler::instance().produce("ssd");
	
	unique_ptr<C2DInterpolatorFactory>   ipfactory(create_2dinterpolation_factory(ip_bspline3));

	P2DTransformation transform; 
	
	while (x_shift && y_shift) {
		C2DBounds BlockSize(1 << x_shift, 1 << y_shift);
		cvmsg() << "Blocksize = " << BlockSize.x << "x"<< BlockSize.y << "\n";
		
		stringstream downscale_descr;
		downscale_descr << "downscale:bx=" << BlockSize.x << ",by=" << BlockSize.y; 
		C2DFilterPlugin::ProductPtr downscaler = 
			C2DFilterPluginHandler::instance().produce(downscale_descr.str().c_str()); 
		
		P2DImage ModelScale = downscaler->filter(*Model); 
		P2DImage RefScale   = downscaler->filter(*Reference);	
		
		if (transform) 
			transform = transform->upscale(ModelScale->get_size()); 
		else 
			transform = tr_creator->create(ModelScale->get_size()); 
		
		cvinfo() << ModelScale->get_size() << " vs " << transform->get_size() << "\n"; 
		
		register_l(*ModelScale, *RefScale, *transform, *cost, *ipfactory, minimizer); 

		if (x_shift) 
			x_shift--;  

		if (y_shift) 
			y_shift--;  
	}
	
	transform = transform ? transform->upscale(Model->get_size()): 
		transform = tr_creator->create(Model->get_size()); 
	register_l(*Model, *Reference, *transform, *cost, *ipfactory, minimizer); 

	cvinfo() << Model->get_size() << " vs " << transform->get_size() << "\n"; 
	P2DImage result = (*transform)(*Model, *ipfactory); 

	if (!trans_filename.empty()) {
		cvwarn() << "saving the transformation is not yet implemented";
	}
	
	return save_image2d(out_filename, result); 
	
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

CGradientProblem::CGradientProblem(const C2DImage& model, const C2DImage& reference, C2DTransformation& transf, 
				   const C2DImageCost& cost, const C2DInterpolatorFactory& ipf):
	gsl::CFDFMinimizer::Problem(transf.degrees_of_freedom()),
	_M_model(model), 
	_M_reference(reference), 
	_M_transf(transf), 
	_M_cost(cost), 
	_M_ipf(ipf)
{
	
}

P2DImage CGradientProblem::apply(const DoubleVector& x)
{
	cvinfo() << "x= "; 
	for(size_t i = 0; i < x.size(); ++i) 
		cverb << x[i] << " "; 
	cverb << "\n"; 
	_M_transf.set_parameters(x); 
	return _M_transf(_M_model, _M_ipf); 
}

double  CGradientProblem::do_f(const DoubleVector& x)
{
	P2DImage temp = apply(x); 
	const double value = _M_cost.value(*temp, _M_reference); 
	cvmsg() << "Cost = " << value << "\n"; 
	return value; 
}

void    CGradientProblem::do_df(const DoubleVector& x, DoubleVector&  g)
{
	P2DImage temp = apply(x); 

	C2DFVectorfield gradient(_M_model.get_size()); 
	_M_cost.evaluate_force(*temp, _M_reference, 1.0, gradient); 
	_M_transf.translate(gradient, g); 
}

double  CGradientProblem::do_fdf(const DoubleVector& x, DoubleVector&  g)
{
	P2DImage temp = apply(x); 
	C2DFVectorfield gradient(_M_model.get_size()); 
	_M_cost.evaluate_force(*temp, _M_reference, 1.0, gradient); 
	_M_transf.translate(gradient, g); 
	return _M_cost.value(*temp, _M_reference); 
}


CRegProblem::CRegProblem(const C2DImage& model, const C2DImage& reference, C2DTransformation& transf, 
	    const C2DImageCost& cost, const C2DInterpolatorFactory& ipf):
	gsl::CFMinimizer::Problem(transf.degrees_of_freedom()),
	_M_model(model), 
	_M_reference(reference), 
	_M_transf(transf), 
	_M_cost(cost), 
	_M_ipf(ipf)
{
	
}


double  CRegProblem::do_f(const DoubleVector& x)
{
	_M_transf.set_parameters(x); 
	P2DImage test =  _M_transf(_M_model, _M_ipf);
	const double value = _M_cost.value(*test, _M_reference); 
	cvmsg() << "Cost = " << value << "\n"; 
	return value; 
}

