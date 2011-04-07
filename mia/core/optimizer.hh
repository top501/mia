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


#ifndef mia_core_optimizer_hh
#define mia_core_optimizer_hh

#include <mia/core/problem.hh>
#include <mia/core/factory.hh>
#include <mia/core/handler.hh>

NS_MIA_BEGIN

/**
   Base class for all optimizers
*/

struct EXPORT_CORE algorithm_type {
	static const char *value;
};



class EXPORT_CORE COptimizer : public CProductBase {
public:
	typedef COptimizer plugin_data; 
	typedef algorithm_type plugin_type; 
	static const char *type_descr;

	enum EOptimizerResults { or_failed=0,
				 or_converged,
				 or_residum_low,
				 or_gradient_low,
				 or_step_low,
				 or_max_iter,
				 or_keep_running };

	virtual ~COptimizer();
	EOptimizerResults run(CProblem& problem);
	const char *get_name() const;

private:

	/**
	   This one needs to be implemented to obtaine
	*/
	virtual EOptimizerResults do_run(CProblem& problem) const = 0;
	virtual const char *do_get_name() const = 0;
};

typedef TFactory<COptimizer> COptimizerPlugin;
typedef THandlerSingleton<TFactoryPluginHandler<COptimizerPlugin> > COptimizerPluginPluginHandler;


NS_MIA_END

#endif
