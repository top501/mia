/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2011 Gert Wollny
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

#include <mia/internal/autotest.hh>

#include <stdexcept>
#include <cmath>

#include <mia/core/fftslopeclassifier.hh>


using namespace std;
using namespace mia;

struct SlopeClassifierFixture {
	typedef struct {
		vector<int> periodic_indices;
		int RV_idx;
		int LV_idx;
		int baseline_idx;
		int perfusion_idx;
		int RV_peak;
		int LV_peak;
	} Result;
	void run(size_t length, size_t components, const float *data, const Result& r, bool mean_removed = false);
	void check_equal(int a, int b, const char *descr); 
};

#define BOOST_CHECK_EQUAL_DESCR( L, R, D )				\
	BOOST_CHECK_WITH_ARGS_IMPL( ::boost::test_tools::tt_detail::equal_impl_frwd(), D, CHECK, CHECK_EQUAL, (L)(R) )


void SlopeClassifierFixture::check_equal(int value, int expect, const char *descr)
{
	if (expect != -2) 
		BOOST_CHECK_EQUAL_DESCR( value, expect, descr ); 
}

void SlopeClassifierFixture::run(size_t length, size_t components, const float *data, const Result& result, bool mean_removed)
{
	CFFTSlopeClassifier::Columns columns( components );

	for (size_t c = 0; c < components; ++c)
		columns[c].resize(length);

	const float *i = data;
	for (size_t r = 0; r < length; ++r)
		for (size_t c = 0; c < components; ++c, ++i)
			columns[c][r]  = *i;

	CFFTSlopeClassifier c(columns, 6.5, mean_removed);
	const auto periodic = c.get_periodic_indices(); 
	check_equal(periodic.size(), result.periodic_indices.size(), "periodic index size"); 
	for (auto ip = periodic.begin(), it = result.periodic_indices.begin(); ip != periodic.end(); 
	     ++ip, ++it)
		check_equal(*ip, *it, "periodic index");

	check_equal(c.get_RV_idx(), result.RV_idx, "RV index");
	
	check_equal(c.get_LV_idx(), result.LV_idx, "LV index");
//	check_equal(c.get_perfusion_idx(), result.perfusion_idx, "perfusion index");
//	check_equal(c.get_baseline_idx(), result.baseline_idx, "baseline index");
	check_equal(c.get_RV_peak(), result.RV_peak, "RV peak");
	check_equal(c.get_LV_peak(), result.LV_peak, "LV peak");
}


BOOST_FIXTURE_TEST_CASE( test_fftclassifier_3_mean_stripped, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 3;
	const float init_real_data[rdlength2 * rdcomponents] = {
		-74.3251,  -40.9479,   -21.252,
		-71.5047,  -41.9889,  -36.5246,
		-69.0681,  -41.6188,  -6.08587,
		-80.708,  -41.2647,   16.8441,
		-82.3026,   -42.437,  -17.2515,
		-82.2451,  -40.9327,  -34.8276,
		-84.9291,  -36.3305,  -42.1706,
		-84.0717,  -27.0905,    4.5987,
		-75.4865,  -3.73599,   40.3824,
		-69.242,   16.4529,   5.26785,
		-68.3827,   38.0306,  -28.9403,
		-61.8668,   52.3811,  -47.1251,
		-64.5289,    48.883,  -46.1324,
		-58.9265,   30.5691,   56.0564,
		-42.4296,    24.498,   52.7858,
		-24.378,   37.3981,   19.5246,
		-9.12802,   41.7224,  -35.0637,
		-3.16756,   30.7365,   32.9862,
		8.67048,   14.8941,   69.7443,
		26.9611,   14.3608,   39.5409,
		33.3179,   17.4204,  -32.7811,
		40.6155,    18.064,  -35.2979,
		52.6239,   10.4639,  -29.4597,
		48.4536,   11.1876,  -44.3112,
		50.1748,   2.64327,  -40.2084,
		41.0998,  -6.94567,   30.7431,
		32.9581,  -7.43376,   64.5546,
		39.4291,  -3.95108,   42.0424,
		41.839,   3.84527,    -35.19,
		40.6105,   3.37204,  -41.5261,
		31.8568, -0.158562,  -7.39317,
		20.9368,   -8.7657,   56.1996,
		23.7326,   -9.1078,   36.1214,
		29.8032,  -6.12039,  -7.84226,
		26.5866,   2.76126,  -45.5263,
		25.7835,   1.57838, -0.981124,
		8.12057,   -3.6592,   57.6185,
		17.8922,  -4.80469,   49.1256,
		19.6035,  -1.10291,   -2.1022,
		26.0325,   3.23042,  -31.8905,
		29.6977,   3.85521,  -33.2491,
		22.5807,  -1.53032,  -29.6737,
		16.0141,  -8.51632,   24.5368,
		8.23435,  -4.71869,   61.2152,
		19.3197,   1.80737,  -17.8008,
		30.0301,   2.90151,  -41.1934,
		22.5158,    1.5144,  -40.8091,
		15.592,  -5.94334,   40.2724,
		11.3537,  -6.30845,   67.1715,
		24.5139,  -4.82413,   32.8879,
		28.4618,    0.5147,  -34.8623,
		31.4814,  -1.56695,   -41.014,
		26.4198,  -2.59699,  -28.1021,
		14.8158,  -9.44573,   55.8294,
		20.8277,  -8.44618,   49.8534,
		32.838,  -5.76385,   7.43059,
		33.3169,  -2.88559,  -32.0714,
		31.5756,  -4.14314,  -44.6744
	};

	Result r;

	r.periodic_indices = {2};
	r.RV_idx = 1;
	r.LV_idx = 0;
	r.baseline_idx = -1;
	r.perfusion_idx = -1;

	r.RV_peak = 11;
	r.LV_peak = 22;

	run( rdlength2, rdcomponents, init_real_data,  r);
}




BOOST_FIXTURE_TEST_CASE( test_fftclassifier_4_mean_stripped_normalized, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 4;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-0.333977,  -3.94754,  -1.34662,  -2.74569,
		-0.209406,  -4.04827,  -2.31203,  -2.62334,
		-0.477033,  -4.00886, -0.388419,  -2.56316,
		-0.169401,  -3.98286,   1.03042,  -3.00964,
		-0.169836,  -4.10093,  -1.10015,  -3.05172,
		-0.185502,  -3.94503,  -2.21191,  -3.02039,
		-0.0815538,    -3.504,   -2.6753,   -3.1157,
		0.0897221,  -2.61899,  0.252251,  -3.12221,
		-0.06843, -0.352342,    2.5015,   -2.8235,
		-0.0690284,   1.59013,  0.318406,  -2.58256,
		-0.000659823,   3.67673,  -1.82641,  -2.52148,
		0.0692103,   5.07058,   -2.9743,   -2.2518,
		0.242558,   4.72481,  -2.91793,  -2.35422,
		0.407645,   2.95963,   3.46175,  -2.19992,
		-0.00952595,   2.38163,   3.28265,  -1.59357,
		0.144536,   3.60837,   1.21912, -0.925331,
		0.1877,   4.03264,  -2.20549, -0.306342,
		-0.129536,   2.98122,   2.06856, -0.133419,
		0.383428,   1.44354,   4.33902,  0.299585,
		-0.00140643,   1.39097,   2.48383,  0.976537,
		0.0852201,   1.67647,  -2.03825,   1.25391,
		0.499373,   1.73304,  -2.21893,    1.5396,
		0.736639,  0.993641,  -1.86373,   1.98636,
		0.619809,   1.06539,  -2.78825,   1.83969,
		0.754675,   0.23491,  -2.53737,   1.90123,
		-0.0786036, -0.666738,   1.93686,   1.50889,
		0.205328, -0.714903,   4.03115,   1.19662,
		-0.0771046, -0.381491,   2.64955,   1.43105,
		-0.253121,  0.372406,  -2.17241,   1.56736,
		0.157942,  0.320057,  -2.59376,   1.53827,
		-0.507316,-0.00539073, -0.426273,   1.17783,
		0.31376, -0.850711,    3.5041,  0.750096,
		0.0833796, -0.889626,   2.27216,   0.84555,
		0.00318218, -0.604985, -0.467946,   1.09118,
		-0.209726,  0.270559,  -2.83128,   1.01539,
		-0.445738,  0.160934,-0.0289602,  0.947642,
		0.153292, -0.349854,   3.59393,  0.275995,
		0.0843008,  -0.46865,   3.07967,  0.627375,
		-0.446612, -0.105501,-0.0927621,  0.706603,
		-0.0540282,  0.310195,  -1.98259,   0.98387,
		0.175263,   0.36261,  -2.07481,   1.12041,
		0.172178,  -0.15763,  -1.85404,  0.853832,
		0.17204, -0.829903,   1.53459,  0.575989,
		0.157068, -0.451139,   3.81749,  0.279283,
		-0.541466,  0.178595,  -1.07161,  0.707241,
		-0.0858782,  0.280737,  -2.56448,   1.14153,
		-0.367375,  0.154032,  -2.52978,   0.85885,
		-0.0138096, -0.572355,   2.52443,  0.553149,
		0.774078, -0.630307,   4.17331,  0.384382,
		-0.19285,  -0.46809,   2.08329,  0.873166,
		-0.438202, 0.0555185,    -2.148,   1.06822,
		0.0183089, -0.157257,  -2.55446,   1.19091,
		-0.319559, -0.251229,  -1.72798,   0.98491,
		0.18585, -0.913834,   3.48585,  0.520034,
		-0.359772, -0.810796,   3.14823,  0.727758,
		-0.163052, -0.568527,  0.500121,   1.18687,
		-0.246912, -0.275222,  -1.98473,   1.25364,
		-0.170257, -0.396369,  -2.78126,   1.20319
	};

	Result r;

	r.periodic_indices = {2};
	r.RV_idx = 1;
	r.LV_idx = 3;
	r.baseline_idx = 0;
	r.perfusion_idx = -1;

	r.RV_peak = 11;
	r.LV_peak = 22;

	run( rdlength2, rdcomponents, init_real_data5,  r);
}

BOOST_FIXTURE_TEST_CASE( test_fftclassifier_5_mean_stripped_normalized, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 5;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		 -0.337928,   -4.3535,   -2.8777,  -5.81013,    -2.973,
		 -0.503076,  -4.02309,    -2.739, -0.822572,  -3.26204,
		 -0.51522,  -3.98719,  -2.79981,   1.84109,  -3.20777,
		 -0.250471,  -3.97582,  -2.90742,   2.02425,  -3.23826,
		 -0.375565,  -3.98267,  -2.97203,   2.06275,  -3.23519,
		 -0.471721,  -4.04352,   -2.8741,  -1.04509,  -3.20101,
		 -0.0341674,  -4.35104,  -3.25516,  -6.13801,  -2.92641,
		 -0.521398,  -2.14759,  -2.84955,  -1.60957,  -3.34932,
		 -0.374229,   5.04918,  -3.13471,   1.20525,  -3.46378,
		 -0.129993,   7.97761,   -3.1547,   1.39125,  -3.46901,
		 -0.075652,    8.6514,  -3.14835,   1.84946,  -3.34062,
		 0.0399395,   8.28726,   -3.2423,   2.02153,  -3.23943,
		 -0.219216,   7.38352,  -2.73647,   -3.6345,  -2.61829,
		 0.0896999,    5.6293,  -3.87446,  0.978247, -0.573539,
		 0.0679431,   4.52537,  -4.02871,   2.15678,   1.69087,
		 -0.00621625,   3.17359,  -3.87094,  -1.04762,   3.83384,
		 0.146572,   1.02402,  -2.74141,   -6.5762,   4.55864,
		 0.119675,  0.704314,  -2.88794,  0.979293,   7.22177,
		 0.211168, -0.344782,  -1.29941,   2.31742,   7.52901,
		 0.483478,  -1.03504, -0.679071,   2.32557,   6.60499,
		 0.0523503,  -1.77977, 0.0474983,  -5.89342,   4.84741,
		 0.317688,  -1.43475,  0.777731,  0.913827,    4.9098,
		 0.359736,  -1.33758,   1.73883,    2.7586,   3.40621,
		 0.404177,  -1.60613,   2.02613,   2.11121,   2.53109,
		 0.160111,  -1.43021,   2.64497,   1.59757,   1.04984,
		 0.33735, -0.777487,   2.48394,   2.38534,  0.189705,
		 0.288014, -0.635124,   2.42826,   2.82347,-0.0477628,
		 0.398939, -0.430965,   2.26612,   2.56165, -0.375392,
		 -0.15057, -0.212497,   2.19441,  -2.77743, -0.567252,
		 0.0587856,  0.174651,   2.25864,   0.39981, -0.675114,
		 0.183049,  0.440661,   1.90372,   2.62729, -0.547272,
		 0.326591,  0.396136,    1.5902,   2.40708,  -0.58087,
		 0.20704,  0.447888,   1.54211,   2.69796, -0.336021,
		 0.23325,  0.239261,   1.37297,   1.85263, -0.297511,
		 -0.102197, -0.192818,   1.23322,  -5.51786, -0.211432,
		 0.0775201, -0.364814,   1.35887,   1.13038,  0.106161,
		 0.105982, -0.383997,   1.26686,   2.40133,   0.41867,
		 -0.105662, -0.646301,   1.15585,  -5.04566,  0.059048,
		 -0.250138, -0.408086,   1.47335, -0.661364,  0.447294,
		 0.194694, -0.482292,    1.1583,   1.94749,  0.387808,
		 0.0859516, -0.402189,   1.30924,   2.18185,  0.384867,
		 -0.0637214, -0.814235,   1.31398,  -5.29327,  0.054274,
		 -0.213735, -0.663673,   1.57202, -0.049137,  0.172588,
		 0.152096, -0.767067,   1.36046,   1.97671, 0.0103516,
		 0.0638535, -0.636486,   1.42322,   2.22867, 0.0588318,
		 -0.248255, -0.610515,   1.56397,  -3.55545, -0.339614,
		 -0.0886441, -0.556308,    1.5021,   1.20774,-0.0998046,
		 0.123961, -0.630348,   1.35904,   2.24357, -0.231888,
		 0.0355604,  -0.59308,   1.40978,   2.19708,  -0.18366,
		 -0.0504062, -0.530053,   1.36603, -0.292082, -0.327193,
		 -0.215511, -0.337022,   1.61815,  -4.88995, -0.431454,
		 0.143322, -0.449969,   1.34705,   1.82765, -0.358579,
		 0.175857,  -0.50345,   1.32077,   2.58816, -0.397978,
		 0.210724, -0.391277,   1.31044,   2.31172, -0.440532,
		 -0.256261, -0.423272,   1.32386,  -4.82235, -0.624508,
		 -0.130833, -0.464304,   1.41111,  -4.56921, -0.573838,
		 -0.0145948, -0.454401,   1.33357,   2.02028, -0.277873,
		 -0.149502, -0.509432,   1.30646,  -2.50106, -0.449868
	};

	Result r;

	r.periodic_indices = {3};
	r.RV_idx = 1;
	r.LV_idx = 4;
	r.baseline_idx = 0;
	r.perfusion_idx = 2;

	r.RV_peak = 10;
	r.LV_peak = 18;

	run( rdlength2, rdcomponents, init_real_data5,  r);
}

