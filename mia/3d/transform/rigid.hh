/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2010
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


#ifndef mia_3d_rigidtransform_hh
#define mia_3d_rigidtransform_hh

#include <iterator>
#include <boost/lambda/lambda.hpp>
#include <mia/3d/transform.hh>


NS_MIA_BEGIN



class C3DRigidTransformation;
class EXPORT_3D C3DRigidTransformation : public C3DTransformation {
public:
	enum EParamPosition {
		pp_translate_x = 0,
		pp_translate_y,
		pp_translate_z,
		pp_rotate_xy,
		pp_rotate_xz,
		pp_rotate_yz,
	};


	C3DRigidTransformation(const C3DBounds& size);
	C3DRigidTransformation(const C3DBounds& size,const C3DFVector& translation,
					       float rotation);

	void translate(float x, float y, float z);
	void rotate(float xy, float xz, float yz);

	C3DFVector apply(const C3DFVector& x) const;


	class EXPORT_3D iterator_impl: public C3DTransformation::iterator_impl  {
	public:
		iterator_impl(const C3DBounds& pos, const C3DBounds& size, 
			      const C3DRigidTransformation& trans); 
	private: 
		virtual C3DTransformation::iterator_impl * clone() const; 
		virtual const C3DFVector&  do_get_value()const; 
		virtual void do_x_increment(); 
		virtual void do_y_increment(); 
		virtual void do_z_increment(); 

		const C3DRigidTransformation& _M_trans;
		C3DFVector _M_value;
		C3DFVector _M_dx;

	};


	C3DTransformation::const_iterator begin() const;
	C3DTransformation::const_iterator end() const;

	virtual const C3DBounds& get_size() const;
	virtual C3DTransformation *invert() const;
	virtual P3DImage apply(const C3DImage& image, const C3DInterpolatorFactory& ipf) const;
	virtual bool save(const std::string& filename, const std::string& type) const;
	virtual P3DTransformation upscale(const C3DBounds& size) const;
	virtual void add(const C3DTransformation& a);
	virtual void translate(const C3DFVectorfield& gradient, gsl::DoubleVector& params) const;
	virtual size_t degrees_of_freedom() const;
	virtual void update(float step, const C3DFVectorfield& a);
	virtual C3DFMatrix derivative_at(int x, int y, int z) const;
	virtual gsl::DoubleVector get_parameters() const;
	virtual void set_parameters(const gsl::DoubleVector& params);
	virtual void set_identity();
	virtual float get_max_transform() const;
	virtual float pertuberate(C3DFVectorfield& v) const;
	virtual C3DFVector operator () (const C3DFVector& x) const;
	virtual float get_jacobian(const C3DFVectorfield& v, float delta) const;
	C3DFVector transform(const C3DFVector& x)const;
	virtual float divergence() const;
	virtual float curl() const;
	float grad_divergence() const;
	float grad_curl() const;
	double get_divcurl_cost(double wd, double wr, gsl::DoubleVector& gradient) const; 
	double get_divcurl_cost(double wd, double wr) const; 
private:
	virtual C3DTransformation *do_clone() const;
	void evaluate_matrix() const;
	C3DRigidTransformation(const C3DRigidTransformation& other);
	C3DRigidTransformation& operator =(const C3DRigidTransformation& other);
	mutable std::vector<double> _M_t;
	C3DBounds _M_size;
	C3DFVector _M_translation;
	float _M_rotation;
	mutable bool _M_matrix_valid;
};


NS_MIA_END

namespace std {
	template <>
	struct iterator_traits<mia::C3DRigidTransformation::const_iterator> {
		typedef input_iterator_tag iterator_category;
		typedef mia::C3DFVector     value_type;
		typedef mia::C3DBounds      difference_type;
		typedef mia::C3DFVector*    pointer;
		typedef mia::C3DFVector&    reference;
	};
}
#endif