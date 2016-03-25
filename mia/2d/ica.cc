/* -*- mia-c++  -*-
 *
 * This file is part of MIA - a toolbox for medical image analysis 
 * Copyright (c) Leipzig, Madrid 1999-2015 Gert Wollny
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

#ifdef WIN32
#  define EXPORT_TDataSeriesICA __declspec(dllexport)
#else
# ifdef __GNUC__
#   define EXPORT_TDataSeriesICA __attribute__((visibility("default")))
# else
#   define EXPORT_TDataSeriesICA
# endif
#endif

#include <mia/2d/ica.hh>
#include <mia/core/ica_template.cxx>


NS_MIA_BEGIN
template class TDataSeriesICA<C2DFImage>;
NS_MIA_END
