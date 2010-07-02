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

#include <mia/internal/autotest.hh>

#include <stdexcept>
#include <cmath>

#include <mia/core/slopeclassifier.hh>


using namespace std;
using namespace mia;

struct SlopeClassifierFixture {
	typedef struct {
		int periodic_idx;
		int RV_idx;
		int LV_idx;
		int baseline_idx;
		int perfusion_idx;
		int RV_peak;
		int LV_peak;
	} Result;
	void run(size_t length, size_t components, const float *data, const Result& r, bool mean_removed = false);
};

void SlopeClassifierFixture::run(size_t length, size_t components, const float *data, const Result& result, bool mean_removed)
{
	CSlopeClassifier::Columns columns( components );

	for (size_t c = 0; c < components; ++c)
		columns[c].resize(length);

	const float *i = data;
	for (size_t r = 0; r < length; ++r)
		for (size_t c = 0; c < components; ++c, ++i)
			columns[c][r]  = *i;

	CSlopeClassifier c(columns, mean_removed);

	BOOST_CHECK_EQUAL(c.get_periodic_idx(), result.periodic_idx);
	BOOST_CHECK_EQUAL(c.get_RV_idx(), result.RV_idx);
	BOOST_CHECK_EQUAL(c.get_LV_idx(), result.LV_idx);
	BOOST_CHECK_EQUAL(c.get_perfusion_idx(), result.perfusion_idx);
	BOOST_CHECK_EQUAL(c.get_baseline_idx(), result.baseline_idx);
	BOOST_CHECK_EQUAL(c.get_RV_peak(), result.RV_peak);
	BOOST_CHECK_EQUAL(c.get_LV_peak(), result.LV_peak);
}

BOOST_FIXTURE_TEST_CASE( test_classifier4, SlopeClassifierFixture )
{
	const size_t length = 10;
	const size_t components = 4;

	const float init_Mix[length * components ]  = {
		-40, 10,  1, -1,
		-38,  5, 12, -4,
		-39,  0, 13, -4,
		-38, -5,  9, -5,
		-41,-10,  1, -14,
		-40, -5,  0, -16,
		-41,  0, -5, -16,
		-40,  5, -6, -17,
		-40, 10, -3, -14,
		-40,  5, -2, -14
	};
	const Result result = {1, 2, 3, 0, -1, 2, 7};
	run(length, components, init_Mix, result);
}

BOOST_FIXTURE_TEST_CASE( test_classifier3, SlopeClassifierFixture )
{
	const size_t length = 10;
	const size_t components = 3;

	const float init_Mix[length * components ]  = {
		-40,  1, -1,
		-38, 12, -4,
		-39, 13, -4,
		-38,  9, -5,
		-41,  1, -14,
		-40,  0, -16,
		-41, -5, -16,
		-40, -6, -17,
		-40, -3, -14,
		-40, -2, -14
	};
	const Result result = {-1, 1, 2, 0, -1, 2, 7};
	run(length, components, init_Mix, result);
}


BOOST_AUTO_TEST_CASE( test_max_length)
{
	const size_t length = 10;
	const size_t components = 3;

	const float init_Mix[length * components ]  = {
		-40,  1, -1,
		-38, 12, -4,
		-39, 13, -4,
		-38,  9, -5,
		-41,  1, -14,
		-40,  0, -16,
		-41, -5, -16,
		-40, -6, -17,
		-40, -3, -14,
		-40, -2, -14
	};
	CSlopeClassifier::Columns columns( components );

	for (size_t c = 0; c < components; ++c)
		columns[c].resize(length);

	const float *i = init_Mix;
	for (size_t r = 0; r < length; ++r)
		for (size_t c = 0; c < components; ++c, ++i)
			columns[c][r]  = *i;

	CSlopeClassifier c(columns, false);
	BOOST_CHECK_CLOSE(c.get_max_slope_length_diff(), 37.148876f  - 23.44436, 0.01);
}

#if 0 // check this test
BOOST_FIXTURE_TEST_CASE( test_classifier_rd3 , FixtureClassifier )
{
	const size_t length = 58;
	const size_t components = 4;
	const float init_real_data3[length * components] = {
		-0.709406,    41.6813,   0.715418,   -1.66311,
		-0.190115,    41.2402, -0.00104648,   0.668675,
		0.187925,    39.9257,  -0.731897,    1.73036,
		-1.06532,    41.1944,    1.13608,  -0.925986,
		-1.27219,    41.3628,    1.38234,    -2.1344,
		-1.46881,    41.2549,   0.995326,   -2.11587,
		-1.67321,    41.2693,   -1.23951,   -2.39431,
		-1.23218,    41.1528,   -28.8613,    1.05743,
		-0.563881,    40.7701,   -32.5977,    2.46865,
		-1.37908,    39.7973,   -31.9903,   0.125437,
		-1.32959,    39.7044,   -32.9785,   0.545082,
		0.355864,    37.9947,   -34.5978,    3.56483,
		0.863566,    39.8132,    -33.576,    2.09411,
		7.08637,    39.2091,   -34.7778,    2.02566,
		16.7158,    36.5244,   -34.7715,    4.27531,
		21.0478,    39.7182,   -31.2344,  -0.458803,
		22.9082,    40.2202,   -32.5895,   -0.47484,
		25.604,    36.4984,   -33.2009,    3.38954,
		27.3598,    38.9371,   -31.7071,    2.47291,
		27.8014,     39.618,   -29.2439,  -0.765197,
		28.7842,    40.1757,   -28.0046,   -1.81104,
		29.9045,    38.2662,   -26.5018,   0.833308,
		31.1522,    39.1932,   -21.3177,   -1.78874,
		30.8608,    40.4043,   -16.0593,   -6.69739,
		30.9448,    40.8722,   -15.0146,   -8.31921,
		31.8187,    39.8063,    -13.467,   -6.41122,
		31.9662,    40.6696,   -12.7559,   -9.83939,
		31.5602,    40.4217,   -12.6256,   -12.5589,
		31.3201,    40.8733,   -14.3942,    -14.308,
		31.162,    40.4963,   -15.3076,   -15.0412,
		30.8288,    40.5002,   -17.2765,   -16.3521,
		30.4436,    40.5834,   -18.3404,   -17.4949,
		30.0737,    40.9814,   -20.5287,   -18.3034,
		30.2591,     39.805,   -24.4116,   -14.5313,
		29.1536,    38.9329,   -27.1657,   -10.8963,
		28.3372,    39.4898,   -28.5318,   -15.6629,
		27.2092,    40.2782,   -29.3128,   -19.7535,
		25.9003,    40.0148,   -29.3997,   -20.8893,
		25.0539,    40.1441,   -29.8228,   -22.0298,
		23.8383,    40.3182,    -29.813,   -22.7571,
		22.3635,    40.1396,   -30.7937,    -21.342,
		21.7182,    39.4404,   -30.6885,   -19.9335,
		21.3952,    40.4161,   -30.0973,   -23.7699,
		21.4003,    39.9727,   -29.9895,   -23.6375,
		21.9594,    40.1878,   -30.1205,   -24.1485,
		22.1199,    39.7348,   -29.9995,   -23.8477,
		22.8905,    39.5587,   -31.0807,   -22.7587,
		23.9454,    38.2598,    -30.488,    -15.568,
		24.1567,    39.9831,   -29.0537,   -23.2369,
		23.9358,    39.8715,   -28.2337,   -23.8221,
		24.2844,    40.3854,   -28.3163,   -24.4975,
		24.1504,    39.7844,   -26.8665,   -24.3716,
		24.2032,    40.1522,   -26.8313,   -24.6314,
		24.6502,      39.08,   -26.5577,    -21.218,
		25.6241,    38.1045,   -26.7664,   -15.7689,
		24.7975,    39.5507,   -23.9245,   -22.0648,
		24.2107,    40.2695,   -23.4283,   -24.4966,
		23.7971,    40.0007,   -22.8042,   -24.7005
	};

	Result r;

	r.periodic_idx = 3;
	r.RV_idx = 2;
	r.LV_idx = 0;
	r.baseline_idx = 1;
	r.perfusion_idx = -1;

	r.RV_peak = 13;
	r.LV_peak = 26;

	run( length, components, init_real_data3,  r);
}
#endif

BOOST_FIXTURE_TEST_CASE( test_classifier_rd5 , SlopeClassifierFixture
 )
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 4;
	const float init_real_data5[rdlength2 * rdcomponents] = {

		1.93107,    33.9098,    1.24544,  -0.375548,
		3.3634,    34.0585,    1.15972,  -0.618234,
		4.36437,    34.1363,     1.3174,  -0.678597,
		5.33475,     33.696,    1.20767,  -0.624952,
		6.93798,    33.7545,   0.970156,  -0.681925,
		7.37977,    33.3261,   0.951123,  -0.643085,
		1.95015,    33.8977,    1.11698,   0.345177,
		-0.686319,     33.352,    1.04379,   0.670644,
		1.40583,    34.0626,    1.22452,  0.0572739,
		5.17559,    33.1467,    1.10063,  -0.207255,
		8.35277,    32.9519,   0.185485, -0.0723758,
		10.5031,    32.1477,   -1.89872,  0.0633403,
		12.4011,    32.1607,   -5.40913,   0.383761,
		12.9279,    31.8824,   -8.80358,   0.479939,
		13.7029,    31.9414,   -10.6424,   0.751751,
		13.5917,    31.4926,   -11.1955,   0.904017,
		9.76603,    32.6163,   -11.7688,   0.972901,
		2.27079,     33.668,   -10.9162,    2.06139,
		1.3043,     34.552,   -9.54846,    2.49135,
		2.7412,    33.9169,   -8.23394,    2.59609,
		6.70452,    33.2824,   -7.75694,    3.58373,
		10.4073,    32.0978,   -7.90973,    5.22125,
		12.5893,     31.756,   -7.66469,    6.73096,
		13.1485,    31.1779,   -6.77166,    7.59789,
		12.2005,    31.4045,   -6.34094,    8.77374,
		6.12324,    32.4497,   -5.45471,    9.61676,
		1.01432,    33.3813,   -4.33458,    10.1126,
		0.679071,    33.6297,   -3.53136,    9.76761,
		6.01815,    33.1053,   -3.31658,     9.6861,
		10.5366,    31.7371,    -3.5263,    9.42801,
		12.8674,      31.49,   -3.74297,    9.34467,
		12.5499,    31.0438,   -3.47572,    8.99039,
		14.3069,    31.4436,   -3.29667,    8.63641,
		11.3908,    31.3068,   -2.67206,    8.53356,
		3.76099,    33.0387,   -2.10219,    8.70258,
		0.174254,     32.953,   -1.37855,    7.79107,
		-0.172855,    34.0223,   -1.15293,    7.46854,
		3.72706,    33.5131,    -1.0569,    6.96759,
		9.0858,    32.6775,   -1.47233,     7.0273,
		12.6362,    31.7726,   -2.00357,    6.86717,
		14.3969,    31.8277,   -2.39181,    6.79701,
		14.8163,     31.228,   -2.37096,    6.54723,
		12.0622,    32.0688,   -2.22653,    6.75613,
		4.46751,     33.217,   -1.96504,    6.66156,
		1.65239,    34.4252,   -1.83616,    6.67476,
		4.9307,    33.8333,   -1.25837,    6.13548,
		10.2155,    32.8136,   -1.76934,    6.28366,
		13.0799,    32.1741,   -2.06523,     6.1876,
		14.1879,    32.2419,    -2.3195,    6.29476,
		14.3584,    31.9697,   -2.16251,    6.27448,
		14.6951,    32.0383,   -2.26453,     6.3687,
		14.7204,    31.8767,   -2.03409,    6.29586,
		11.3993,    32.5922,   -1.76237,    6.47273,
		4.65293,    33.6051,    -1.3557,      6.587,
		6.09165,    34.1937,   -1.07023,    6.35567,
		11.413,     32.809,   -1.28584,     6.4165,
		13.8185,    32.5431,   -1.54205,     6.4817,
		14.7547,    32.0997,   -1.69328,    6.42059
	};

	Result r;

	r.periodic_idx = 0;
	r.RV_idx = 2;
	r.LV_idx = 3;
	r.baseline_idx = 1;
	r.perfusion_idx = -1;

	r.RV_peak = 16;
	r.LV_peak = 26;

	run( rdlength2, rdcomponents, init_real_data5,  r);
}


BOOST_FIXTURE_TEST_CASE( test_classifier_mean_removed_3_1 , SlopeClassifierFixture
 )
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 3;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-148.65,   -81.8958,    -42.504,
		-143.009,   -83.9778,   -73.0492,
		-138.136,   -83.2376,   -12.1717,
		-161.416,   -82.5295,    33.6882,
		-164.605,   -84.8741,    -34.503,
		-164.49,   -81.8653,   -69.6552,
		-169.858,    -72.661,   -84.3412,
		-168.143,    -54.181,    9.19739,
		-150.973,   -7.47198,    80.7648,
		-138.484,    32.9059,    10.5357,
		-136.765,    76.0613,   -57.8805,
		-123.734,    104.762,   -94.2503,
		-129.058,    97.7661,   -92.2648,
		-117.853,    61.1382,    112.113,
		-84.8592,     48.996,    105.572,
		-48.756,    74.7962,    39.0493,
		-18.256,    83.4449,   -70.1274,
		-6.33511,    61.4729,    65.9724,
		17.341,    29.7881,    139.489,
		53.9222,    28.7216,    79.0818,
		66.6357,    34.8407,   -65.5622,
		81.2309,     36.128,   -70.5958,
		105.248,    20.9279,   -58.9194,
		96.9073,    22.3752,   -88.6224,
		100.35,    5.28655,   -80.4167,
		82.1997,   -13.8913,    61.4863,
		65.9162,   -14.8675,    129.109,
		78.8582,   -7.90216,    84.0848,
		83.6781,    7.69054,     -70.38,
		81.221,    6.74408,   -83.0522,
		63.7136,  -0.317124,   -14.7863,
		41.8737,   -17.5314,    112.399,
		47.4652,   -18.2156,    72.2429,
		59.6065,   -12.2408,   -15.6845,
		53.1731,    5.52251,   -91.0527,
		51.567,    3.15675,   -1.96225,
		16.2411,    -7.3184,    115.237,
		35.7844,   -9.60939,    98.2512,
		39.2069,   -2.20582,   -4.20441,
		52.0651,    6.46084,    -63.781,
		59.3954,    7.71042,   -66.4981,
		45.1613,   -3.06063,   -59.3474,
		32.0282,   -17.0326,    49.0736,
		16.4687,   -9.43738,     122.43,
		38.6395,    3.61474,   -35.6015,
		60.0602,    5.80303,   -82.3868,
		45.0315,    3.02881,   -81.6182,
		31.1841,   -11.8867,    80.5449,
		22.7073,   -12.6169,    134.343,
		49.0278,   -9.64825,    65.7758,
		56.9236,     1.0294,   -69.7246,
		62.9628,    -3.1339,   -82.0279,
		52.8396,   -5.19398,   -56.2043,
		29.6315,   -18.8915,    111.659,
		41.6553,   -16.8924,    99.7068,
		65.6759,   -11.5277,    14.8612,
		66.6338,   -5.77118,   -64.1429,
		63.1511,   -8.28628,   -89.3487
	};

	Result r;

	r.periodic_idx = 2;
	r.RV_idx = 1;
	r.LV_idx = 0;
	r.baseline_idx = -1;
	r.perfusion_idx = -1;

	r.RV_peak = 11;
	r.LV_peak = 22;

	run( rdlength2, rdcomponents, init_real_data5,  r, true);
}


BOOST_FIXTURE_TEST_CASE( test_classifier_mean_removed_4_1 , SlopeClassifierFixture
 )
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 4;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-81.886,   -47.4317,   -147.859,   -4.74962,
		-85.1003,   -76.9524,   -140.559,   -34.3173,
		-82.3675,   -17.2738,    -138.64,    19.4746,
		-81.0973,    30.4024,   -163.177,    41.1188,
		-83.6432,   -42.6068,   -165.271,    21.8034,
		-83.462,   -71.9334,   -161.627,   -43.1932,
		-74.2547,    -87.722,   -166.894,   -46.9515,
		-53.6198,    6.87133,   -168.863,    16.5016,
		-7.21572,    85.5739,   -152.571,    23.9291,
		35.5871,    2.90998,   -142.128,    54.6223,
		77.0401,   -65.3077,    -137.99,    2.53593,
		103.116,   -96.2394,   -121.797,   -60.0989,
		96.3613,   -94.9263,   -127.372,   -54.3945,
		57.3153,    132.619,     -115.8,   -54.8901,
		47.2919,    119.172,   -84.8842,   -13.0908,
		77.5525,     34.302,   -52.9469,    60.1813,
		82.2621,   -71.3278,   -16.7315,   -44.2277,
		62.344,    69.1367,   -8.59861,    29.0301,
		26.0745,    162.157,    19.4736,   -44.8892,
		30.1154,    81.6543,    51.3016,    44.2685,
		34.8531,   -70.1108,    67.3475,    -16.534,
		34.4295,   -70.2721,    83.9102,   -52.1787,
		19.0797,   -57.2581,    108.112,   -51.7329,
		19.7246,    -86.901,    100.999,   -74.6583,
		2.18348,   -76.8312,     104.97,   -80.9682,
		-13.7193,    66.0768,    81.5261,    18.0439,
		-17.5996,    147.762,    67.5463,   -25.0535,
		-6.28225,    86.1163,    76.2558,    51.9501,
		8.37299,   -77.5339,    83.9561,   -2.65308,
		5.1483,   -84.3216,    84.1634,      -51.3,
		1.40462,   -20.8738,    62.1594,    31.2694,
		-19.2758,    126.595,    42.5944,   -8.78622,
		-16.2606,     72.068,    44.6484,    56.6151,
		-9.3044,   -25.6658,    56.8085,    56.1685,
		4.90649,   -96.0714,    55.0846,    -33.302,
		5.33566,   -8.44518,    49.2943,    43.1815,
		-9.73305,     131.67,     17.527,   -22.1757,
		-8.73513,     103.44,    33.7347,    40.2755,
		1.76467,   -16.4098,    35.0057,    79.0301,
		6.42185,   -68.3896,    52.9879,   -15.5851,
		7.48548,     -70.71,    60.5646,   -20.0152,
		-2.90538,   -64.3238,    45.8694,   -10.1438,
		-16.4774,    51.2496,    30.9979,    23.1667,
		-11.8469,    139.396,    17.6741,   -20.3312,
		6.70095,   -47.5512,    35.7626,    53.8275,
		4.89031,   -85.7853,    62.2017,   -37.3365,
		2.88906,   -87.3994,       46.3,   -21.4442,
		-11.3685,    85.3687,    29.7559,    29.2429,
		-15.6373,    154.099,    24.4711,   -29.7621,
		-6.50375,    61.5774,    44.9262,    78.6137,
		2.11266,   -78.2372,    56.7405,    5.94978,
		-3.29693,   -87.7476,    64.3494,   -21.6793,
		-4.04421,   -63.9324,     52.449,     10.688,
		-20.1634,    124.291,    29.8244,   0.636895,
		-14.5625,    100.552,    38.0559,    70.2889,
		-7.25056,    3.26523,    61.0272,    89.9982,
		-5.78163,   -68.9206,    67.6546,   -14.4285,
		-9.36843,   -92.9114,    65.6788,   -41.5411
	};

	Result r;

	r.periodic_idx = 1;
	r.RV_idx = 0;
	r.LV_idx = 2;
	r.baseline_idx = -1;
	r.perfusion_idx = 3;

	r.RV_peak = 11;
	r.LV_peak = 22;

	run( rdlength2, rdcomponents, init_real_data5,  r, true);
}

BOOST_FIXTURE_TEST_CASE( test_classifier_mean_removed_4_2 , SlopeClassifierFixture
 )
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 4;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-1.21697,    -12.889,    -8.8137,   -11.2628,
		7.26992,   -10.1643,   -7.71096,   -10.2105,
		3.36918,   -9.61763,    -7.5347,   -10.2059,
		-5.31023,   -10.8776,   -8.19432,   -10.7311,
		-3.61536,    -11.757,   -8.53927,    -10.786,
		7.43997,   -9.72578,   -7.70598,   -9.37773,
		3.96546,   -8.38908,   -7.17976,    -3.5115,
		-6.22638,   -9.78003,   -8.38516,    4.31378,
		-2.22855,   -11.1856,   -8.76427,    9.20779,
		8.28368,   -9.18611,   -8.94622,      13.12,
		1.34157,   -9.57345,   -7.77406,    15.3541,
		-5.89887,   -10.5877,   -7.39647,    13.6015,
		3.33047,   -10.4561,   -5.99676,    13.6439,
		11.51,   -8.87868,    -2.7022,    13.4846,
		-0.201523,    -9.3964,    4.60773,    12.5756,
		-6.46308,   -9.74944,    8.06126,    8.36199,
		2.09767,   -9.91261,    11.2294,    1.78679,
		9.98559,   -7.19383,    11.3783,    2.69818,
		-2.20947,   -6.43649,    11.8703,    2.25018,
		-6.46433,   -6.83957,    11.0092,  -0.542518,
		5.37192,   -5.87149,    12.3547,   -5.41251,
		3.82535,   -3.10461,     12.523,   -4.56304,
		-4.9578,   -1.60572,    10.4858,   -3.90724,
		-6.78449,   -1.70081,    8.48264,   -6.42681,
		6.01595,   0.314861,    8.62154,   -8.89845,
		6.47053,    3.85449,    7.31228,   -7.60484,
		-4.00454,    5.13422,    5.03714,   -5.66759,
		-7.35794,    4.92735,    2.79475,   -6.39917,
		0.160344,    4.97459,      2.087,   -7.57198,
		8.01644,    7.57877,     1.4296,   -5.21079,
		1.70258,    8.99215,   0.095665,   -2.86466,
		-5.86428,     8.2516,   -1.42558,   -2.07345,
		-7.57355,    7.06447,   -2.49541,  -0.815717,
		1.07946,     6.2852,   -3.01762,  -0.461196,
		10.2077,    7.14585,   -3.01935,    1.50698,
		2.50873,    9.08613,   -2.76672,    2.46428,
		-4.11772,    7.88459,   -2.81467,    2.72878,
		-7.30043,    6.71874,    -2.8543,    2.09026,
		-7.81432,    5.34063,   -2.81373,     2.2376,
		1.21732,     4.6602,   -2.02043,    1.28263,
		10.6291,    5.97178,    -0.9351,    1.94118,
		4.20717,    7.56745,   0.448437,    1.96171,
		-5.38515,    7.36575,   0.340722,    2.19751,
		-8.16073,    6.01278,  -0.015374,    1.08096,
		-0.268398,    4.54413,   0.434688,  -0.314944,
		10.1471,    5.79406,   0.677009,  -0.407828,
		2.88709,    7.32053,    1.45917,   0.511453,
		-6.25489,    6.58108,   0.717902,   0.687246,
		-7.81785,    5.45234,   0.138807,   0.325375,
		-5.39009,    4.31263,  -0.144036,  -0.700289,
		7.36143,    4.95953,   0.127016,   -1.10937,
		-0.0970818,    6.92727,   0.371997,   0.473863,
		-6.81938,    5.74919,  -0.475212,    1.05183,
		-8.47326,    4.63227,   -1.00542,  0.0984377,
		2.4529,    4.64438,  -0.785798,  0.0570382,
		9.60226,    6.35436,   -0.32421,   0.355114,
		-0.548125,    7.12886,  -0.522247,     1.8589,
		-7.63212,    5.34681,     -1.017,     1.7284
	};

	Result r;

	r.periodic_idx = 0;
	r.RV_idx = 3;
	r.LV_idx = 2;
	r.baseline_idx = -1;
	r.perfusion_idx = 1;

	r.RV_peak = 10;
	r.LV_peak = 21;

	run( rdlength2, rdcomponents, init_real_data5,  r, true);
}


BOOST_FIXTURE_TEST_CASE( test_classifier_5_1 , SlopeClassifierFixture
 )
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 5;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		   -6.6426,    82.3264,    245.254,   -4.89471,    3.39039,
  -2.40795,    10.5167,    244.893,   -3.15199,   -6.18647,
  0.753241,   -48.9448,    239.122,  -0.886501,   -1.66189,
  -10.3483,    47.2591,     243.57,   -11.3251,    4.69179,
  -11.9519,    83.2187,    243.443,   -11.1869,    10.0508,
  -13.8011,    90.3283,    242.511,   -7.00366,    7.38971,
  -15.6851,    96.0849,    242.369,    13.7635,    9.47757,
   -12.529,   -4.62883,    244.041,    259.698,   -7.02043,
  -6.52561,   -39.3654,    242.701,    291.748,   -15.9926,
  -13.2791,    89.8503,     232.43,    296.191,   -25.9798,
  -12.9328,    77.9299,    232.222,    304.452,    -28.054,
   1.98158,   -71.6903,    227.142,    308.203,   -21.6628,
   7.47355,     7.9125,    235.228,    304.761,   -27.2974,
   67.0661,   -5.42986,    232.115,    314.756,   -23.7571,
   158.187,   -168.416,    221.942,    302.289,     -4.159,
   201.781,    92.5124,    231.958,    289.712,   -25.1562,
   219.548,    80.9292,    235.353,    301.068,   -21.4786,
    243.52,   -168.205,    221.993,    287.617,    5.97257,
   260.968,   -70.9723,    232.984,    281.715,   -13.9675,
   266.492,     79.305,     231.98,    270.332,   -18.2121,
   276.148,    93.6682,    234.998,    259.577,   -8.11513,
   285.692,   -59.0445,    229.019,    234.276,    4.98165,
   298.405,    7.45736,    232.663,    191.016,    20.2313,
   297.096,     144.28,    235.767,    151.776,    45.1756,
   298.235,    159.308,     238.34,     142.49,     63.649,
   305.646,    30.0134,    236.633,    118.285,    79.6582,
   307.942,    95.5257,    239.978,    115.329,    107.154,
   304.858,    166.063,    236.351,    118.673,    122.622,
   302.966,    191.483,    238.372,    135.946,    139.428,
   301.614,    199.283,    235.954,    144.569,    147.363,
   298.684,    209.153,    235.814,    162.624,    163.065,
    295.22,     217.76,    236.185,    172.385,    176.808,
   291.786,    214.888,     238.77,    191.594,    189.642,
   292.133,    53.2619,    237.109,    215.363,    189.706,
   280.287,   -77.0211,    236.113,    231.413,    181.538,
   273.808,    35.2665,    236.048,     250.75,    212.683,
    264.23,    147.917,    237.225,    265.153,    234.047,
   252.085,     189.14,    234.303,    268.866,    236.809,
   244.249,    207.317,    234.598,    273.593,    247.418,
   232.737,     211.94,    235.611,    273.411,    256.783,
    217.92,    123.711,    237.633,    275.481,    266.739,
   211.228,    67.9108,    235.336,    270.544,    265.417,
   209.401,    195.926,    237.019,    273.904,    277.677,
   209.459,    200.566,    234.176,    273.457,      274.1,
   214.921,    205.324,    235.375,    274.747,    279.753,
   216.405,    202.916,    232.713,    273.669,     276.09,
   223.236,    129.179,    234.251,    277.786,    285.154,
   231.125,   -78.6424,    233.058,     258.71,    251.215,
   235.514,    142.934,    236.402,    260.145,    287.137,
    233.67,    177.237,     234.56,    255.306,    283.993,
   237.158,    185.946,     237.42,    256.362,    290.798,
   235.855,    184.765,    233.905,    243.138,    289.347,
   236.333,    172.041,     236.64,    241.498,    297.499,
   239.462,    52.7296,     234.16,    230.745,    288.371,
    247.14,   -99.6712,    233.093,    222.756,    260.853,
    241.08,    67.3951,    236.648,    207.379,    295.747,
   236.262,    149.147,    238.293,    208.432,    303.448,
   232.418,    165.352,    236.129,    204.042,    300.972
	};

	Result r;

	r.periodic_idx = 1;
	r.RV_idx = 3;
	r.LV_idx = 0;
	r.baseline_idx = 2;
	r.perfusion_idx = 4;

	r.RV_peak = 13;
	r.LV_peak = 26;

	run( rdlength2, rdcomponents, init_real_data5,  r, true);
}

BOOST_FIXTURE_TEST_CASE( test_classifier_mean_removed_4_3 , SlopeClassifierFixture
 )
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 4;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-128.389,   -199.836,   -16.1937,   -218.145,
		-135.116,   -195.022,   -87.9185,   -216.362,
		-128.205,     -191.2,   -147.619,    -213.87,
		-125.653,   -203.223,   -51.3636,   -224.604,
		-121.757,   -205.097,   -15.3479,   -224.437,
		-124.699,   -206.981,   -8.66392,   -220.136,
		-122.777,   -208.903,   -2.84058,   -199.103,
		-134.324,   -205.048,   -103.595,    49.5847,
		-141.772,   -198.758,   -138.922,    81.9997,
		-157.253,   -206.202,   -12.7612,    87.2742,
		-158.775,   -205.773,   -24.8978,    95.5976,
		-146.188,   -189.509,   -174.063,    99.3408,
		-155.039,   -184.877,   -93.8502,    95.5644,
		-151.064,   -124.895,   -106.301,    105.684,
		-124.949,   -31.9216,   -267.411,    93.0144,
		-156.981,    9.53119,    -6.1546,    80.3441,
		-152.849,    27.3343,   -16.3701,    91.6064,
		-115.384,    53.7401,   -263.972,    78.1632,
		-139.43,    70.1394,   -166.043,    71.8973,
		-150.154,     74.616,   -16.3162,    60.8235,
		-140.763,     84.117,  -0.426917,    49.7884,
		-121.555,    95.0744,   -152.146,    24.2331,
		-109.452,    107.278,   -83.3664,    -19.531,
		-90.4076,    104.859,    55.2591,   -59.2512,
		-72.6481,    105.842,    72.1472,   -68.7858,
		-51.5505,    114.392,   -55.0495,   -93.3007,
		-26.9038,    116.138,    12.9994,   -96.4141,
		-14.4329,     112.65,    83.6974,   -92.7745,
		1.38472,    110.516,    110.474,   -75.4245,
		8.96665,    109.198,     118.45,   -66.5657,
		24.2784,    106.228,    129.411,   -48.2815,
		37.6966,      102.7,    138.945,   -38.4449,
		50.7571,    99.2164,     137.32,   -19.1813,
		57.5076,    100.907,   -23.4338,    4.85686,
		55.0372,     90.009,   -154.613,     20.891,
		81.4062,     82.744,   -40.3362,    40.6611,
		98.207,    72.2436,    73.0846,    55.2083,
		99.3479,    59.8223,    113.317,    59.1176,
		109.246,     51.834,    132.014,    63.8939,
		118.522,    40.2234,    137.012,    63.6127,
		132.152,    26.0268,    50.1687,    65.5716,
		133.198,    19.8149,   -6.14248,    60.6162,
		140.391,    16.9047,    121.644,    63.8774,
		136.621,    17.0157,    125.468,    63.5873,
		142.12,    22.4234,    130.785,    64.7848,
		138.554,      24.01,     127.64,    63.8283,
		150.602,    31.4297,    55.5924,    67.8728,
		125.282,    40.9213,    -153.75,    48.4277,
		152.09,    43.5582,    69.6205,    49.8283,
		147.569,     41.477,    102.945,    45.0193,
		154.107,    44.8104,    112.414,    45.8669,
		152.597,    43.6409,     110.63,    32.7174,
		161.294,    44.1469,    99.0188,    30.8808,
		156.913,    48.3154,   -20.2233,    20.1327,
		135.761,    57.1489,   -174.048,    11.8936,
		163.693,    49.7469,   -4.88444,   -3.68887,
		168.158,    44.2117,    76.7749,   -2.72104,
		165.013,    40.2888,    92.1924,   -7.04049
	};

	Result r;

	r.periodic_idx = 2;
	r.RV_idx = 3;
	r.LV_idx = 1;
	r.baseline_idx = -1;
	r.perfusion_idx = 0;

	r.RV_peak = 13;
	r.LV_peak = 26;

	run( rdlength2, rdcomponents, init_real_data5,  r, true);
}

BOOST_AUTO_TEST_CASE( test_correlation)
{
	const float init_a[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	const float init_b[10] = { -1, -2, -3, -4, -5, -6, -7, -8, -9, -10};
	const float init_c[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const float init_d[10] = { 2, 1, 2, 3, 4, 1, 2, 1, 10, 9};


	vector<float> a(init_a, init_a + 10);
	vector<float> b(init_b, init_b + 10);
	vector<float> c(init_c, init_c + 10);
	vector<float> d(init_d, init_d + 10);

	BOOST_CHECK_CLOSE(correlation(a, a), 1.0f, 0.01);
	BOOST_CHECK_CLOSE(correlation(a, b), 1.0f, 0.01);
	BOOST_CHECK_CLOSE(correlation(a, c) + 1, 1.0f, 0.01);
	BOOST_CHECK_CLOSE(correlation(a, d), 0.40686f, 0.01);


};




BOOST_AUTO_TEST_CASE( test_max_correlation)
{
	float data_rows[3 * 58] = {
		-8.33161,   -5.10186,   -15.0917,
		-6.96909,    3.12137,   -12.9105,
		-7.30663,  -0.381237,   -11.9855,
		-8.29131,   -8.59312,    -12.598,
		-8.12499,   -7.17316,   -13.6857,
		-6.15904,    3.31934,   -12.4489,
		-0.99387,    0.66291,   -10.3515,
		5.99371,   -8.68329,   -10.5568,
		11.0326,    -4.8886,   -12.0612,
		15.6426,    4.84018,    -10.884,
		16.8659,  -0.906586,   -10.2147,
		14.5143,   -7.45862,   -10.5573,
		14.8352,    1.54118,   -10.9445,
		14.333,    10.3105,    -9.4125,
		10.3307,    2.49179,    -7.2102,
		4.79124,   -2.07766,   -6.44488,
		-1.76433,    6.65528,    -7.0605,
		-0.289917,    13.9405,   -5.06004,
		-1.78389,    2.95353,   -3.13089,
		-4.50386,   -1.43204,   -3.50679,
		-8.61227,    9.64421,   -3.64281,
		-7.92335,    8.30778,  -0.735524,
		-7.32191,  -0.522849,    1.08297,
		-9.24206,   -3.13553,   0.536172,
		-10.6123,     8.4578,    1.19548,
		-8.88261,    8.37262,    4.35304,
		-7.12609,   -2.02588,    6.13895,
		-7.39544,    -6.0291,    5.67968,
		-7.71596,   0.494461,    4.80302,
		-4.62405,     7.5096,    6.59965,
		-2.44633,    1.31393,    8.37902,
		-1.82359,   -6.16003,    8.05009,
		-0.456511,   -8.06569,    6.88945,
		0.686634,  -0.333995,    5.24902,
		3.26941,    8.10876,    5.37177,
		3.54848,    1.19863,    8.06705,
		3.28434,   -4.84777,    7.51501,
		2.42494,   -7.79327,    6.62909,
		2.48684,   -8.22002,    5.36254,
		2.01624,   0.311154,    4.00579,
		3.05437,    9.37004,    4.70828,
		2.19007,    4.03349,    7.15733,
		1.70909,   -4.76347,    7.82631,
		0.52038,   -7.48782,    6.61925,
		-0.365862,  -0.149876,    4.49744,
		0.292494,    9.44751,    4.80671,
		0.399364,    3.15453,    7.18225,
		0.0792932,   -5.48093,       7.14,
		-0.228122,   -7.14708,    6.03476,
		-0.950857,   -5.08279,    4.58232,
		-0.434079,    6.65227,    4.08275,
		0.460185, -0.00656957,    6.82004,
		0.730875,   -6.44403,    6.13398,
		-0.159494,   -8.20682,    5.02276,
		0.570189,    1.86787,    4.08707,
		1.29139,    8.58275,    5.21385,
		2.01705,  -0.703119,     6.9356,
		1.46862,   -7.36123,    5.73602};

	CSlopeClassifier::Columns columns( 3 );

	for (size_t c = 0; c < 3; ++c)
		columns[c].resize(58);

	const float *i = data_rows;
	for (size_t r = 0; r < 58; ++r)
		for (size_t c = 0; c < 3; ++c, ++i)
			columns[c][r]  = *i;

	CSlopeClassifier c(columns, true);

	BOOST_CHECK_CLOSE(c.max_selfcorrelation().corr, 0.026411, 0.01);
};


BOOST_FIXTURE_TEST_CASE( test_classifier_7 , SlopeClassifierFixture )
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 7;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-40.1047,   -16.9555,    -31.241,   -5.37162,   -37.6032,   -60.2176,   -35.8488,
		-40.2106,    -17.158,   -24.0179,    3.93582,   -31.7327,   -56.6077,   -40.0227,
		-39.706,   -16.7819,   -21.1349,     11.665,   -22.8374,   -55.1337,   -38.4269,
		-39.6016,   -16.9136,   -10.5167,    17.0759,   -15.2057,   -53.1984,   -28.7697,
		-39.063,   -16.0588,   -6.10676,    22.6064,    -3.6312,   -48.5633,   -15.1243,
		-39.3581,   -16.2762,   -7.06032,    24.1171,   -10.0958,    -49.588,   -17.7728,
		-38.8668,   -14.7053,   -46.5116,   -9.05628,   -55.0097,   -56.1233,   -29.6628,
		-40.3746,    -13.286,   -49.3572,   -24.1834,   -77.2354,   -59.6212,   -5.29381,
		-39.5176,   -14.9442,   -31.1151,    3.04364,    -38.783,   -58.3483,   -37.2792,
		-39.455,   -15.2395,   -5.50793,    21.5392,   -10.3132,   -53.7003,   -19.8789,
		-39.0905,   -14.0979,    9.05797,    17.4227,     13.864,    -42.044,    2.91938,
		-38.5726,   -13.5838,    24.1528,    1.71686,    24.5481,   -22.5903,    15.2367,
		-39.1421,   -14.4559,    28.8951,   -12.4527,    36.8647,    5.96355,    25.7209,
		-39.2029,   -15.4581,    32.3101,   -20.1741,    35.2295,    35.2465,    37.7428,
		-37.7105,   -15.2571,    32.9314,   -26.3422,    39.5647,    53.2982,    46.4405,
		-37.2369,   -16.0488,    28.8949,    -20.964,    33.2994,    62.2649,    50.1281,
		-38.3385,   -16.6371,    -9.9017,    23.3433,    -3.0373,    79.7468,     20.306,
		-35.5468,    -13.836,    -37.974,    3.68114,   -47.9387,    71.1057,   -50.7787,
		-28.6871,   -13.9207,   -41.4923,   -3.33156,   -47.7866,    57.8821,   -50.9853,
		-17.0224,   -16.4429,   -23.6838,    15.7667,   -32.3312,     43.044,   -42.4479,
		0.453652,   -16.6787,  -0.742737,    23.5956,    2.39767,    38.6267,    5.54717,
		17.8875,    -15.854,    21.5136,   -2.37443,    22.5439,    34.8509,    29.8368,
		34.6997,   -13.6318,    28.8985,   -17.3165,    36.3013,    32.4091,    31.8662,
		46.4006,    -11.754,    36.6534,   -24.8767,    37.9528,    25.1835,    27.6265,
		63.5513,    -9.1117,    11.4701,    6.47439,    21.6016,     23.102,    25.2581,
		70.7059,    -4.7131,   -25.0143,    25.1014,   -25.6462,    21.1808,   -13.2238,
		72.5551,   0.336157,   -50.7238,   -6.15387,   -60.6959,    8.96428,   -28.3438,
		65.8879,  -0.474331,   -36.1594,   0.823315,   -49.0889,   0.980592,   -34.5743,
		59.3859,    0.51508,   -2.96713,    24.8086,   -3.16753,   0.372514,     -1.803,
		52.3287,    2.47608,    25.8445,   0.802217,    23.5984,    4.97028,    20.3664,
		48.442,     4.7449,    28.0711,   -8.04611,    32.4982,    6.07814,    22.1791,
		42.157,    6.08515,    30.7636,    -8.3365,     29.568,     7.3208,    20.6963,
		38.0369,    8.27015,    38.2356,   -22.8385,    43.7139,    9.04308,     16.507,
		35.7178,    9.80106,    2.69103,    19.4516,    5.58973,    4.49146,    14.4299,
		32.1969,    11.7232,   -41.8881, -0.0484388,   -48.3417,   -1.74848,   -18.9144,
		21.4051,    13.4256,   -49.3679,    -31.708,   -88.3036,   -14.8064,    26.7785,
		18.0535,    12.6171,   -46.0581,   -20.3626,   -70.0092,   -19.3539,    3.00826,
		12.8971,    11.0537,   -17.3521,    18.1317,   -25.2302,   -13.6394,   -23.7161,
		12.1904,    13.4542,    12.1459,    16.7878,    16.0224,   -5.60132,    10.6164,
		9.47646,    13.1803,    33.3434,    -9.4209,    34.3551,   0.684297,    12.7443,
		7.46745,    14.0032,    38.8389,   -23.1083,    47.2274,    5.96492,    10.6184,
		4.16157,    14.2301,      43.17,   -28.9647,    46.9153,    7.51435,    8.43546,
		3.80424,    15.0953,    1.75047,    16.7508,    12.4179,    5.53774,    14.4093,
		1.15201,    14.0314,   -38.5151,    9.13972,   -41.9105,    2.82193,   -20.4217,
		-1.00213,    14.6358,   -46.2783,   -6.15891,   -51.7479,   -2.88448,   -20.7201,
		-3.47497,    13.1989,   -14.6609,    21.6352,   -16.7795,    -5.7776,   -18.7366,
		-1.93725,    16.0385,    12.5979,    14.1791,    22.6802,    0.96117,    13.1006,
		-3.02747,    15.0351,     28.907,   -4.28057,    34.8117,    4.71671,    14.3342,
		-2.36169,    15.6747,    30.7762,   -11.1889,    43.6844,    6.45374,    13.5113,
		-2.14671,     15.483,    33.4849,   -14.3573,    42.9311,    6.29532,    11.6864,
		-0.589087,    15.9458,    33.4673,   -16.6613,    47.3335,    6.64797,    11.1541,
		-1.77128,    15.2718,    35.2978,   -17.3162,    44.6695,    5.26655,    10.2135,
		-0.895071,    16.0546,   -5.75294,    18.2927,    6.24082,    3.73719,    8.01124,
		-1.18768,    14.8755,   -34.5721,    16.7591,   -35.6699,  -0.584476,   -11.9724,
		-1.80491,    15.1438,   -12.6631,    27.7462,   -6.94231,   -5.29563,   -6.68034,
		-0.63329,    15.9106,    19.1375,    4.41218,    28.2593,   -2.07846,    12.4051,
		-1.28552,    16.0875,    28.7921,   -12.8408,    43.1762,    2.12076,    10.5018,
		-2.08908,    15.8768,    36.2425,   -22.5702,    47.2133,    2.65797,    7.06177
	};

	Result r;

	r.periodic_idx = 4;
	r.RV_idx = 5;
	r.LV_idx = 0;
	// these two are not yet sure
	r.baseline_idx = 6;
	r.perfusion_idx = 2;

	r.RV_peak = 16;
	r.LV_peak = 26;

	run( rdlength2, rdcomponents, init_real_data5,  r, true);
}

