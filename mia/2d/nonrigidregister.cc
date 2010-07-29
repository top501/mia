/* -*- mona-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2010
 *
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

#define VSTREAM_DOMAIN "rigidreg"

#include <mia/2d/nonrigidregister.hh>
#include <mia/2d/2dfilter.hh>
#include <mia/2d/2dimageio.hh>
#include <mia/2d/transformfactory.hh>
#include <gsl++/multimin.hh>



NS_MIA_BEGIN

using namespace gsl;
using namespace std;

struct C2DNonrigidRegisterImpl {

	C2DNonrigidRegisterImpl(C2DFullCostList& costs, EMinimizers minimizer,
				P2DTransformationFactory transform_creator,
				const C2DInterpolatorFactory& ipf);

	P2DTransformation run(P2DImage src, P2DImage ref,  size_t mg_levels) const;
private:

	void apply(C2DTransformation& transf, const gsl_multimin_fdfminimizer_type *optimizer)const ;


	void apply(C2DTransformation& transf, const gsl_multimin_fminimizer_type *optimizer)const ;


	C2DFullCostList& _M_costs;
	EMinimizers _M_minimizer;
	C2DInterpolatorFactory _M_ipf;
	P2DTransformationFactory _M_transform_creator;
};

class C2DNonrigRegGradientProblem: public gsl::CFDFMinimizer::Problem {
public:
	C2DNonrigRegGradientProblem(const C2DFullCostList& costs, C2DTransformation& transf,
			      const C2DInterpolatorFactory& _M_ipf);
private:
	double  do_f(const DoubleVector& x);
	void    do_df(const DoubleVector& x, DoubleVector&  g);
	double  do_fdf(const DoubleVector& x, DoubleVector&  g);

	P2DImage apply(const DoubleVector& x);
	const C2DFullCostList& _M_costs; 
	C2DTransformation& _M_transf;
	const C2DInterpolatorFactory& _M_ipf;
};
typedef shared_ptr<C2DNonrigRegGradientProblem> P2DGradientNonrigregProblem;

C2DNonrigidRegister::C2DNonrigidRegister(C2DFullCostList& costs, EMinimizers minimizer,
					 P2DTransformationFactory transform_creation,
					 const C2DInterpolatorFactory& ipf):
	impl(new C2DNonrigidRegisterImpl( costs, minimizer, transform_creation, ipf))
{
}


C2DNonrigidRegister::~C2DNonrigidRegister()
{
	delete impl;
}

P2DTransformation C2DNonrigidRegister::run(P2DImage src, P2DImage ref,  size_t mg_levels) const
{
	return impl->run(src, ref, mg_levels);
}

C2DNonrigidRegisterImpl::C2DNonrigidRegisterImpl(C2DFullCostList& costs, EMinimizers minimizer,
						 P2DTransformationFactory transform_creation, 
						 const C2DInterpolatorFactory& ipf):
	_M_costs(costs),
	_M_minimizer(minimizer),
	_M_ipf(ipf),
	_M_transform_creator(transform_creation)
{
}

static bool minimizer_need_gradient(EMinimizers m)
{
	switch (m) {
	case min_undefined: throw invalid_argument("Try to use minimizer 'undefined'");
	default: return true;
	}
}

struct  UMinimzer{
		const gsl_multimin_fminimizer_type *fmin;
		const gsl_multimin_fdfminimizer_type *fdfmin;
};

UMinimzer gradminimizers[min_undefined] = {
	{ NULL, gsl_multimin_fdfminimizer_conjugate_fr },
	{ NULL, gsl_multimin_fdfminimizer_conjugate_pr },
	{ NULL, gsl_multimin_fdfminimizer_vector_bfgs },
	{ NULL, gsl_multimin_fdfminimizer_vector_bfgs2 },
	{ NULL, gsl_multimin_fdfminimizer_steepest_descent }
};


void C2DNonrigidRegisterImpl::apply(C2DTransformation& transf, const gsl_multimin_fdfminimizer_type *optimizer)const
{
	if (!_M_costs.has(property_gradient))
		throw invalid_argument("requested optimizer needs gradient, but cost functions doesn't prvide one");
	P2DGradientNonrigregProblem gp(new C2DNonrigRegGradientProblem( _M_costs, transf, _M_ipf));
	CFDFMinimizer minimizer(gp, optimizer );

	auto x = transf.get_parameters();
	minimizer.run(x);
	transf.set_parameters(x);
}



P2DTransformation C2DNonrigidRegisterImpl::run(P2DImage src, P2DImage ref,  size_t mg_levels) const
{
	assert(src);
	assert(ref);
	assert(src->get_size() == ref->get_size());

	if (!minimizer_need_gradient(_M_minimizer))
		throw invalid_argument("Non-gradient based optimization not supported\n"); 

	P2DTransformation transform;

	C2DBounds global_size = src->get_size();

	int x_shift = mg_levels + 1;
	int y_shift = mg_levels + 1;

	while (x_shift && y_shift) {
		if (x_shift)
			x_shift--;

		if (y_shift)
			y_shift--;

		C2DBounds BlockSize(1 << x_shift, 1 << y_shift);
		cvinfo() << "Blocksize = " << BlockSize.x << "x"<< BlockSize.y << "\n";

		stringstream downscale_descr;
		downscale_descr << "downscale:bx=" << BlockSize.x << ",by=" << BlockSize.y;
		C2DFilterPlugin::ProductPtr downscaler =
			C2DFilterPluginHandler::instance().produce(downscale_descr.str().c_str());

		P2DImage src_scaled = x_shift && y_shift ? downscaler->filter(*src) : src;
		P2DImage ref_scaled = x_shift && y_shift ? downscaler->filter(*ref) : ref;

		if (transform)
			transform = transform->upscale(src_scaled->get_size());
		else
			transform = _M_transform_creator->create(src_scaled->get_size());

		cvmsg() << "register at " << src_scaled->get_size() << "\n";

		save_image2d("src.@", src_scaled);
		save_image2d("ref.@", ref_scaled);
		_M_costs.set_size(src_scaled->get_size()); 
		
		apply(*transform, gradminimizers[_M_minimizer].fdfmin);

		auto params = transform->get_parameters(); 
	}
	return transform;
}

C2DNonrigRegGradientProblem::C2DNonrigRegGradientProblem(const C2DFullCostList& costs, C2DTransformation& transf, 
					     const C2DInterpolatorFactory& ipf):
	gsl::CFDFMinimizer::Problem(transf.degrees_of_freedom()),
	_M_costs(costs),
	_M_transf(transf),
	_M_ipf(ipf)
{

}

double  C2DNonrigRegGradientProblem::do_f(const DoubleVector& x)
{
	cvwarn() << "C2DNonrigRegGradientProblem::do_f implemented by means of C2DNonrigRegGradientProblem::do_fdf\n"; 
	DoubleVector  g(x.size()); 
	return do_fdf(x, g);
}

void    C2DNonrigRegGradientProblem::do_df(const DoubleVector& x, DoubleVector&  g)
{
	_M_transf.set_parameters(x);
	_M_costs.evaluate(_M_transf, g);
}

double  C2DNonrigRegGradientProblem::do_fdf(const DoubleVector& x, DoubleVector&  g)
{
	_M_transf.set_parameters(x);
	return _M_costs.evaluate(_M_transf, g);
}

NS_MIA_END
