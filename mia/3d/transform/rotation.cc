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

#include <fstream>
#include <mia/core/msgstream.hh>
#include <mia/core/utils.hh>
#include <mia/3d/transformfactory.hh>

#include <mia/3d/transform/rotation.hh>

NS_MIA_BEGIN
using namespace std;


C3DFVector C3DRotationTransformation::apply(const C3DFVector& x) const
{
	return transform(x);
}



C3DFVector C3DRotationTransformation::transform(const C3DFVector& x)const
{
	if (!m_matrix_valid)
		evaluate_matrix();

	return C3DFVector(
		m_t[0] * x.x + m_t[1] * x.y + m_t[2] * x.z,
		m_t[3] * x.x + m_t[4] * x.y + m_t[5] * x.z,
		m_t[6] * x.x + m_t[7] * x.y + m_t[8] * x.z);
}

C3DRotationTransformation::C3DRotationTransformation(const C3DBounds& size, const C3DInterpolatorFactory& ipf):
	C3DTransformation(ipf), 
	m_t(9),
	m_size(size),
	m_rotation(0.0,0.0,0.0),
	m_matrix_valid(false)
{
}

C3DRotationTransformation::C3DRotationTransformation(const C3DRotationTransformation& other):
	C3DTransformation(other), 
	m_t(other.m_t),
	m_size(other.m_size),
	m_rotation(other.m_rotation),
	m_matrix_valid(m_matrix_valid)
{
}

C3DTransformation *C3DRotationTransformation::do_clone()const
{
	return new C3DRotationTransformation(*this);
}

C3DTransformation *C3DRotationTransformation::invert()const
{
	
	C3DRotationTransformation *result = new C3DRotationTransformation(*this); 
	cverr() << "to be implemented\n"; 
	result->m_matrix_valid = false; 
	return result;
}


C3DRotationTransformation::C3DRotationTransformation(const C3DBounds& size,
					       const C3DFVector& rotation, const C3DInterpolatorFactory& ipf):
	C3DTransformation(ipf), 
	m_t(9),
	m_size(size),
	m_rotation(rotation),
	m_matrix_valid(false)
{
}

size_t C3DRotationTransformation::degrees_of_freedom() const
{
	return 3;
}

void C3DRotationTransformation::update(float /*step*/, const C3DFVectorfield& /*a*/)
{
	assert(!"not implemented");
}

void C3DRotationTransformation::rotate(float xy, float xz, float yz)
{
	m_matrix_valid = false;
	m_rotation.z += xy;
	m_rotation.y += xz;
	m_rotation.x += yz;
	float sx,cx,sy,cy,sz,cz; 
	sincosf(m_rotation.z, &sz, &cz); 
	sincosf(m_rotation.y, &sy, &cy);
	sincosf(m_rotation.x, &sx, &cx);
}

CDoubleVector C3DRotationTransformation::get_parameters() const
{
	CDoubleVector result(degrees_of_freedom());
	result[0] = m_rotation.x;
	result[1] = m_rotation.y;
	result[2] = m_rotation.z;

	return result;
}

void C3DRotationTransformation::set_parameters(const CDoubleVector& params)
{
	assert(degrees_of_freedom() == params.size());

	m_rotation.x    = params[0];
	m_rotation.y    = params[1];
	m_rotation.z    = params[2];
	cvdebug() << "Rotation transform = (" << m_rotation << ")\n"; 

	m_matrix_valid = false;
}

float C3DRotationTransformation::divergence() const
{
	return 0.0;
}

float C3DRotationTransformation::grad_divergence() const
{
	return 0.0;
}


float C3DRotationTransformation::grad_curl() const
{
	return 0.0;
}


float C3DRotationTransformation::curl() const
{
	// this is not right
	return 0.0;
}

double C3DRotationTransformation::get_divcurl_cost(double /*wd*/, double /*wr*/, CDoubleVector& /*gradient*/) const
{
	return 0.0; 
}

double C3DRotationTransformation::get_divcurl_cost(double /*wd*/, double /*wr*/) const
{
	return 0.0; 
}


const C3DBounds& C3DRotationTransformation::get_size() const
{
	return m_size;
}

P3DTransformation C3DRotationTransformation::do_upscale(const C3DBounds& size) const
{
	return P3DTransformation(new C3DRotationTransformation(size, m_rotation, get_interpolator_factory()));
}

C3DFMatrix C3DRotationTransformation::derivative_at(const C3DFVector& MIA_PARAM_UNUSED(x)) const
{
	if (!m_matrix_valid)
		evaluate_matrix();
	return C3DFMatrix(
		C3DFVector(m_t[0], m_t[3], m_t[6]), 
		C3DFVector(m_t[1], m_t[4], m_t[7]), 
		C3DFVector(m_t[2], m_t[5], m_t[8]));
}

C3DFMatrix C3DRotationTransformation::derivative_at(int MIA_PARAM_UNUSED(x), int MIA_PARAM_UNUSED(y), 
						 int MIA_PARAM_UNUSED(z)) const
{
	if (!m_matrix_valid)
		evaluate_matrix();

	return C3DFMatrix(
		C3DFVector(m_t[0], m_t[3], m_t[6]), 
		C3DFVector(m_t[1], m_t[4], m_t[7]), 
		C3DFVector(m_t[2], m_t[5], m_t[8]));
}

void C3DRotationTransformation::set_identity()
{
	cvdebug() << "set identity\n";
	m_rotation    = C3DFVector(); 
}

void C3DRotationTransformation::evaluate_matrix() const
{
	CScopedLock lock(m_mutex); 
	
	float sx,cx,sy,cy,sz,cz; 
	sincosf(m_rotation.z, &sz, &cz); 
	sincosf(m_rotation.y, &sy, &cy);
	sincosf(m_rotation.x, &sx, &cx);

	m_t[0] =    cy*cz;
	m_t[1] =  - cx*sz-cz*sx*sy; 
	m_t[2] =    sx*sz-cx*cz*sy;
	
	m_t[3] = cy*sz;
	m_t[4] = cx*cz-sx*sy*sz;
	m_t[5] = - cx*sy*sz-cz*sx;

	m_t[6] = sy;
	m_t[7] = cy*sx;
	m_t[8]= cx*cy;

	m_matrix_valid = true;
}

float C3DRotationTransformation::get_max_transform() const
{
	if (!m_matrix_valid) 
		evaluate_matrix(); 
	C3DFVector corners[7] = {
		C3DFVector(get_size().x, 0, 0), 
		C3DFVector(get_size().x, get_size().y,            0), 
		C3DFVector(           0, get_size().y,            0), 
		C3DFVector(           0, get_size().y, get_size().z), 
		C3DFVector(get_size().x,            0, get_size().z), 
		C3DFVector(           0,            0, get_size().z), 
		C3DFVector(get_size())
	};

	float result = apply(C3DFVector()).norm2(); 
	for(int i = 0; i < 7; ++i) {
		float h = (apply(corners[i]) - corners[i]).norm2(); 
		if (result < h) 
			result = h; 
	}

	return sqrt(result);

}

void C3DRotationTransformation::add(const C3DTransformation& /*other*/)
{
	// *this  = other * *this
	assert(0 && "not implemented");
}

C3DFVector C3DRotationTransformation::operator () (const C3DFVector& x) const
{
	return apply(x); 
}

float C3DRotationTransformation::get_jacobian(const C3DFVectorfield& /*v*/, float /*delta*/) const
{
	// a rotation transformation doesn't introduce a volume change
	return 1.0; 
}

void C3DRotationTransformation::translate(const C3DFVectorfield& gradient, CDoubleVector& params) const
{
	//

	assert(gradient.get_size() == m_size);
	assert(params.size() == degrees_of_freedom());

	vector<double> r(params.size(), 0.0);

	auto g = gradient.begin();
	for (size_t z = 0; z < m_size.z; ++z)
		for (size_t y = 0; y < m_size.y; ++y)
			for (size_t x = 0; x < m_size.x; ++x, ++g) {
				r[0] += -float(z) * g->y + float(y) * g->z; 
				r[1] += -float(z) * g->x + float(x) * g->z; 
				r[2] += -float(y) * g->x + float(x) * g->y; 
			}
	copy(r.begin(), r.end(), params.begin()); 
}



C3DRotationTransformation::iterator_impl::iterator_impl(const C3DBounds& pos, const C3DBounds& size, 
						      const C3DRotationTransformation& trans):
	C3DTransformation::iterator_impl(pos, size),
	m_trans(trans), 
	m_value(trans.transform(C3DFVector(pos)))
{
	m_dx = m_trans.transform(C3DFVector(pos.x + 1.0, pos.y, pos.z)) - m_value;
}

C3DRotationTransformation::iterator_impl::iterator_impl(const C3DBounds& pos, const C3DBounds& begin, 
						     const C3DBounds& end, const C3DBounds& size, 
						     const C3DRotationTransformation& trans):
	C3DTransformation::iterator_impl(pos, begin, end, size),
	m_trans(trans), 
	m_value(trans.apply(C3DFVector(pos)))
{
	m_dx = m_trans.transform(C3DFVector(pos.x + 1.0, pos.y, pos.z)) - m_value;
}
		
C3DTransformation::iterator_impl * C3DRotationTransformation::iterator_impl::clone() const
{
	return new iterator_impl(*this); 
}

const C3DFVector&  C3DRotationTransformation::iterator_impl::do_get_value()const
{
	return m_value; 
}

void C3DRotationTransformation::iterator_impl::do_x_increment()
{
	m_value += m_dx; 
}

void C3DRotationTransformation::iterator_impl::do_y_increment()
{
	m_value = m_trans.transform(C3DFVector(get_pos())); 
	m_dx = m_trans.transform(C3DFVector(get_pos().x + 1.0, get_pos().y, get_pos().z)) - m_value;
}

void C3DRotationTransformation::iterator_impl::do_z_increment()
{
	m_value = m_trans.transform(C3DFVector(get_pos())); 
	m_dx = m_trans.transform(C3DFVector(get_pos().x + 1.0, get_pos().y, get_pos().z)) - m_value;
}

C3DTransformation::const_iterator C3DRotationTransformation::begin() const
{
	if (!m_matrix_valid) 
		evaluate_matrix(); 
	return C3DTransformation::const_iterator(new iterator_impl(C3DBounds(0,0,0), get_size(), *this)); 
}

C3DTransformation::const_iterator C3DRotationTransformation::end() const
{
	return C3DTransformation::const_iterator(new iterator_impl(get_size(), get_size(), *this)); 
}

C3DTransformation::const_iterator C3DRotationTransformation::begin_range(const C3DBounds& begin, const C3DBounds& end) const
{
	if (!m_matrix_valid) 
		evaluate_matrix(); 
	return C3DTransformation::const_iterator(new iterator_impl(begin, begin, end, get_size(), *this)); 
}

C3DTransformation::const_iterator C3DRotationTransformation::end_range(const C3DBounds& begin, const C3DBounds& end) const
{
	return C3DTransformation::const_iterator(new iterator_impl(end, begin, end, get_size(), *this)); 
}


float C3DRotationTransformation::pertuberate(C3DFVectorfield& /*v*/) const
{
	DEBUG_ASSERT_RELEASE_THROW(false, "C3DAffineTransformation doesn't implement pertuberate."); 
}

class C3DRotationTransformCreator: public C3DTransformCreator {
public: 
	C3DRotationTransformCreator(const C3DInterpolatorFactory& ipf); 
private: 
	virtual P3DTransformation do_create(const C3DBounds& size, const C3DInterpolatorFactory& ipf) const;
};

C3DRotationTransformCreator::C3DRotationTransformCreator(const C3DInterpolatorFactory& ipf):
	C3DTransformCreator(ipf)
{
}

P3DTransformation C3DRotationTransformCreator::do_create(const C3DBounds& size, const C3DInterpolatorFactory& ipf) const
{
	return P3DTransformation(new C3DRotationTransformation(size, ipf));
}

class C3DRotationTransformCreatorPlugin: public C3DTransformCreatorPlugin {
public:
	C3DRotationTransformCreatorPlugin();
	virtual C3DTransformCreator *do_create(const C3DInterpolatorFactory& ipf) const;
	const std::string do_get_descr() const;
};

C3DRotationTransformCreatorPlugin::C3DRotationTransformCreatorPlugin():
	C3DTransformCreatorPlugin("rotation")
{
}

C3DTransformCreator *C3DRotationTransformCreatorPlugin::do_create(const C3DInterpolatorFactory& ipf) const
{
	return new C3DRotationTransformCreator(ipf);
}

const std::string C3DRotationTransformCreatorPlugin::do_get_descr() const
{
	return "Rotation transformation (three degrees of freedom).";
}

extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	return new C3DRotationTransformCreatorPlugin();
}


NS_MIA_END