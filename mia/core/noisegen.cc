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

/*
  LatexBeginPluginSection{Noise Generators}
  \label{sec:noisegen}
  
  These plug-ins are used to create specific types of random data. 
  
  Noise Generators are created by the CNoiseGeneratorPluginHandler singleton.

  LatexEnd
*/



#include <mia/core/export_handler.hh>

#define VSTREAM_DOMAIN "NOISE_GENERATOR"

#include <ctime>
#include <cstdlib>

#include <mia/core/noisegen.hh>
#include <mia/core/handler.cxx>
#include <mia/core/plugin_base.cxx>


NS_MIA_BEGIN


const char *grayscale_noise_data::data_descr = "noise";
const char *generator_type::type_descr = "generator";

CNoiseGenerator::CNoiseGenerator(unsigned int seed)
{
	if (seed)
		srand(seed);
	else
		srand(time(NULL));
}
CNoiseGenerator::~CNoiseGenerator()
{
}

double CNoiseGenerator::operator ()()const
{
	return get();
}

double CNoiseGenerator::ranf() const
{
	double x = rand();

	return x / RAND_MAX;
}

EXPLICIT_INSTANCE_HANDLER(CNoiseGenerator); 

NS_MIA_END
