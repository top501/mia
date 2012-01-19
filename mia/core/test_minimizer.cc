/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2012 Gert Wollny
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

#define BOOST_TEST_MODULE MINIMIZER_TEST

#include <mia/internal/autotest.hh>

#include <mia/core/minimizer.hh>
#include <mia/core/cmdlineparser.hh>
#include <mia/core/msgstream.hh>


using namespace std; 
using namespace mia; 
using namespace boost;
namespace bfs=::boost::filesystem; 

class CMinimizerMock : public CMinimizer {
	
	
private: 
	int do_run(CDoubleVector& x);
}; 


class CProblemMock : public CMinimizer::Problem {
private: 
	virtual double  do_f(const CDoubleVector& x); 
	virtual void    do_df(const CDoubleVector& x, CDoubleVector&  g); 
	virtual double  do_fdf(const CDoubleVector& x, CDoubleVector&  g); 
	virtual size_t do_size() const; 
}; 


const char *minimizer_property = "CMinimizerMock"; 


BOOST_AUTO_TEST_CASE( test_load_minimizer_plugins )	
{
	list< bfs::path> searchpath; 

	searchpath.push_back( bfs::path("minimizer"));
	
	CMinimizerPluginHandler::set_search_path(searchpath); 

	const CMinimizerPluginHandler::Instance& handler = CMinimizerPluginHandler::instance();

	BOOST_CHECK_EQUAL(handler.size(), 3u); 
	BOOST_CHECK_EQUAL(handler.get_plugin_names(), "gdsq gsl nlopt "); 

}

BOOST_AUTO_TEST_CASE( test_missing_property_throws )
{
	CMinimizer::PProblem problem(new CProblemMock); 
	CMinimizerMock minimizer; 
	
	minimizer.add(minimizer_property); 
	minimizer.set_problem(problem); 

	CDoubleVector x(problem->size()); 
	BOOST_CHECK_THROW(minimizer.run(x), invalid_argument); 
	
}

BOOST_AUTO_TEST_CASE( test_test_function_calls )
{
	CMinimizer::PProblem problem(new CProblemMock); 
	CMinimizerMock minimizer; 
	minimizer.set_problem(problem); 
	CDoubleVector x(problem->size()); 
	fill(x.begin(), x.end(), 10.0); 
	BOOST_REQUIRE(minimizer.run(x) == 0); 
	
	
}

BOOST_AUTO_TEST_CASE( test_test_function_calls_2 )
{
	CMinimizer::PProblem problem(new CProblemMock); 
	CMinimizerMock minimizer; 
	minimizer.set_problem(problem); 
	CDoubleVector x(problem->size()); 
	fill(x.begin(), x.end(), 10.0); 
	
	CDoubleVector g(problem->size()); 

	BOOST_CHECK_CLOSE(problem->f(x), 100.0, 0.0001); 
	BOOST_CHECK_CLOSE(problem->fdf(x, g), 100.0, 0.0001); 
	
	for (size_t i = 0; i < problem->size(); ++i) {
		BOOST_CHECK_CLOSE(g[i], 2 * x[i], 0.0001);  
	}
	problem->df(x, g);	
	for (size_t i = 0; i < problem->size(); ++i) {
		BOOST_CHECK_CLOSE(g[i], 2 * x[i], 0.0001);  
	}


	vector<double> vx(problem->size()); 
	fill(vx.begin(), vx.end(), 10.0); 
	vector<double> vg(problem->size()); 
	
	BOOST_CHECK_CLOSE(problem->f(vx), 100.0, 0.0001); 
	BOOST_CHECK_CLOSE(problem->fdf(vx, vg), 100.0, 0.0001);
	
	for (size_t i = 0; i < problem->size(); ++i) {
		BOOST_CHECK_CLOSE(vg[i], 2 * vx[i], 0.0001);  
	}
	
	problem->df(vx, vg);
	for (size_t i = 0; i < problem->size(); ++i) {
		BOOST_CHECK_CLOSE(vg[i], 2 * vx[i], 0.0001);  
	}

	
}


int CMinimizerMock::do_run(CDoubleVector& x)
{
	// this test minimization is only possible because the 
	// minimum is the only root 
	double f; 
	CDoubleVector g(get_problem().size()); 
	int niter = 100; 
	do {
		f = get_problem().fdf(x, g);
		for (auto ix = x.begin(), ig = g.begin(); ix != x.end(); ++ix, ++ig) {
			if (*ig != 0.0) 
				*ix -= f / *ig; 
		}
		--niter; 
		cvdebug() <<"[" << niter << "]: x = " << x[0] << ", f = " << f <<"\n"; 
	} while (f > 0.0001 && niter); 
	
	return niter > 0 ? 0 : -1; 
}

double CProblemMock::do_f(const CDoubleVector& x)
{
	return inner_product(x.begin(), x.end(), x.begin(), 0.0); 
}

void   CProblemMock::do_df(const CDoubleVector& x, CDoubleVector&  g)
{
	assert(x.size() == g.size()); 
	transform(x.begin(), x.end(), g.begin(), [](double x) {return 2 * x;}); 
}

double CProblemMock::do_fdf(const CDoubleVector& x, CDoubleVector&  g)
{
	do_df(x, g); 
	return do_f(x); 
}

size_t CProblemMock::do_size() const
{
	return 1; 
}
