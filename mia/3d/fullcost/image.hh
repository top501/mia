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

#ifndef mia_3d_imagefullcost_hh
#define mia_3d_imagefullcost_hh


#include <mia/3d/fullcost.hh>
#include <mia/3d/3dimageio.hh>
#include <mia/3d/cost.hh>

NS_MIA_BEGIN

class EXPORT C3DImageFullCost : public C3DFullCost {
public: 
	C3DImageFullCost(const std::string& src, 
			 const std::string& ref, 
			 const std::string& cost, 
			 double weight, 
			 bool debug); 
private: 
	double do_evaluate(const C3DTransformation& t, CDoubleVector& gradient) const;
	void do_set_size(); 

	static P3DImage get_from_pool(const C3DImageDataKey& key); 

	bool do_has(const char *property) const; 
	double do_value(const C3DTransformation& t) const; 

	double do_value() const; 
	void do_reinit(); 
	bool do_get_full_size(C3DBounds& size) const; 

	C3DImageDataKey m_src_key;
	C3DImageDataKey m_ref_key;
	
	P3DImage m_src; 
	P3DImage m_ref; 
	
	P3DImage m_src_scaled; 
	P3DImage m_ref_scaled;


	P3DImageCost m_cost_kernel; 
	bool m_debug;
}; 

NS_MIA_END

#endif
