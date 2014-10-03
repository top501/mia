/* -*- mia-c++  -*-
 *
 * This file is part of MIA - a toolbox for medical image analysis 
 * Copyright (c) Leipzig, Madrid 1999-2014 Gert Wollny
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

#include <mia/core/fastica_nonlinearity.hh>
#include <gsl++/matrix_vector_ops.hh>
#include <mia/core/export_handler.hh>
#include <mia/core/plugin_base.cxx>
#include <mia/core/handler.cxx>


namespace mia {

using namespace gsl; 

CFastICANonlinearityBase::CFastICANonlinearityBase():m_mu(1.0)
{
}

void CFastICANonlinearityBase::set_signal(const Matrix *signal)
{
	m_signal = signal; 
	assert(m_signal); 
	m_sample_scale = 1.0 / m_signal->cols(); 
	
	post_set_signal(); 
}

void CFastICANonlinearityBase::set_mu(double mu)
{
	m_mu = mu; 
}


const Matrix& CFastICANonlinearityBase::get_signal() const
{
	assert(m_signal); 
	return *m_signal; 
}

void CFastICADeflNonlinearity::post_set_signal()
{
	m_workspace = DoubleVector(get_signal().rows(), false); 
	m_XTw = DoubleVector(get_signal().cols(), false); 
}

const char *CFastICANonlinearityBase::data_descr = "fastica"; 
const char *CFastICADeflNonlinearity::type_descr = "deflation"; 
const char *CFastICASymmNonlinearity::type_descr = "symmetric"; 


void CFastICADeflNonlinearity::apply(gsl::DoubleVector& w)
{
	multiply_v_m(m_XTw, w, get_signal());
	if (get_mu() >= 1.0) 
		do_apply(w);  
	else 
		do_apply_stabelized(w); 
}

void CFastICASymmNonlinearity::post_set_signal()
{
        // create helper matrix 
}

template<>  const char * const 
TPluginHandler<TFactory<CFastICADeflNonlinearity>>::m_help = 
	"These plug-ins provide various non-linearity models for the FastICA algorithm "
        "that uses deflation.";

template<>  const char * const 
TPluginHandler<TFactory<CFastICASymmNonlinearity>>::m_help = 
	"These plug-ins provide various non-linearity models for the FastICA algorithm "
        "that uses symetric estimation.";

EXPLICIT_INSTANCE_HANDLER(CFastICADeflNonlinearity); 
EXPLICIT_INSTANCE_HANDLER(CFastICASymmNonlinearity); 


}// end namespace 
