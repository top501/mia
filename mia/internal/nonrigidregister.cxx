/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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
 * along with this program; if not, write to theFree Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define VSTREAM_DOMAIN "NR-REG"

#include <boost/lambda/lambda.hpp>
#include <iomanip>

NS_MIA_BEGIN

template <typename T> 
struct TNonrigidRegisterImpl {
	typedef dim_traits<T> this_dim_traits;
	typedef typename T::Pointer PTransformation; 
	typedef typename this_dim_traits::Size Size; 
	typedef typename this_dim_traits::Image Image; 
	typedef typename this_dim_traits::PImage PImage; 
	typedef typename this_dim_traits::PTransformationFactory PTransformationFactory; 
	typedef typename this_dim_traits::FullCostList FullCostList; 
	typedef typename this_dim_traits::Filter Filter; 
	typedef typename this_dim_traits::FilterPluginHandler FilterPluginHandler;
	typedef typename this_dim_traits::InterpolatorFactory InterpolatorFactory; 

	TNonrigidRegisterImpl(FullCostList& costs, PMinimizer minimizer,
				PTransformationFactory transform_creator,
				const InterpolatorFactory& ipf,  size_t mg_levels, int idx);

	PTransformation run(PImage src, PImage ref) const;
private:

	FullCostList& m_costs;
	PMinimizer m_minimizer;
	InterpolatorFactory m_ipf;
	PTransformationFactory m_transform_creator;
	size_t m_mg_levels; 
	int m_idx; 
};

template <typename T> 
class TNonrigRegGradientProblem: public CMinimizer::Problem {
public:
	typedef dim_traits<T> this_dim_traits;
	typedef typename this_dim_traits::Size Size; 
	typedef typename this_dim_traits::Image Image; 
	typedef typename this_dim_traits::PImage PImage; 
	typedef typename this_dim_traits::PTransformationFactory PTransformationFactory; 
	typedef typename this_dim_traits::FullCostList FullCostList; 
	typedef typename this_dim_traits::Filter Filter; 
	typedef typename this_dim_traits::FilterPluginHandler FilterPluginHandler;
	typedef typename this_dim_traits::InterpolatorFactory InterpolatorFactory; 


	TNonrigRegGradientProblem(const FullCostList& costs, T& transf,
			      const InterpolatorFactory& m_ipf);

	void reset_counters(); 
	
	typedef shared_ptr<TNonrigRegGradientProblem<T> > PNonrigRegGradientProblem; 
private:
	double  do_f(const CDoubleVector& x);
	void    do_df(const CDoubleVector& x, CDoubleVector&  g);
	double  do_fdf(const CDoubleVector& x, CDoubleVector&  g);

	bool do_has(const char *property) const; 

	size_t do_size() const; 

	const FullCostList& m_costs; 
	T& m_transf;
	const InterpolatorFactory& m_ipf;
	size_t m_func_evals; 
	size_t m_grad_evals; 
	double m_start_cost; 
};

template <typename T> 
TNonrigidRegister<T>::TNonrigidRegister(FullCostList& costs, PMinimizer minimizer,
					 PTransformationFactory transform_creation,
					 const InterpolatorFactory& ipf, size_t mg_levels, int idx):
	impl(new TNonrigidRegisterImpl<T>( costs, minimizer, transform_creation, ipf, mg_levels, idx))
{
}

template <typename T> 
TNonrigidRegister<T>::~TNonrigidRegister()
{
	delete impl;
}

template <typename T> 
typename TNonrigidRegister<T>::PTransformation 
TNonrigidRegister<T>::run(PImage src, PImage ref) const
{
	return impl->run(src, ref);
}


template <typename T> 
TNonrigidRegisterImpl<T>::TNonrigidRegisterImpl(FullCostList& costs, PMinimizer minimizer,
						 PTransformationFactory transform_creation, 
						const InterpolatorFactory& ipf,size_t mg_levels, int idx):
	m_costs(costs),
	m_minimizer(minimizer),
	m_ipf(ipf),
	m_transform_creator(transform_creation), 
	m_mg_levels(mg_levels), 
	m_idx(idx)
{
}


/*
  This filter could be replaced by a histogram equalizing filter 
  or it should be moved outside the registration function 
  and considered what it is, a pre-processing step
*/

template <typename T> 
class FScaleFilterCreator: public TFilter<typename TNonrigidRegisterImpl<T>::FilterPluginHandler::ProductPtr> {
	typedef typename TNonrigidRegisterImpl<T>::FilterPluginHandler FilterPluginHandler;
public: 
	typedef typename TFilter<typename TNonrigidRegisterImpl<T>::FilterPluginHandler::ProductPtr>::result_type result_type; 
	template <typename V, typename S>
	result_type operator ()(const V& a, const S& b) const {
		double sum = 0.0; 
		double sum2 = 0.0; 
		int n = 2 * a.size(); 

		auto ia = a.begin(); 
		
		for(auto ib = b.begin(); ia != a.end(); ++ia, ++ib) {
			sum += *ia + *ib; 
			sum2 += *ia * *ia + *ib * *ib;
		}
		
		double mean = sum / n; 
		double sigma = sqrt((sum2 - sum * sum / n) / (n - 1));

		// both images are of the same single color 
		if (sigma == 0.0) 
			return result_type(); 

		// I want a conversion filter, that makes the images together zero mean 
		// and diversion 1
		stringstream filter_descr; 
		filter_descr << "convert:repn=float,map=linear,b=" << -mean/sigma << ",a=" << 1.0/sigma; 
		cvinfo() << "Will convert using the filter:" << filter_descr.str() << "\n"; 
		
		return FilterPluginHandler::instance().produce(filter_descr.str()); 
		
	}; 
	
}; 

template <typename T> 
typename TNonrigidRegisterImpl<T>::PTransformation 
TNonrigidRegisterImpl<T>::run(PImage src, PImage ref) const
{
	assert(src);
	assert(ref);
	assert(src->get_size() == ref->get_size());

	PTransformation transform;

	// convert the images to float ans scale to range [-1,1]
	// this should be replaced by some kind of general pre-filter plug-in 
	FScaleFilterCreator<T> fc; 
	auto tofloat_converter = ::mia::filter(fc, *src, *ref); 
	
	if (tofloat_converter) {
		src = tofloat_converter->filter(*src); 
		ref = tofloat_converter->filter(*ref); 
	}
	else // both images have only one value, and are, therefore, already registered
		return m_transform_creator->create(src->get_size());


	Size global_size = src->get_size();

	int shift = m_mg_levels;

	std::string src_name("src.@"); 
	std::string ref_name("src.@"); 

	if (m_idx >= 0) {
		std::stringstream src_ss; 
		std::stringstream ref_ss; 

		src_ss << "src" << m_idx << ".@"; 
		ref_ss << "ref" << m_idx << ".@"; 
		src_name = src_ss.str(); 
		ref_name = ref_ss.str(); 
	}

	do {
		shift--;

		Size BlockSize; 
                BlockSize.fill(1 << shift);
		cvinfo() << "Blocksize = " << BlockSize  << "\n";

		stringstream downscale_descr;
		downscale_descr << "downscale:b=[" << BlockSize<<"]";
		auto downscaler = FilterPluginHandler::instance().produce(downscale_descr.str().c_str());

		PImage src_scaled = shift ? downscaler->filter(*src) : src;
		PImage ref_scaled = shift ? downscaler->filter(*ref) : ref;

		if (transform)
			transform = transform->upscale(src_scaled->get_size());
		else
			transform = m_transform_creator->create(src_scaled->get_size());

		cvinfo() << "register at " << src_scaled->get_size() << "\n";
		/**
		   This code is somewhat ugly, it stored the images in the internal buffer 
		   and then it forces the cost function to reload the images
		   However, currently the downscaling does not support a specific target size
		 */
		save_image(src_name, src_scaled);
		save_image(ref_name, ref_scaled);
		m_costs.reinit(); 
		
		// currently this call does nothing, however it should replace the three lines above 
		// and the cost function should handle the image scaling 

		m_costs.set_size(src_scaled->get_size()); 
		
		std::shared_ptr<TNonrigRegGradientProblem<T> > 
			gp(new TNonrigRegGradientProblem<T>( m_costs, *transform, m_ipf));
		
		m_minimizer->set_problem(gp);

		auto x = transform->get_parameters();
		cvinfo() << "Start Registration of " << x.size() <<  " parameters\n"; 
		m_minimizer->run(x);
		transform->set_parameters(x);
		
		// run the registration at refined splines 
		if (transform->refine()) {
			m_minimizer->set_problem(gp);
			x = transform->get_parameters();
			cvinfo() << "Start Registration of " << x.size() <<  " parameters\n"; 
			m_minimizer->run(x);
			transform->set_parameters(x);
		}

	} while (shift); 
	return transform;
}

template <typename T> 
TNonrigRegGradientProblem<T>::TNonrigRegGradientProblem(const FullCostList& costs, 
						       T& transf, const InterpolatorFactory& ipf):
	m_costs(costs),
	m_transf(transf),
	m_ipf(ipf), 
	m_func_evals(0),
	m_grad_evals(0), 
	m_start_cost(0.0)
{

}

template <typename T> 
void TNonrigRegGradientProblem<T>::reset_counters()
{
	m_func_evals = m_grad_evals = 0; 
}

template <typename T> 
double  TNonrigRegGradientProblem<T>::do_f(const CDoubleVector& x)
{
       

	m_transf.set_parameters(x);
	double result = m_costs.cost_value(m_transf);
	if (!m_func_evals && !m_grad_evals) 
		m_start_cost = result; 
	
	m_func_evals++; 
	cvinfo() << "Cost[fg="<<setw(4)<<m_grad_evals 
		<< ",fe="<<setw(4)<<m_func_evals<<"]=" 
		<< setw(20) << setprecision(12) << result 
		<< "ratio:" << setw(20) << setprecision(12) 
		<< result / m_start_cost <<   "\n"; 
	return result; 
}

template <typename T> 
void    TNonrigRegGradientProblem<T>::do_df(const CDoubleVector& x, CDoubleVector&  g)
{
	do_fdf(x,g); 
}

template <typename T> 
double  TNonrigRegGradientProblem<T>::do_fdf(const CDoubleVector& x, CDoubleVector&  g)
{

	m_transf.set_parameters(x);
	fill(g.begin(), g.end(), 0.0); 
	double result = m_costs.evaluate(m_transf, g);

	if (!m_func_evals && !m_grad_evals) 
		m_start_cost = result; 

	m_grad_evals++; 

	cvinfo() << "Cost[fg="<<setw(4)<<m_grad_evals 
		<< ",fe="<<setw(4)<<m_func_evals<<"]= with " 
		<< x.size() << " parameters= " 
		<< setw(20) << setprecision(12) << result 
		<< " ratio:" << setw(20) << setprecision(12) << result / m_start_cost <<  "\n"; 
	return result; 
}

template <typename T> 
bool TNonrigRegGradientProblem<T>::do_has(const char *property) const
{
	return m_costs.has(property); 
}

template <typename T> 
size_t TNonrigRegGradientProblem<T>::do_size() const
{
	return m_transf.degrees_of_freedom(); 
}

NS_MIA_END
