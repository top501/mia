/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2010
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
  This plug-in implements the navier-stokes operator like registration model
  that accounts for linear elastic and fluid dynamic registration.
  Which model is used depends on the selected time step.

  To solve the PDE a full SOR is implemented.
*/

#include <iomanip>
#include <set>
#include <limits>

#ifdef __POWERPC__
#include <ppc_intrinsics.h>
#endif

#include <mia/3d/model.hh>

#ifndef WIN32
#define PRIVATE __attribute__((visibility("hidden")))
#else
#define PRIVATE
#endif

NS_MIA_USE
using namespace boost;
using namespace std;

NS_BEGIN(naviera_regmodel)

class C3DNavierRegModel: public C3DRegModel {
public:
	C3DNavierRegModel(float mu, float lambda, size_t maxiter, float epsilon);

	bool  test_kernel();
private:
	virtual void do_solve(const C3DFVectorfield& b, C3DFVectorfield& x) const;
	virtual float do_force_scale() const;

	PRIVATE void set_flags(C3DUBImage::iterator loc)const;

	C3DFVector get_p(const C3DFVector& b, const C3DFVectorfield::iterator& v) const;
	C3DFVector get_q(const C3DFVectorfield::iterator& v)const;
	PRIVATE float  solve_at(const C3DFVector& b, C3DFVectorfield::iterator& x)const;
	float _M_mu;
	float _M_lambda;
	float _M_a1,_M_a, _M_b, _M_c, _M_a_b, _M_b_4;
//	float _M_omega;
	float _M_epsilon;
	size_t _M_max_iter;
	mutable int _M_dx;
	mutable int _M_dxy;

};

C3DNavierRegModel::C3DNavierRegModel(float mu, float lambda, size_t maxiter,  float epsilon):
	_M_mu(mu),
	_M_lambda(lambda),
	_M_epsilon(epsilon),
	_M_max_iter(maxiter)
{
	float a = mu;
	float b = lambda + mu;
	_M_c = 1 / (6*a+2*b);
	_M_b_4 = 0.25 * b * _M_c;
	_M_a_b = ( a + b ) * _M_c;
	_M_a = a * _M_c;

	cvdebug() << "initialise model with mu=" << mu << " lambda=" << lambda << "\n";
}


float C3DNavierRegModel::do_force_scale() const
{
	cvdebug() << "Force scale = " <<_M_c << "\n";
	return _M_c;
}

inline PRIVATE  void C3DNavierRegModel::set_flags(C3DUBImage::iterator loc)const
{
	const register int dx = _M_dx;
	const register int mdx = - _M_dx;

	C3DUBImage::iterator lmz = loc - _M_dxy;
	C3DUBImage::iterator lpz = loc + _M_dxy;
	C3DUBImage::iterator lmy = loc - dx;
	C3DUBImage::iterator lpy = loc + dx;

	lmz[     mdx] = 1;
	lmz[ -    1 ] = 1;
	lmz[      0 ] = 1;
	lmz[ +    1 ] = 1;
	lmz[      dx] = 1;

	lmy[ - 1 ] = 1;
	lmy[   0 ] = 1;
	lmy[   1 ] = 1;

	loc[ - 1 ] = 1;
	loc[   0 ] = 1;
	loc[ + 1 ] = 1;

	lpy[ - 1 ] = 1;
	lpy[   0 ] = 1;
	lpy[ + 1 ] = 1;

	lpz[   mdx ] = 1;
	lpz[ -   1 ] = 1;
	lpz[     0 ] = 1;
	lpz[     1 ] = 1;
	lpz[    dx ] = 1;
}


void C3DNavierRegModel::do_solve(const C3DFVectorfield& b, C3DFVectorfield& v) const
{
	// init velocity fields
	float start_residuum = 0.0;
	float residuum;
	float thresh = 0.0;

	size_t i = 0;
	assert(b.get_size() == v.get_size());

	_M_dx = b.get_size().x;
	_M_dxy = b.get_plane_size_xy();

	C3DFImage residua(b.get_size());
	fill(residua.begin(), residua.end(), 0.0f);

	C3DUBImage update_flags1(b.get_size());
	C3DUBImage update_flags2(b.get_size());

	C3DUBImage& update_flags = update_flags1;
	C3DUBImage& dset_flags = update_flags2;

	fill(update_flags.begin(), update_flags.end(), 1);

	do {
		++i;
		residuum = 0;



		for (size_t z = 1; z < b.get_size().z-1; z++) {

			size_t step = _M_dx + _M_dxy * z;

			C3DFVectorfield::const_iterator ib = b.begin() + step;
			C3DFVectorfield::iterator iv = v.begin() + step;
			C3DFImage::iterator ir = residua.begin() + step;
			C3DUBImage::iterator iu = update_flags.begin() + step;
			C3DUBImage::iterator is = dset_flags.begin() + step;


			for (size_t y = 1; y < b.get_size().y-1; y++) {
				++ib;
				++iv;
				++ir;
				++iu;
				++is;
				for (size_t x = 1; x < b.get_size().x-1; x++, ++ib, ++iv, ++ir, ++iu, ++is){
					if (*iu) {
						*iu = 0;
						*ir = solve_at(*ib, iv);
					}
					if (*ir > thresh)
						set_flags(is);

					residuum += *ir;
				}
				++ib;
				++iv;
				++ir;
				++iu;
				++is;

			}
			if (i ==1)
				start_residuum = residuum;

		}

		swap(update_flags, dset_flags);


		thresh = (residuum * residuum) / (start_residuum * residua.size() * (i+1));

		cvdebug() << "SOLVE " << setw(3) << i <<":" << residuum << " : " << thresh<< " : "<< residua.size()<<"\n";

		if (residuum < 1)
			break;

	} while (i < _M_max_iter && residuum / start_residuum > _M_epsilon);

}

inline C3DFVector C3DNavierRegModel::get_p(const C3DFVector& b, const C3DFVectorfield::iterator& v)const
{
	const C3DFVector vm = v[-1];
	const C3DFVector vp = v[1];

	const C3DFVector vym = v[-_M_dx];
	const C3DFVector vyp = v[ _M_dx];
	const C3DFVector vzm = v[-_M_dxy];
	const C3DFVector vzp = v[ _M_dxy];


	float rx = (vm.x + vp.x) * _M_a_b;
	float ry = (vm.y + vp.y) * _M_a_b;
	float rz = (vm.z + vp.z) * _M_a_b;

	rx += b.x;
	ry += b.y;
	rz += b.z;

	float hx = (vym.x + vyp.x + vzp.x + vzm.x) * _M_a;
	float hy = (vym.y + vyp.y + vzp.y + vzm.y) * _M_a;
	float hz = (vym.z + vyp.z + vzp.z + vzm.z) * _M_a;

	return  C3DFVector(rx + hx, ry + hy, rz + hz);
}

inline C3DFVector C3DNavierRegModel::get_q(const C3DFVectorfield::iterator& v)const
{

	const C3DFVector& vmm0 = v[ -1 - _M_dx];
	const C3DFVector& vpm0 = v[  1 - _M_dx];
	const C3DFVector& vmp0 = v[ -1 + _M_dx];
	const C3DFVector& vpp0 = v[  1 + _M_dx];

	const  C3DFVectorfield::iterator vm = v - _M_dxy;
	const  C3DFVectorfield::iterator vp = v + _M_dxy;

	const C3DFVector& vm0m = vm[ -1];
	const C3DFVector& vp0m = vm[  1];
	const C3DFVector& vm0p = vp[ -1];
	const C3DFVector& vp0p = vp[  1];

	const C3DFVector& v0mm = vm[ -_M_dx];
	const C3DFVector& v0pm = vm[  _M_dx];
	const C3DFVector& v0mp = vp[ -_M_dx];
	const C3DFVector& v0pp = vp[  _M_dx];


	return C3DFVector( (vmm0.y + vpp0.y - vmp0.y - vpm0.y + vm0m.z + vp0p.z - vm0p.z - vp0m.z) * _M_b_4,
			   (vmm0.x + vpp0.x - vmp0.x - vpm0.x + v0mm.z + v0pp.z - v0mp.z - v0pm.z) * _M_b_4,
			   (vm0m.x + vp0p.x - vm0p.x - vp0m.x + v0mm.y + v0pp.y - v0mp.y - v0pm.y) * _M_b_4);
}

inline float  C3DNavierRegModel::solve_at(const C3DFVector& b, C3DFVectorfield::iterator& v)const
{
	register C3DFVector& vv = *v;
	const C3DFVector old_v = vv;

	const  C3DFVectorfield::iterator vm = v - _M_dxy;
	const  C3DFVectorfield::iterator vp = v + _M_dxy;

	const register int dx = _M_dx;
	const register int mx = - _M_dx;

	const C3DFVector& v0mm = vm[ mx];
	const C3DFVector& vm0m = vm[ -1];
	const C3DFVector& vzm =  vm[0];
	const C3DFVector& vp0m = vm[  1];
	const C3DFVector& v0pm = vm[  dx];


	const C3DFVector& vmm0 = v[ -1 - dx];
	const C3DFVector& vpm0 = v[  1 - dx];

	const C3DFVector& vym = v[mx];
	const C3DFVector& vm00 = v[-1];
	const C3DFVector& vp00 = v[1];
	const C3DFVector& vyp = v[ dx];

	const C3DFVector& vmp0 = v[ -1 + dx];
	const C3DFVector& vpp0 = v[  1 + dx];

	const C3DFVector& vm0p = vp[ -1];
	const C3DFVector& v0mp = vp[ mx];
	const C3DFVector& vzp = *vp;
	const C3DFVector& v0pp = vp[  dx];
	const C3DFVector& vp0p = vp[  1];

	register float rx = (vm00.x + vp00.x) * _M_a_b + b.x;
	register const float hx = vym.x + vyp.x + vzp.x + vzm.x;

	register float ry = (vm00.y + vp00.y) * _M_a_b + b.y;
	register const float hy = vym.y + vyp.y + vzp.y + vzm.y;

	register float rz = (vm00.z + vp00.z) * _M_a_b + b.z;
	register const float hz = vym.z + vyp.z + vzp.z + vzm.z;

	{
		register float ax = vmm0.y + vpp0.y;
		register float ay = vmm0.x + vpp0.x;
		register float az = vm0m.x + vp0p.x;

		ax -= vmp0.y;
		ay -= vmp0.x;
		az -= vm0p.x;

		ax -= vpm0.y;
		ay -= vpm0.x;
		az -= vp0m.x;

		ax += vm0m.z;
		ay += v0mm.z;
		az += v0mm.y;

		ax += vp0p.z;
		ay += v0pp.z;
		az += v0pp.y;

		ax -= vm0p.z;
		ay -= v0mp.z;
		az -= v0mp.y;

		ax -= vp0m.z;
		ay -= v0pm.z;
		az -= v0pm.y;

		rx += _M_b_4 * ax;
		ry += _M_b_4 * ay;
		rz += _M_b_4 * az;

		rx += hx * _M_a;
		ry += hy * _M_a;
		rz += hz * _M_a;

	}

	vv.x = rx;
	rx -= old_v.x;

	vv.y = ry;
	ry -= old_v.y;

	vv.z = rz;
	rz -= old_v.z;

	rx *= rx;
	ry *= ry;
	rz *= rz;
	rx += ry + rz;

	//C3DFVector q = get_q(v);

	//const C3DFVector hmm((( p + q ) - *v) * _M_omega);



#ifdef __POWERPC__
	return rx > 0 ?__fres( __frsqrte(rx) ) : 0.0f;
#else
	return rx > 0 ? sqrtf(rx) : 0.0f;
#endif
}



bool  C3DNavierRegModel::test_kernel()
{

	// assume mu = 1, lambda = 2
	//float a_b = 0.4f;
	//float a   = 0.1f;
	//float b_4 = 0.075;


	C3DBounds size(3,3,3);
	size_t mid_index = 1 + size.x + size.x * size.y;
	_M_dx = size.x;
	_M_dxy = size.x * size.y;

	C3DFVectorfield v(size);

	C3DFVector b(1.0, 2.0, 3.0);

	for (size_t z = 0; z < size.z; ++z)
		for (size_t y = 0; y < size.y; ++y)
			for (size_t x = 0; x < size.x; ++x)
				v(x,y,z) = C3DFVector(y * x, 2 * z * y, 3 * x * z);

	C3DFVectorfield::iterator vi = v.begin() + mid_index;

	const C3DFVector q_test( 0.75, 0.25, 0.5);
	const C3DFVector q_eval = get_q(vi);

	cvdebug() << q_eval << " vs. " << q_test << "\n";
	assert( (q_eval - q_test).norm() < 0.00001);

	// test p eval
	const C3DFVector p_test( 2.0, 4.0f, 6.0f);
	const C3DFVector p_eval = get_p(b, vi);

	cvdebug() << p_eval << " vs. " << p_test << "\n";
	assert( (p_eval - p_test).norm() < 0.00001);

	// test q eval

	C3DFVector test0(1.75, 2.25, 3.5);

	C3DFVector test(2.75, 4.25, 6.5);

	float res = solve_at(b, vi);
	cvdebug() << res << " vs. " << test0.norm() <<"\n";
	cvdebug() << v(1,1,1) << " vs. " << test <<"\n";


#ifdef __POWERPC__
	return (fabs(res - test0.norm()) < 0.1) && (test - v(1,1,1)).norm() < 0.0001;
#else
	return (fabs(res - test0.norm()) < 0.0001) && (test - v(1,1,1)).norm() < 0.0001;
#endif

}

class C3DNavierRegModelPlugin: public C3DRegModelPlugin {
public:
	C3DNavierRegModelPlugin();
	C3DNavierRegModelPlugin::ProductPtr do_create()const;

private:
	bool  do_test() const;
	const string do_get_descr()const;

	float _M_mu;
	float _M_lambda;
	float _M_epsilon;
	int _M_maxiter;
};

C3DNavierRegModelPlugin::C3DNavierRegModelPlugin():
	C3DRegModelPlugin("naviera"),
	_M_mu(1.0),
	_M_lambda(1.0),
	_M_epsilon(0.0001),
	_M_maxiter(40)
{
	typedef CParamList::PParameter PParameter;
	add_parameter("mu", new CFloatParameter(_M_mu, 0.0, numeric_limits<float>::max(),
							   false, "isotropic compliance"));
	add_parameter("lambda", new CFloatParameter(_M_lambda, 0.0, numeric_limits<float>::max(),
							       false, "isotropic compression"));

	add_parameter("epsilon", new CFloatParameter(_M_epsilon, 0.000001, 0.1,
								false, "stopping parameter"));
	add_parameter("iter", new CIntParameter(_M_maxiter, 10, 10000,
							   false, "maximum number of iterations"));
}

C3DNavierRegModelPlugin::ProductPtr C3DNavierRegModelPlugin::do_create()const
{
	return C3DNavierRegModelPlugin::ProductPtr(new C3DNavierRegModel(_M_mu, _M_lambda,
									 _M_maxiter, _M_epsilon));
}

bool  C3DNavierRegModelPlugin::do_test() const
{
	C3DNavierRegModel model(1.0, 2.0, 10, 0.001);

	return model.test_kernel();
}

const string C3DNavierRegModelPlugin::do_get_descr()const
{
	return "navier-stokes based registration model";
}

extern "C"  EXPORT CPluginBase *get_plugin_interface()
{
	return new C3DNavierRegModelPlugin();
}

NS_END

