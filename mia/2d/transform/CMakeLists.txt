#
# This file is part of MIA - a toolbox for medical image analysis 
# Copyright (c) Leipzig, Madrid 1999-2015 Gert Wollny
#
# MIA is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
#

SET(transforms affine rigid rotation translate) 
PLUGINGROUP_WITH_TEST_AND_PREFIX2("2dimage" "transform" "${transforms}" "${MIA2DLIBS}")

SET(nonlinear_src spline.cc  vectorfield.cc) 
PLUGIN_WITH_TEST_MULTISOURCE("nonlinear" "2dimage" "transform" "${nonlinear_src}" "${MIA2DLIBS}") 

