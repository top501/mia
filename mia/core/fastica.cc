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

#include <mia/core/gsl_matrix_vector_ops.hh>
#include <mia/core/gsl_pca.hh>
#include <mia/core/fastica.hh>
#include <algorithm>
#include <random>
#include <cmath>
#include <stdexcept>
#include <gsl/gsl_blas.h>
#include <mia/core/fastica_nonlinearity.hh>

NS_MIA_BEGIN 

using gsl::Vector; 
using gsl::Matrix; 
using gsl::CSymmvEvalEvec; 

using std::sort; 
using std::transform; 
using std::vector; 

FastICA::FastICA(int num_ic):
        m_approach(appr_defl), 
        m_numOfIC(num_ic), 
        m_finetune(true), 
        m_mu(1.0), 
        m_epsilon(1e-10), 
        m_sampleSize(1.0), 
        m_stabilization(true), 
        m_maxNumIterations(1000), 
        m_maxFineTune(200), 
        m_PCAonly(false), 
	m_component_energy_limit(0.9), 
	m_with_initial_guess(false), 
	m_do_saddle_check(false), 
	m_saddle_postiter(100), 
	m_separating_matrix(1,1,false)
{
        m_nonlinearity = produce_fastica_nonlinearity("pow3");  
}

struct CCenteredSignal  {
	CCenteredSignal(const Matrix& mixedSig); 
	
	Matrix signal; 
	Vector mean; 
}; 


CCenteredSignal::CCenteredSignal(const Matrix& mixedSig):
	signal(mixedSig.rows(), mixedSig.cols(), false), 
	mean(mixedSig.rows(), true)
{
	for (size_t r = 0; r < mixedSig.rows(); ++r) {
		for (size_t c = 0; c < mixedSig.cols(); ++c)
			mean[r] += mixedSig(r,c); 
		mean[r] /= mixedSig.cols(); 
		for (size_t c = 0; c < mixedSig.cols(); ++c)
			signal.set(r,c, mixedSig(r,c) - mean[r]); 
	}
}

void FastICA::evaluate_whiten_matrix(const Matrix& evec, const Vector& eval)
{
	m_whitening_matrix.reset(evec.cols(), evec.rows(), false); 
	m_dewhitening_matrix.reset(evec.rows(), evec.cols(), false);
	
	for (unsigned  i = 0; i < eval.size(); i++) {
		double iwscale = sqrt(eval[i]);
		double wscale = 1.0 / iwscale;
		
		auto wmr = m_whitening_matrix.get_row(i); 
		auto inv = evec.get_column(i);
		auto dwmc = m_dewhitening_matrix.get_column(i); 
		
		transform(inv.begin(), inv.end(), wmr.begin(), 
			  [wscale](double x) {return wscale * x;}); 
		
		transform(inv.begin(), inv.end(), dwmc.begin(), 
			  [iwscale](double x) {return iwscale * x;}); 
	}
	
	cvdebug() << "Whitening: = " << m_whitening_matrix << "\n"; 
	cvdebug() << "DeWhitening: = " << m_dewhitening_matrix << "\n"; 

}

bool FastICA::separate(const gsl::Matrix&  mix)
{
	CCenteredSignal centered(mix);
	cvdebug() << "separate signal of size " << centered.signal.rows() << "x" << centered.signal.cols() << "\n"; 

	// currently not used 
	Matrix guess;
	if (m_initGuess)
		guess = m_initGuess;


	// run PCA to prepare the whitening 
	// also select the limit of useful compinents based on maximal 
	// energy 
	gsl::PCA pca(m_numOfIC, m_component_energy_limit);
	auto pca_result = pca.analyze(centered.signal); 
	
	if (pca_result.eval.size() < 1) {
		m_independent_components = mix;
		return false;
	}

	// update the number of components 
	m_numOfIC = pca_result.eval.size(); 

	cvdebug() << "Considering " << m_numOfIC << " independend components\n"; 
	cvdebug() << "PCA: eval= " << pca_result.eval << "\n"; 
	cvdebug() << "PCA: evec= " << pca_result.evec << "\n"; 

	evaluate_whiten_matrix(pca_result.evec, pca_result.eval);

	// if only PCA, stop here and save the dewithening as independend components 
	// where is the mixing matrix? 
	if (m_PCAonly) {
		m_mixing_matrix.reset(mix.rows(), m_dewhitening_matrix.cols(), 1.0);
		m_independent_components = m_dewhitening_matrix; 
		return true; 
	}

	// run the actual ICA 
	
	auto whitened_signal = m_whitening_matrix * centered.signal; 

	bool result = false; 
	Matrix B( whitened_signal.rows(), m_numOfIC,  true); 	
	
	switch (m_approach) {
	case appr_defl: result = fpica_defl(whitened_signal, B);break; 
	case appr_symm: result = fpica_symm(whitened_signal, B); break; 
	default: 
		throw std::invalid_argument("FastICA::separate: unknown apporach given"); 
		
	}

	// evaluate the results if the ICA 
	m_mixing_matrix = m_dewhitening_matrix * B; 
		
	cvdebug() << "Mixing matrix= " << m_mixing_matrix << "\n"; 

	multiply_mT_m(m_separating_matrix, B, m_whitening_matrix); 
	
	m_independent_components = m_separating_matrix * mix; 

	cvdebug() << "ICs= " << m_independent_components << "\n"; 

	return result; 
}

bool FastICA::fpica_defl_round(int component, Vector& w, Matrix& B)
{
	Vector w_old(w);
	Vector w_old2(w);

	double mu = m_mu;
	double delta = m_epsilon + 1.0;

	bool is_finetuning = false;  
	bool converged = false; 
	bool loong = false; 

	double stroke = 0.0; 

	int iter = 0;
	int maxiter = m_maxNumIterations; 
	
	while (!converged && iter < maxiter) {
			
		cvdebug() << "Defl: c=" << component
			  << ", iter= " << iter 
			  << ", delta= " << delta
			  << "\n"; 
		
		m_nonlinearity->set_mu(mu); 
		m_nonlinearity->apply(w); 
		
		Vector w_save(w); 
		for (int j = 0; j < component; ++j) {
			const double wdot = B.dot_column(j, w_save); 
                        auto wj = B.get_column(j);
			cblas_daxpy(wj->size, -wdot, wj->data, wj->stride, w->data, w->stride);
		}
		
		double norm = sqrt(dot(w, w));
		if (norm > 0.0) 
			gsl_vector_scale(w, 1.0/norm); 

		Vector w_help = w_old; 
		gsl_vector_sub(w_help, w); 
		
		cvdebug() << "w-help=" << w_help << "\n"; 
		delta = sqrt(dot(w_help, w_help));

		cvmsg() << "DEFL["<<iter<<"]: delta = " << delta << "\n"; 

		if (delta < m_epsilon) {
			if (m_finetune && !is_finetuning) {
				cvinfo() << "DEFL: start fine tuning\n"; 
				is_finetuning = true; 
				maxiter += m_maxFineTune; 
				mu = 0.01 * m_mu; 
			}else{
				converged = true; 
			}
		} else if (m_stabilization) {
			gsl_vector_sub(w_old2, w);
			double delta2 = sqrt(dot(w_old2, w_old2));
			if ( (stroke == 0.0) && (delta2 < m_epsilon)) {
				stroke = mu; 
				mu *= 0.5; 
			}else if (stroke != 0.0) {
				mu = stroke; 
				stroke = 0.0; 
			}else if (! loong && 2*iter >  m_maxNumIterations ) {
				loong = true; 
				mu *= 0.5; 
			}
		}
		w_old2 = w_old; 
		w_old = w; 
		
		cvdebug() << "w_old=" << w_old << " norm=" << norm << "\n"; 
		
		iter++; 
	}
	
	return delta < m_epsilon; 
}

bool FastICA::fpica_defl(const Matrix& X, Matrix& B)
{ 
	// not yet supported 
	assert(!m_with_initial_guess); 
		
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> random_source( -0.5, 0.5 ); 

	Vector w( X.rows(), false); 

	m_nonlinearity->set_signal(&X);
	
	bool global_converged = true; 
	
	for (int i = 0; i < m_numOfIC; ++i) {
		
		// initalize vector (should also go into extra class 
		for (unsigned i = 0; i < w.size(); ++i) 
			w[i] = random_source(gen);
		
		bool converged = fpica_defl_round(i, w, B); 
		cvmsg() << "Round(" << i << ")" << (converged ? "converged" : "did not converge") << "\n"; 

		global_converged &= converged; 
		B.set_column(i, w);
	}

	return global_converged; 
}


static double min_abs_diag(const Matrix& m)
{
	unsigned N = m.rows() > m.cols() ? m.rows() : m.cols(); 
	double min_val = fabs(m(0,0)); 
	for (unsigned i = 1; i < N; ++i) {
		double v = fabs(m(i,i)); 
		if ( min_val > v) 
			min_val = v; 
	}
	return min_val; 
}

double FastICA::fpica_symm_step(Matrix& B, Matrix& B_old,double mu, Matrix& workspace)
{
	m_nonlinearity->set_mu(mu); 
	m_nonlinearity->apply(B);
	matrix_inv_sqrt(B);
	multiply_mT_m(workspace, B, B_old); 
	return min_abs_diag(workspace); 
}

bool FastICA::fpica_symm(const Matrix& X, Matrix& B)
{
	// not yet supported 
	assert(!m_with_initial_guess); 

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> random_source( -0.5, 0.5 ); 

	Matrix B_old2(B);
	Matrix B_restart(B);
	
	// random and orthogonalize 
	for(auto ib = B.begin(); ib != B.end(); ++ib) 
		*ib = random_source(gen); 

	matrix_orthogonalize(B); 
	Matrix B_old(B);

	m_nonlinearity->set_signal(&X);
	
	bool is_fine_tuning = false; 
	double mu = m_mu;

	Matrix BTB(B.cols(), B.rows(), false); 

	bool converged = false; 
	int iter = 0; 
	double stroke = 0.0; 
	bool loong = false; 

	int maxiter = m_maxNumIterations; 

	bool do_saddle_check = m_do_saddle_check; 
	bool finished = !do_saddle_check; 

	do {
		while (!converged  && iter < maxiter) {

			double minAbsCos = fpica_symm_step(B, B_old, mu, BTB); 
			
			cvdebug() << "B= "  << B << "\n"; 

			cvmsg() << "FastICA: "<<  iter << ":" << 1.0 - minAbsCos << "\n"; 
			
			if ( 1.0 - minAbsCos < m_epsilon) {
				// run one more time with lower step-width
				if (m_finetune && !is_fine_tuning) {
					mu *= 0.01;
					is_fine_tuning = true; 
					maxiter += m_maxFineTune;
				} else {
					converged = true; 
				}
			} else if (m_stabilization) {
				multiply_mT_m(BTB, B, B_old2); 
				double minAbsCos2 = min_abs_diag(BTB); 
				
				// Avoid ping-pong 
				if (!stroke && (1 - minAbsCos2 < m_epsilon)) {
					stroke = mu; 
					mu /= 2.0; 
				} else if (stroke) { // back to normal 
					mu = stroke; 
					stroke = 0; 
				} else if ( !loong && 2 * iter > m_maxNumIterations) {
					// already running some time and 
					// no convergence, try half step width 
					// once
					loong = true; 
					mu *= 0.5; 
				}
			}
			
			B_old2 = B_old; 
			B_old = B;
			
			++iter; 
		}
		if (do_saddle_check) {
			do_saddle_check = run_saddlecheck(B, X); 
			m_saddle_postiter = maxiter; 
			iter = 0; 
		} else 
			finished = true;
		
	} while (!finished); 
	
	return converged; 	
}


/*
  This is the saddle check as described in 
 
  Petr Tichavský, Zbynek Koldovský, and Erkki Oja
  "Performance Analysis of the FastICA Algorithm and Cramér–Rao "
  "Bounds for Linear Independent Component Analysis"
  IEEE Tran Signal Processing, 54(4), 2006, 1189-1203  
  
  Returns true if at least one check resulted in an update of the
  components.
  
*/ 
bool FastICA::run_saddlecheck(Matrix &B, const Matrix& X)
{
	bool result = false; 
	vector<bool> rotated(B.cols(), false); 
	Matrix U(B.cols(), X.cols(), false); 
	multiply_mT_m(U, B, X); 
	auto table = m_nonlinearity->get_saddle_test_table(U);
	
	for (unsigned i = 0; i < U.rows(); ++i) {
		for (unsigned j = i+1; j < U.rows() && !rotated[i]; ++j) {
			if (rotated[j]) 
				continue; 
			auto ui = U.get_row(i);
			auto uj = U.get_row(j);
			const double isqrt2 = 1.0 / sqrt(2.0); 
			auto ui_new = (ui + uj) * isqrt2; 
			auto uj_new = (ui - uj) * isqrt2; 
			
			auto ui_sir = m_nonlinearity->get_saddle_test_value(ui_new);
			auto uj_sir = m_nonlinearity->get_saddle_test_value(uj_new);
			
			if (fmax(ui_sir, uj_sir) > fmax(table[i], table[j])) {
				result = rotated[i] = rotated[j] = true;
				auto bi_new = (B.get_row(i) + B.get_row(j)) * isqrt2; 
				auto bj_new = (B.get_row(i) - B.get_row(j)) * isqrt2; 
				B.set_row(i, bi_new); 
				B.set_row(j, bj_new);
				cvdebug() << "Rotating components " << i << " and " << j << "\n"; 
			}
		}
	}
	return result; 
}; 

void FastICA::set_approach(EApproach apr)
{
        m_approach = apr; 
}

void FastICA::set_nr_of_independent_components (int nrIC)
{
        m_numOfIC = nrIC; 
}


void FastICA::set_nonlinearity (PFastICADeflNonlinearity g)
{
        assert(g); 
        m_nonlinearity = g; 
}


void FastICA::set_finetune (bool finetune)
{
        m_finetune = finetune; 
}

void FastICA::set_mu (double mu)
{
        m_mu = mu; 
}

void FastICA::set_epsilon (double epsilon)
{
        m_epsilon = epsilon; 
}

void FastICA::set_sample_size (double sampleSize)
{
        m_sampleSize = sampleSize; 
}

void FastICA::set_component_energy_limit(double limit)
{
	m_component_energy_limit = limit; 
}

void FastICA::set_stabilization (bool stabilization)
{
        m_stabilization = stabilization; 
}

void FastICA::set_max_num_iterations (int maxNumIterations)
{
        m_maxNumIterations = maxNumIterations; 
}

void FastICA::set_max_fine_tune (int maxFineTune)
{
        m_maxFineTune = maxFineTune; 
}

void FastICA::set_pca_only (bool PCAonly)
{
        m_PCAonly = PCAonly; 
}

void FastICA::set_init_guess (const Matrix&  initGuess)
{
        m_initGuess = initGuess;
}

void FastICA::set_saddle_check(bool saddle_check)
{
	m_do_saddle_check = saddle_check; 
}

void FastICA::set_saddle_check_postiterations(int saddle_postiter)
{
	m_saddle_postiter = saddle_postiter; 
}
	
int FastICA::get_nr_of_independent_components () const
{
        return m_numOfIC; 
}

const Matrix& FastICA::get_mixing_matrix () const
{
        return m_mixing_matrix; 
}

const Matrix& FastICA::get_separating_matrix () const
{
        return m_separating_matrix; 
}

const Matrix& FastICA::get_independent_components () const
{
        return m_independent_components; 
}


const Matrix& FastICA::get_principal_eigenvectors () const
{
        return m_principal_eigenvectors; 
}

const Matrix& FastICA::get_whitening_matrix () const
{
        return m_whitening_matrix; 
}

const Matrix& FastICA::get_dewhitening_matrix () const
{
        return m_dewhitening_matrix; 
}

const Matrix& FastICA::get_white_signal () const
{
        return m_white_sig; 
}

NS_END  
