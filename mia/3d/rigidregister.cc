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

#include <mia/3d/rigidregister.hh>
#include <mia/3d/3dfilter.hh>
#include <mia/3d/transformfactory.hh>
#include <mia/core/filter.hh>

NS_MIA_BEGIN

using namespace std;

struct C3DRigidRegisterImpl {

	C3DRigidRegisterImpl(P3DImageCost cost, PMinimizer minimizer,
			     P3DTransformationFactory transform_creator,
			     const C3DInterpolatorFactory& ipf,  size_t mg_levels);

	P3DTransformation run(P3DImage src, P3DImage ref) const;
private:


	P3DImageCost _M_cost;
	PMinimizer _M_minimizer;
	C3DInterpolatorFactory _M_ipf;
	P3DTransformationFactory _M_transform_creator; 
	size_t _M_mg_levels; 
};

class C3DRegGradientProblem: public CMinimizer::Problem {
public:
	C3DRegGradientProblem(const C3DImage& model, const C3DImage& reference, C3DTransformation& transf,
			      const C3DImageCost& _M_cost, const C3DInterpolatorFactory& _M_ipf);
private:
	void    do_df(const CDoubleVector& x, CDoubleVector&  g);
	double  do_fdf(const CDoubleVector& x, CDoubleVector&  g);
protected: 
	double  do_f(const CDoubleVector& x);
	P3DImage apply(const CDoubleVector& x);
	const C3DImage& _M_model;
	const C3DImage& _M_reference;
	C3DTransformation& _M_transf;
	const C3DImageCost& _M_cost;
	const C3DInterpolatorFactory& _M_ipf;
	
	size_t _M_geval;
	size_t _M_feval;

};
typedef shared_ptr<C3DRegGradientProblem> P3DGradientProblem;


class C3DRegFakeGradientProblem: public C3DRegGradientProblem {
public:
	C3DRegFakeGradientProblem(const C3DImage& model, const C3DImage& reference, C3DTransformation& transf,
				  const C3DImageCost& _M_cost, const C3DInterpolatorFactory& _M_ipf);
private:
	void    do_df(const CDoubleVector& x, CDoubleVector&  g);
	double  do_fdf(const CDoubleVector& x, CDoubleVector&  g);

};

class C3DRegProblem: public CMinimizer::Problem {
public:
	C3DRegProblem(const C3DImage& model, const C3DImage& reference, C3DTransformation& transf,
			 const C3DImageCost& _M_cost, const C3DInterpolatorFactory& _M_ipf);
private:
	double  do_f(const CDoubleVector& x);
	void    do_df(const CDoubleVector& x, CDoubleVector&  g);
	double  do_fdf(const CDoubleVector& x, CDoubleVector&  g);

	const C3DImage& _M_model;
	const C3DImage& _M_reference;
	C3DTransformation& _M_transf;
	const C3DImageCost& _M_cost;
	const C3DInterpolatorFactory& _M_ipf;
};
typedef shared_ptr<C3DRegProblem> P3DRegProblem;


C3DRigidRegister::C3DRigidRegister(P3DImageCost cost, PMinimizer minimizer,
				   P3DTransformationFactory transform_creator,
				   const C3DInterpolatorFactory& ipf, size_t mg_levels):
	impl(new C3DRigidRegisterImpl( cost, minimizer, transform_creator, ipf, mg_levels))
{
}


C3DRigidRegister::~C3DRigidRegister()
{
	delete impl;
}

P3DTransformation C3DRigidRegister::run(P3DImage src, P3DImage ref) const
{
	return impl->run(src, ref);
}

C3DRigidRegisterImpl::C3DRigidRegisterImpl(P3DImageCost cost, PMinimizer minimizer,
					   P3DTransformationFactory transform_creator,
					   const C3DInterpolatorFactory& ipf,  
					   size_t mg_levels):
	_M_cost(cost),
	_M_minimizer(minimizer),
	_M_ipf(ipf),
	_M_transform_creator(transform_creator), 
	_M_mg_levels(mg_levels)
{
}

P3DTransformation C3DRigidRegisterImpl::run(P3DImage src, P3DImage ref) const
{
	assert(src);
	assert(ref);
	assert(src->get_size() == ref->get_size());

	P3DTransformation transform;

	C3DBounds global_size = src->get_size();

	int x_shift = _M_mg_levels + 1;
	int y_shift = _M_mg_levels + 1;
	int z_shift = _M_mg_levels + 1;

	while (x_shift && y_shift && z_shift) {
		if (x_shift)
			x_shift--;

		if (y_shift)
			y_shift--;

		if (z_shift)
			z_shift--;

		C3DBounds BlockSize(1 << x_shift, 1 << y_shift, 1 << z_shift);
		cvinfo() << "Blocksize = " << BlockSize << "\n";

		stringstream downscale_descr;
		downscale_descr << "downscale:bx=" << BlockSize.x 
				<< ",by=" << BlockSize.y
				<< ",bz=" << BlockSize.z;
		
		auto downscaler = C3DFilterPluginHandler::instance().produce(downscale_descr.str().c_str());

		auto src_scaled = x_shift && y_shift ? downscaler->filter(*src) : src;
		auto ref_scaled = x_shift && y_shift ? downscaler->filter(*ref) : ref;

		if (transform)
			transform = transform->upscale(src_scaled->get_size());
		else
			transform = _M_transform_creator->create(src_scaled->get_size());

		cvmsg() << "register at " << src_scaled->get_size() << "\n";


		CMinimizer::PProblem gp = _M_minimizer->has(property_gradient)? 
			CMinimizer::PProblem(new C3DRegFakeGradientProblem(*src_scaled, *ref_scaled,
									   *transform, *_M_cost, _M_ipf)):
			CMinimizer::PProblem(new C3DRegProblem(*src_scaled, *ref_scaled, *transform, 
							       *_M_cost, _M_ipf)); 
		
		_M_minimizer->set_problem(gp); 

		auto x = transform->get_parameters();
		_M_minimizer->run(x);
		transform->set_parameters(x);

		auto params = transform->get_parameters(); 
		cvinfo() << "\nParams:";
		for (auto i = params.begin(); i != params.end(); ++i) 
			cverb << *i << " "; 
		cverb << "\n"; 
	}
	return transform;
}

C3DRegGradientProblem::C3DRegGradientProblem(const C3DImage& model, const C3DImage& reference, 
					     C3DTransformation& transf,
					     const C3DImageCost& cost, const C3DInterpolatorFactory& ipf):
	CMinimizer::Problem(transf.degrees_of_freedom()),
	_M_model(model),
	_M_reference(reference),
	_M_transf(transf),
	_M_cost(cost),
	_M_ipf(ipf),
	_M_geval(0),
	_M_feval(0)

{
	add(property_gradient); 
}

P3DImage C3DRegGradientProblem::apply(const CDoubleVector& x)
{
	cvinfo() << "\nParams:";
	for (auto i = x.begin(); i != x.end(); ++i) 
		cverb << *i << " "; 
	cverb << "\n"; 

	_M_transf.set_parameters(x);
	return _M_transf(_M_model, _M_ipf);
}

double  C3DRegGradientProblem::do_f(const CDoubleVector& x)
{
	++_M_feval; 
	P3DImage temp = apply(x);
	const double value = _M_cost.value(*temp, _M_reference);
	cvmsg() << "Cost(f="<<_M_feval<<",g="<< _M_geval <<") = " << value << "\n";
	return value;
}

void    C3DRegGradientProblem::do_df(const CDoubleVector& x, CDoubleVector&  g)
{
	++_M_geval; 

	P3DImage temp = apply(x);

	C3DFVectorfield gradient(_M_model.get_size());
	_M_cost.evaluate_force(*temp, _M_reference, 1.0, gradient);
	_M_transf.translate(gradient, g);
}

double  C3DRegGradientProblem::do_fdf(const CDoubleVector& x, CDoubleVector&  g)
{
	++_M_geval; 
	++_M_feval;
	P3DImage temp = apply(x);
	C3DFVectorfield gradient(_M_model.get_size());
	_M_cost.evaluate_force(*temp, _M_reference, 1.0, gradient);
	
	_M_transf.translate(gradient, g);

	const double value = _M_cost.value(*temp, _M_reference);
	cvmsg() << "Cost(f="<<_M_feval<<",g="<< _M_geval <<") = " << value << "\n";
	return value;
}

C3DRegFakeGradientProblem::C3DRegFakeGradientProblem(const C3DImage& model, const C3DImage& reference, 
						     C3DTransformation& transf, const C3DImageCost& _M_cost, 
						     const C3DInterpolatorFactory& _M_ipf):
	C3DRegGradientProblem(model, reference, transf,  _M_cost,  _M_ipf)
{
}

void    C3DRegFakeGradientProblem::do_df(const CDoubleVector& x, CDoubleVector&  g)
{
	CDoubleVector x_tmp(x.size()); 
	copy(x.begin(), x.end(), x_tmp.begin()); 
	for (size_t i = 0; i < g.size(); ++i) {
		x_tmp[i] += 0.01; 
		double cost_p = do_f(x_tmp); 
		x_tmp[i] -= 0.02; 
		double cost_m = do_f(x_tmp); 
		x_tmp[i] += 0.01;
		g[i] = (cost_p - cost_m) * 50.0; 
		cvinfo() << "g[" << i << "] = " << g[i] << "\n"; 
	}
}

double  C3DRegFakeGradientProblem::do_fdf(const CDoubleVector& x, CDoubleVector&  g)
{
	double cost_value = do_f(x); 
	do_df(x,g); 
	return cost_value; 
}

C3DRegProblem::C3DRegProblem(const C3DImage& model, const C3DImage& reference, C3DTransformation& transf,
	    const C3DImageCost& cost, const C3DInterpolatorFactory& ipf):
	CMinimizer::Problem(transf.degrees_of_freedom()),
	_M_model(model),
	_M_reference(reference),
	_M_transf(transf),
	_M_cost(cost),
	_M_ipf(ipf)
{
}

double  C3DRegProblem::do_f(const CDoubleVector& x)
{
	cvinfo() << "\nParams:";
	for (auto i = x.begin(); i != x.end(); ++i) 
		cverb << *i << " "; 
	cverb << "\n"; 

	_M_transf.set_parameters(x);
	P3DImage test =  _M_transf(_M_model, _M_ipf);

	const double value = _M_cost.value(*test, _M_reference);
	cvmsg() << "Cost = " << value << "\n";
	return value;
}

void   C3DRegProblem::do_df(const CDoubleVector& , CDoubleVector&  )
{
	assert(0 && "C2DRegProblem::do_df should not be called from a gradient free minimizer"); 
}

double  C3DRegProblem::do_fdf(const CDoubleVector& , CDoubleVector&  )
{
	assert(0 && "C2DRegProblem::do_fdf should not be called from a gradient free minimizer"); 
}

NS_MIA_END
