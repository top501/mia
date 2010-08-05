/* -*- mia-c++  -*-
 * Copyright (c) Leipzig, Madrid 2004-2010
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *


 Function  CGaussNoiseGenerator::box_muller implements the Polar form
 of the Box-Muller Transformation and is

 (c) Copyright 1994, Everett F. Carter Jr.
 Permission is granted by the author to use
 this software for any application provided this
 copyright notice is preserved.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <mia/core/noise/uniform.hh>


NS_BEGIN(uniform_noise_generator)
using namespace std;
using namespace mia;

static const size_t center = 1024;

CUniformNoiseGenerator::CUniformNoiseGenerator(unsigned int seed, double a, double b):
	CNoiseGenerator(seed),
	_M_a(a),
	_M_step(1.0 / (b-a))
{
}

double CUniformNoiseGenerator::get() const
{
	return _M_step * ranf() + _M_a;
}


CUniformNoiseGeneratorFactory::CUniformNoiseGeneratorFactory():
	CNoiseGeneratorPlugin("uniform"),
	_M_param_seed(0),
	_M_param_a(0),
	_M_param_b(1)
{
	add_parameter("a", new CFloatParameter(_M_param_a, -numeric_limits<float>::max(),
								       numeric_limits<float>::max(),
								       false, "mean of distribution"));

	add_parameter("b", new CFloatParameter(_M_param_b, -numeric_limits<float>::max(),
								       numeric_limits<float>::max(),
								       false, "standart derivation of distribution"));
	add_parameter("seed", new CUIntParameter(_M_param_seed, 0,   numeric_limits<unsigned int>::max(),
						  false, "set random seed (0=init based on system time)"));

}

CNoiseGeneratorPlugin::ProductPtr
CUniformNoiseGeneratorFactory::do_create()const
{
	return CNoiseGeneratorPlugin::ProductPtr(new CUniformNoiseGenerator(_M_param_seed, _M_param_a, _M_param_b));
}

const string CUniformNoiseGeneratorFactory::do_get_descr()const
{
	return "Uniform noise generator using C stdlib rand()";
}

bool CUniformNoiseGeneratorFactory::do_test()const
{
	const double a = 1.0;
	const double b = 2.0;

	CUniformNoiseGenerator ng(1, a, b);

	double sum1 = 0.0;
	double sum2 = 0.0;
	const size_t n = 10000000;

	size_t k = n;
	while (k--) {
		double val = ng();
		sum1 += val;
		sum2 += val * val;
	}

	cvdebug() << sum1 << " (" << sum2 << ")\n";

	sum1 /= n;
	sum2 = sqrt(( sum2 - n * sum1 * sum1) / (n-1));

	cvdebug() << sum1 << " (" << sum2 << ")\n";
	double mu = (b+a) * 0.5;
	double sigma = sqrt((b-a) * (b-a) / 12.0);

	if (fabs(mu - sum1) > 0.01 || fabs(sigma  - sum2) > 0.01) {
		cvfail() << "avargaing at " << sum1 << " should be " << mu << " sigma " << sum2 << " should be " << sigma << "\n";
		return -1;
	}

	return 1;
}


extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	return new CUniformNoiseGeneratorFactory();
}

NS_END
