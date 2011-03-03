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

#include <fstream>
#include <mia/core/msgstream.hh>
#include <mia/2d/transformfactory.hh>

#include <mia/2d/transform/rigid.hh>


NS_MIA_BEGIN
using namespace boost::lambda;
using namespace std;

P2DImage C2DRigidTransformation::apply(const C2DImage& image,
					const C2DInterpolatorFactory& ipf) const
{
	if (image.get_size() != get_size()) {
		cvwarn() << "C2DRigidTransformation::apply: size of input differs from transformation target size\n";
	}
	return transform2d(image, ipf, *this);
}

C2DFVector C2DRigidTransformation::apply(const C2DFVector& x) const
{
	return transform(x);
}



C2DFVector C2DRigidTransformation::transform(const C2DFVector& x)const
{
	if (!_M_matrix_valid)
		evaluate_matrix();

	return C2DFVector(
		_M_t[0] * x.x + _M_t[1] * x.y + _M_t[2],
		_M_t[3] * x.x + _M_t[4] * x.y + _M_t[5]
			  );
}

C2DRigidTransformation::C2DRigidTransformation(const C2DBounds& size):
	_M_t(6),
	_M_size(size),
	_M_translation(0.0, 0.0),
	_M_rotation(0.0),
	_M_matrix_valid(false)
{

}

C2DRigidTransformation::C2DRigidTransformation(const C2DRigidTransformation& other):
	_M_t(other._M_t),
	_M_size(other._M_size),
	_M_translation(other._M_translation),
	_M_rotation(other._M_rotation),
	_M_matrix_valid(_M_matrix_valid)
{
}

C2DTransformation *C2DRigidTransformation::do_clone()const
{
	return new C2DRigidTransformation(*this);
}

C2DTransformation *C2DRigidTransformation::invert()const
{
	C2DRigidTransformation *result = new C2DRigidTransformation(*this); 
	result->_M_rotation = -_M_rotation;  
	const double sina = sin(_M_rotation); 
	const double cosa = cos(_M_rotation); 
	result->_M_translation.x = - cosa * _M_translation.x - sina * _M_translation.y; 
	result->_M_translation.y =   sina * _M_translation.x - cosa * _M_translation.y; 
	result->_M_matrix_valid = false; 
	return result;
}


C2DRigidTransformation::C2DRigidTransformation(const C2DBounds& size,const C2DFVector& translation,
					       float rotation):
	_M_t(6),
	_M_size(size),
	_M_translation(translation),
	_M_rotation(rotation),
	_M_matrix_valid(false)
{
}

bool C2DRigidTransformation::save(const std::string& filename, const std::string& /*type*/) const
{
	ofstream file(filename.c_str());
	file << "Transformation: 2D\n"
	     << "Matrix: ";
	for (size_t i = 0; i < 6 ; ++i)
		file << _M_t[i] << " ";
	file << "\n";
	return file.good();
}

size_t C2DRigidTransformation::degrees_of_freedom() const
{
	return 3;
}

void C2DRigidTransformation::update(float /*step*/, const C2DFVectorfield& /*a*/)
{
	assert(!"not implemented");
}

void C2DRigidTransformation::translate(float x, float y)
{
	_M_matrix_valid = false;
	_M_translation.x +=  x;
	_M_translation.y +=  y;
}

void C2DRigidTransformation::rotate(float angle)
{
	_M_matrix_valid = false;
	_M_rotation += angle;
	const double sina = sin(angle);
	const double cosa = cos(angle);

	const double tx      = cosa * _M_translation.x - sina * _M_translation.y;
	_M_translation.y = sina * _M_translation.x + cosa * _M_translation.y;
	_M_translation.x = tx;
}

CDoubleVector C2DRigidTransformation::get_parameters() const
{
	CDoubleVector result(degrees_of_freedom());
	result[0] = _M_translation.x;
	result[1] = _M_translation.y;
	result[2] = _M_rotation;
	return result;
}

void C2DRigidTransformation::set_parameters(const CDoubleVector& params)
{
	assert(degrees_of_freedom() == params.size());

	_M_translation.x = params[0];
	_M_translation.y = params[1];
	_M_rotation = params[2];
	cvdebug() << "Rigid transform = (" << _M_translation << ", " << _M_rotation << ")\n"; 

	_M_matrix_valid = false;
}

float C2DRigidTransformation::divergence() const
{
	return 0.0;
}

float C2DRigidTransformation::grad_divergence() const
{
	return 0.0;
}


float C2DRigidTransformation::grad_curl() const
{
	return 0.0;
}


float C2DRigidTransformation::curl() const
{
	// this is not right
	return _M_rotation;
}

double C2DRigidTransformation::get_divcurl_cost(double /*wd*/, double /*wr*/, CDoubleVector& /*gradient*/) const
{
	return 0.0; 
}

double C2DRigidTransformation::get_divcurl_cost(double /*wd*/, double /*wr*/) const
{
	return 0.0; 
}


const C2DBounds& C2DRigidTransformation::get_size() const
{
	return _M_size;
}

P2DTransformation C2DRigidTransformation::do_upscale(const C2DBounds& size) const
{
	C2DFVector new_trans(float(size.x) / (float)get_size().x * _M_translation.x,
			     float(size.y) / (float)get_size().y * _M_translation.y);
	return P2DTransformation(new C2DRigidTransformation(size, new_trans, _M_rotation));
}

C2DFMatrix C2DRigidTransformation::derivative_at(int /*x*/, int /*y*/) const
{
	const double cosa = cos(_M_rotation);
	const double sina = sin(_M_rotation);

	return C2DFMatrix(C2DFVector(cosa, -sina),
			  C2DFVector(sina,  cosa));
}

void C2DRigidTransformation::set_identity()
{
	cvdebug() << "set identity\n";
	_M_translation.x = _M_translation.y = _M_rotation = 0.0;
}

void C2DRigidTransformation::evaluate_matrix() const
{
	const double cosa = cos(_M_rotation);
	const double sina = sin(_M_rotation);

	_M_t[0] = cosa;
	_M_t[1] = -sina;
	_M_t[2] = _M_translation.x;
	_M_t[3] = sina;
	_M_t[4] = cosa;
	_M_t[5] = _M_translation.y;

	_M_matrix_valid = true;
}

float C2DRigidTransformation::get_max_transform() const
{
	// check the corners
	float m =      (C2DFVector(get_size()) -  apply(C2DFVector(get_size()))).norm2();
	float test0Y = (C2DFVector(0, get_size().y) - apply(C2DFVector(0, get_size().y))).norm2();
	float testX0 = (C2DFVector(get_size().x, 0) - apply(C2DFVector(get_size().x, 0))).norm2();
	float test00 = apply(C2DFVector(0, 0)).norm2();

	if (m < test0Y)
		m = test0Y;

	if (m < testX0)
		m = testX0;

	if (m < test00)
		m = test00;
	return sqrt(m);
}

void C2DRigidTransformation::add(const C2DTransformation& /*other*/)
{
	// *this  = other * *this
	assert(0 && "not implemented");
}

C2DFVector C2DRigidTransformation::operator () (const C2DFVector& x) const
{
	return apply(x); 
}

float C2DRigidTransformation::get_jacobian(const C2DFVectorfield& /*v*/, float /*delta*/) const
{
	assert(!"not implemented");
}

void C2DRigidTransformation::translate(const C2DFVectorfield& gradient, CDoubleVector& params) const
{
	//

	assert(gradient.get_size() == _M_size);
	assert(params.size() == degrees_of_freedom());

	vector<double> r(params.size(), 0.0);

	auto g = gradient.begin();
	for (size_t y = 0; y < _M_size.y; ++y) {
		for (size_t x = 0; x < _M_size.x; ++x, ++g) {
			r[0] += g->x;
			r[1] += g->y;
			r[2] += -y * g->x + x * g->y; 
		}
	}
	params[0] = r[0];
	params[1] = r[1];
	params[2] = r[2];
}



C2DRigidTransformation::iterator_impl::iterator_impl(const C2DBounds& pos, const C2DBounds& size, 
						      const C2DRigidTransformation& trans):
	C2DTransformation::iterator_impl(pos, size),
	_M_trans(trans), 
	_M_value(trans.transform(C2DFVector(pos)))
{
	_M_dx = _M_trans.transform(C2DFVector(pos.x + 1.0, pos.y)) - _M_value;
}

C2DTransformation::iterator_impl * C2DRigidTransformation::iterator_impl::clone() const
{
	return new iterator_impl(get_pos(), get_size(), _M_trans); 
}

const C2DFVector&  C2DRigidTransformation::iterator_impl::do_get_value()const
{
	return _M_value; 
}

void C2DRigidTransformation::iterator_impl::do_x_increment()
{
	_M_value += _M_dx; 
}

void C2DRigidTransformation::iterator_impl::do_y_increment()
{
	_M_value = _M_trans.transform(C2DFVector(get_pos())); 
	_M_dx = _M_trans.transform(C2DFVector(get_pos().x + 1.0, get_pos().y)) - _M_value;
}


C2DTransformation::const_iterator C2DRigidTransformation::begin() const
{
	return C2DTransformation::const_iterator(new iterator_impl(C2DBounds(0,0), get_size(), *this)); 
}

C2DTransformation::const_iterator C2DRigidTransformation::end() const
{
	return C2DTransformation::const_iterator(new iterator_impl(get_size(), get_size(), *this)); 
}

float C2DRigidTransformation::pertuberate(C2DFVectorfield& /*v*/) const
{
	assert(!"not implemented");
}

class C2DRigidTransformCreator: public C2DTransformCreator {
	virtual P2DTransformation do_create(const C2DBounds& size) const;
};

P2DTransformation C2DRigidTransformCreator::do_create(const C2DBounds& size) const
{
	return P2DTransformation(new C2DRigidTransformation(size));
}

class C2DRigidTransformCreatorPlugin: public C2DTransformCreatorPlugin {
public:
	typedef C2DTransformCreatorPlugin::ProductPtr ProductPtr;

	C2DRigidTransformCreatorPlugin();
	virtual ProductPtr do_create() const;
	virtual bool do_test() const;
	const std::string do_get_descr() const;
};

C2DRigidTransformCreatorPlugin::C2DRigidTransformCreatorPlugin():
	C2DTransformCreatorPlugin("rigid")
{
}

C2DRigidTransformCreatorPlugin::ProductPtr
C2DRigidTransformCreatorPlugin::do_create() const
{
	return ProductPtr(new C2DRigidTransformCreator());
}

bool C2DRigidTransformCreatorPlugin::do_test() const
{
	return true;
}

const std::string C2DRigidTransformCreatorPlugin::do_get_descr() const
{
	return "plugin to create rigid transformations";
}

extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	return new C2DRigidTransformCreatorPlugin();
}


NS_MIA_END
