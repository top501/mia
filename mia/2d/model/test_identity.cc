/* -*- mia-c++  -*-
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
\author Gert Wollny <wollny at die.upm.ed>

*/

#include <mia/internal/autotest.hh>

#include <mia/2d/model/identity.hh>

using namespace identity_regmodel;

BOOST_AUTO_TEST_CASE( test_modelsolver )
{
	C2DIdentityRegModel model;

	C2DBounds size(2,3);
	C2DFVectorfield B(size);
	C2DFVectorfield X(size);

	for (size_t y = 0; y < size.y; ++y)
		for (size_t x = 0; x < size.x; ++x) {
			B(x,y ) = C2DFVector(x+1, y+1);
		}

	model.solve(B,X);

	for (size_t y = 0; y < size.y; ++y)
		for (size_t x = 0; x < size.x; ++x) {
			BOOST_CHECK_EQUAL(B(x,y), X(x,y));
		}
}
