/* -*- mia-c++  -*-
 *
 * Copyright (c) Madrid 2010-2011
 *
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

#ifndef mia_internal_fullcost_hh
#define mia_internal_fullcost_hh

#include <mia/core/product_base.hh>
#include <gsl++/vector.hh>

NS_MIA_BEGIN

/**
   Base class for a general cost function. 
 */

template <typename Transform>
class TFullCost : public CProductBase {
public: 
	typedef typename Transform::Size Size; 
	typedef TFullCost<Transform> plugin_data; 
	typedef TFullCost<Transform> plugin_type;

	static const char *type_descr;
	static const char *value;

	typedef std::shared_ptr<TFullCost<Transform> > Pointer; 
	
	/**
	   Initialize the cost function with a weight 
	   \param weight 
	 */
	TFullCost(double weight);


	/**
	   Evaluate the weighted cost value and the weighted gradient in optimizer space 
	   given a current  transformation. 
	   \param t tranformation to be applied 
	   \retval gradient gradient in optimizer space 
	   \returns weighted cost value 
	 */
	double evaluate(const Transform& t, gsl::DoubleVector& gradient) const;

	/**
	   Evaluate the weighted cost value 
	   \param t tranformation to be applied 
	   \returns weighted cost value 
	 */
	double cost_value(const Transform& t) const;

	/**
	   Evaluate the weighted cost value without transforming the image 
	   \returns weighted cost value 
	 */
	double cost_value() const;

	/**
	   Re-initialalize the cost function
	*/
	void reinit();
	
	/**
	   Set the size of the cost function 
	 */
	void set_size(const Size& size); 
protected: 
	/** \returns cost function weight  */
	double get_weight() const; 
	const Size& get_current_size() const; 
private:
	virtual double do_evaluate(const Transform& t, gsl::DoubleVector& gradient) const = 0;
	virtual double do_value(const Transform& t) const = 0;
	virtual double do_value() const = 0;
	virtual void do_reinit();
	virtual void do_set_size() = 0; 
	
	double _M_weight;
	Size _M_current_size; 

}; 

template <typename Transform>
class TFullCostPlugin: public TFactory<TFullCost<Transform> > {
public:
	TFullCostPlugin(const char *name);
private:
	virtual typename TFullCostPlugin<Transform>::ProductPtr do_create() const;
	virtual typename TFullCostPlugin<Transform>::ProductPtr do_create(float weight) const = 0;
	float _M_weight;
}; 


NS_MIA_END

#endif