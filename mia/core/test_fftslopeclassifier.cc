/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2013 Gert Wollny
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

void SlopeClassifierFixture::check_equal(int value, int expect, const char *descr)
{
	if (expect != -2) 
		BOOST_CHECK_MESSAGE( value == expect, descr << ": got '" << value << "' expect '" << expect ); 
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




BOOST_FIXTURE_TEST_CASE( test_fftclassifier_4_mean_stripped_normalized_A0, SlopeClassifierFixture) 
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

BOOST_FIXTURE_TEST_CASE( test_fftclassifier_5_mean_stripped_normalized_A0, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 5;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-1.24323,  -3.93247, -0.916126, -0.516395,   -2.6445,
		-2.33264,  -4.07404, -0.711905, -0.510875,  -2.54339,
		-0.186779,  -3.96323,  -1.01749,  -0.52836,  -2.44894,
		1.26034,  -3.90359, -0.930425, -0.158414,  -2.94374,
		-0.801792,  -4.01719,   -1.4883, -0.225382,  -2.91934,
		-2.31448,  -3.99192, -0.498778, -0.575035,  -2.96107,
		-2.7509,  -3.54107, -0.656688, -0.490849,  -3.04571,
		0.387437,  -2.56166, -0.770456, -0.033591,  -3.07935,
		2.43341, -0.343662,  0.297779, -0.234049,  -2.86526,
		0.776616,   1.74352,   -1.5072,-0.00370442,  -2.43819,
		-1.54716,   3.76498,  -1.27748, -0.203012,   -2.3722,
		-3.06754,   5.04063, -0.319293, -0.449072,  -2.17356,
		-2.97067,   4.71433, -0.458413, -0.242447,  -2.28253,
		2.63672,   2.76674,   2.79136, -0.219354,  -2.49088,
		2.81991,   2.27717,   1.78204, -0.344147,  -1.76084,
		1.63493,   3.75932,  -1.00912,  0.345616,   -0.8413,
		-2.27128,   4.01072, -0.098453,-0.0723111, -0.272274,
		2.12792,   3.01511,  0.248443,-0.0530157, -0.144017,
		3.46792,   1.22826,   3.29747, 0.0248776,-0.0509469,
		2.59765,   1.44189,  0.225444,  0.268719,  0.942426,
		-1.93303,   1.69271, -0.461495,  0.134116,   1.31274,
		-2.37119,   1.68474,  0.265792,  0.399304,   1.49896,
		-2.04396,  0.945629,  0.419133,  0.697614,   1.90272,
		-3.06814,  0.976317,  0.548775,  0.445758,   1.76449,
		-2.90793,  0.123982,   0.82827,   0.55707,   1.78234,
		1.87293, -0.682574,  0.653183,  0.125943,   1.43779,
		3.33387, -0.894306,   2.78612,  0.066717,  0.896482,
		2.76931, -0.335305,  0.237221,  0.278422,     1.391,
		-1.96253,   0.40266, -0.772729,-0.0964177,   1.67798,
		-2.7283,  0.258774,  0.130182,  0.066973,    1.5363,
		-0.121792, 0.0582367, -0.798607, -0.229587,   1.29685,
		3.01028, -0.966269,   2.05022,  0.252517,  0.507533,
		2.46228, -0.816684, -0.116439,   0.42897,    0.8219,
		0.033611, -0.462405,  -1.43292,  0.432793,   1.21439,
		-2.79679,  0.246747, -0.437337, -0.254521,   1.09872,
		0.331416,   0.24831, -0.918887, -0.130925,   1.06887,
		2.96863, -0.507881,   2.41897,-0.0505979, 0.0189592,
		3.03877, -0.457899,  0.675986,  0.286146,  0.530687,
		0.557435, 0.0700039,  -1.80464, 0.0469143,  0.903469,
		-1.8932,  0.315828, -0.467714,-0.0155408,   1.04802,
		-2.01307,  0.367562, -0.399124,  0.208145,   1.15835,
		-1.72594, -0.130505,  -0.59924,  0.247928,  0.904536,
		1.54285, -0.812918,  0.257551,  0.331333,   0.51895,
		3.17987, -0.611213,   2.48875, -0.041179, 0.0133016,
		-0.529665,  0.310492,  -1.63808, -0.181807,  0.908929,
		-2.59541,  0.243385, -0.196345, -0.146626,   1.19017,
		-2.43221,   0.14605, -0.582319,  -0.36971,   0.96609,
		2.47293, -0.572708,  0.609601,  0.143947,  0.475714,
		3.39843, -0.805907,   2.91773,  0.531761, 0.0197614,
		2.48078, -0.344001, -0.718958,  0.271223,  0.931237,
		-1.88012, 0.0982012, -0.990122,    -0.285,   1.21309,
		-2.45947, -0.153156, -0.573488, 0.0603877,   1.25973,
		-1.47223, -0.204439, -0.910226, -0.150734,   1.10641,
		3.06287,  -1.01205,   1.82191,  0.148831,  0.309003,
		3.3318, -0.749815, 0.0612008,0.00193238,  0.722728,
		1.16063,  -0.37724,  -1.71414,  0.429394,   1.34129,
		-1.90388, -0.281914, -0.420423, -0.184625,   1.32813,
		-2.82621, -0.444251, -0.199775, -0.236035,   1.25797

	};

	Result r;

	r.periodic_indices = {0,2};
	r.RV_idx = 1;
	r.LV_idx = 4;
	r.baseline_idx = 0;
	r.perfusion_idx = -1;

	r.RV_peak = 11;
	r.LV_peak = 22;

	run( rdlength2, rdcomponents, init_real_data5,  r);
}

BOOST_FIXTURE_TEST_CASE( test_fftclassifier_6_mean_stripped_normalized_A0, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 6;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-3.92743,  -2.74669,   -0.9969, -0.599847,  -0.48226, -0.760855,
		-4.10575,  -2.46004,  -2.03937, -0.693136, -0.651678,  -1.16166,
		-4.03512,  -2.19923,  0.174118, -0.693447, -0.656636,  -1.23172,
		-3.9825,  -2.50114,   1.59599, -0.304458, -0.258122,  -1.50348,
		-4.01574,  -3.06923, -0.465981,  -1.03398, -0.167369, -0.853494,
		-4.02654,  -2.77062,  -2.03507, -0.430361, -0.737698,  -1.38967,
		-3.57027,  -2.90638,  -2.44333, -0.642548,  -0.66551,  -1.43153,
		-2.70103,  -2.28039,  0.823135, -0.438033, -0.395501,   -2.2801,
		-0.362183,  -2.34967,   2.46378,   1.12978, -0.195293,  -1.17296,
		1.70978,  -2.32682,   1.14445,  -1.00091,-0.0508346,  -1.10114,
		3.79634,  -2.58379,  -1.26282,  -1.09827, -0.213299, -0.771334,
		5.05066,   -2.1211,  -2.86367, -0.552544, -0.654875,  -1.14833,
		4.70202,  -2.12213,  -2.71267, -0.698673, -0.509368,  -1.40258,
		2.78885,  -1.55233,   2.19911,   3.49786, -0.206711,   -1.1928,
		2.31941,  -1.26432,   2.48299,   2.52202, -0.190742, -0.456296,
		3.66326, -0.393021,   1.95587, -0.790314,   0.11364,  -1.13503,
		3.95804,-0.0242899,  -2.09231,  -0.62072,  -0.40702, -0.896963,
		2.83316,   0.90612,   2.36309,   0.22276,  -0.53285,  -1.61021,
		1.2626,  0.709103,   2.81875,   3.78912,  0.141162,-0.0427752,
		1.31353,   1.63435,   2.68757,  0.240593,-0.00191075, -0.651834,
		1.63864,   1.30213,  -1.76848,  -1.12213,  -0.12252,-0.0910608,
		1.57062,   1.95444,  -2.22691, -0.678256,  -0.10362, -0.768183,
		0.78821,    2.5928,   -1.8834, -0.626864, 0.0791056,  -1.02025,
		0.856341,   2.26448,  -2.95764, -0.581759, -0.113961, -0.796718,
		0.00140791,   2.36641,  -2.84816, -0.290274,-0.00735982, -0.842444,
		-0.860369,   2.35206,   1.96925,  0.369352, -0.306431, -0.877443,
		-0.860215,   1.39824,   2.72787,   3.16606,  0.241506,  0.517307,
		-0.362842,   1.52905,   2.66445,  0.446646,  0.348092,  0.502547,
		0.43489,   1.08771,  -1.90229,  -1.27169,-0.0486407,  0.985722,
		0.24186,   1.41646,  -2.71475, -0.643281, -0.123168,  0.235714,
		-0.0116993,   1.26597, 0.0866611,   -1.1088, -0.377956,  0.144727,
		-0.961689,   1.02699,   2.58515,   2.40762,  0.343284,   0.13974,
		-0.801247,  0.731461,   2.36234,  0.235359,  0.618005,  0.639477,
		-0.437355,  0.651644,  0.167496,  -1.48571,  0.580134,   0.89405,
		0.298954,  0.515605,  -2.78318, -0.968407, -0.207194,  0.827396,
		0.20592,  0.927112,   0.50999,  -1.05494, -0.177751,  0.309815,
		-0.407049,  0.188207,   2.35221,    3.0256,  0.307239,  0.763678,
		-0.387253,   0.40383,   2.72383,   1.24393,  0.630768,  0.932769,
		0.134098,  0.072823,  0.697102,  -1.60666,  0.362355,   1.29322,
		0.341407,  0.644736,  -1.85368, -0.891444,-0.0096542,  0.587751,
		0.402427,  0.737974,  -2.00613, -0.854806,  0.222122,  0.630352,
		-0.166223,  0.809985,  -1.57146,  -1.09351, 0.0789725, -0.026897,
		-0.826443,  0.657797,   1.46282,  0.411913,  0.366792,  0.138596,
		-0.484033, 0.0782122,   2.50389,   3.18573,  0.404175,  0.998406,
		0.426871, -0.186601, -0.473491,  -1.53302,  0.229263,   1.68207,
		0.298789,  0.658183,  -2.63541, -0.716489,-0.0938899,   0.84149,
		0.184885,  0.422039,  -2.37437,  -1.04126, -0.330408,  0.745532,
		-0.601427,  0.785786,   2.34927,   0.88921,  0.173148,  0.050343,
		-0.649364,  0.116214,   2.58595,   3.67068,   1.03199,   1.05084,
		-0.297442,  0.519729,    2.4275, -0.281641,   0.58422,   1.08775,
		0.212687,  0.200021,  -1.90414,   -1.2095, 0.0286358,   1.60771,
		-0.087227,  0.598795,  -2.46863,  -1.03799,  0.166553,  0.997982,
		-0.122021,  0.302007,  -1.46575,  -1.11171, 0.0826348,   1.27842,
		-0.936374,  0.439852,   2.56456,   2.38387,  0.467225,  0.746242,
		-0.599368,0.00928245,   2.97632,  0.867834,  0.649024,   1.93236,
		-0.288503,  0.418923,   1.20677,  -1.43648,  0.841421,   1.64702,
		-0.17953,  0.505682,   -2.0062, -0.718424,  0.067401,   1.44034,
		-0.381445,  0.655562,  -2.87611, -0.743626, -0.158513,  0.968377
	};

	Result r;

	r.periodic_indices = {2,3,5};
	r.RV_idx = 0;
	r.LV_idx = 1;
	r.baseline_idx = 4;
	r.perfusion_idx = 5;

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

BOOST_FIXTURE_TEST_CASE( test_fftclassifier_5_mean_stripped_normalized_case_f2, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 5;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-4.20263,  -2.70113,  -1.56285, -0.122419, -0.519838,
		-4.14458,  -2.73708,  -1.50753, -0.392209, -0.571841,
		-4.14812,  -2.42813,  -1.44604,   1.17065,   2.53728,
		-4.19023,  -2.49907,  -1.85959,    1.0809,   3.66755,
		-4.31664,  -2.68787,  -1.35475,  0.539401,   2.46937,
		-4.24254,  -2.78273,  -1.48388,-0.0508881,  0.209082,
		-4.20127,  -2.80553,  -1.55843, -0.496703, -0.693301,
		-4.02603,  -2.76303,  -1.50571, -0.695258, -0.845916,
		-3.08827,  -2.68773,   -1.5782, -0.333504,  -1.03176,
		-0.648834,  -2.72944,  -1.58556, -0.603829,  -1.23253,
		1.90596,  -2.67291,  -1.68885, -0.694407,  -1.48111,
		4.74895,  -2.49014,  -1.75882, -0.431916,  -1.43944,
		6.64879,  -2.69487,  -1.11818,  0.409632,  0.879642,
		5.91837,  -2.61182,  -1.25598,   1.02398,   3.74853,
		4.71948,  -2.42541,  -1.35091,   1.52095,   4.29262,
		4.5704,  -2.53088,  -1.26465,  0.460359,   2.18152,
		5.31024,  -2.25061,  -1.82652, -0.619473, -0.656534,
		5.55091,   -1.4261,   -2.0763, -0.573344,  -1.34082,
		4.93636, -0.496951,   -2.1742, -0.838584,  -1.63907,
		4.22513,  0.448661,   -2.0855,  -1.04623,  -1.69168,
		3.71262,   1.61745,  -2.15455,  -0.68459,  -1.80981,
		2.83105,   2.20842,  -2.01104, -0.845911,  -1.73163,
		2.79341,   2.61518,  -1.87221,  -1.04314,  -1.92346,
		2.34714,   3.07127,  -1.72734, -0.639635,  -1.75186,
		2.1076,   3.11538, -0.972729,  0.474295,  0.697193,
		0.419224,   2.72745, -0.802194,   1.27407,   3.90459,
		-0.481507,   2.92294, -0.491682,    1.4955,   3.80295,
		-0.552636,   3.34084, -0.303361,  0.214098,   0.65461,
		-0.154489,   3.44411, -0.145979, -0.386803, -0.974192,
		0.0703205,   3.37037, 0.0883532, -0.244495,  -1.31844,
		-0.193403,   3.20321,  0.355319, -0.452601,  -1.61332,
		-0.606715,   2.84432,  0.746348, -0.651767,  -1.60697,
		-0.725356,   2.70498,  0.898661, -0.240265,  -1.65224,
		-0.81035,   2.27983,   1.19012, -0.486901,  -1.64254,
		-0.729025,   1.95624,   1.43624, -0.627615,  -1.78248,
		-0.96526,   1.68209,   1.51497,  -0.29144,  -1.59798,
		-0.867621,    1.4022,   1.68212, -0.433499,  -1.75592,
		-0.880838,   1.10996,   1.81271, -0.647641,  -1.68133,
		-0.952748,  0.862895,   1.78697,   1.01805,   1.34387,
		-1.60485,  0.495438,   1.08883,   1.28871,   3.48969,
		-1.68089,  0.358644,    2.0488,  0.357493,  0.951064,
		-1.15736,  0.454826,   1.91556,-0.0193755,  -0.93321,
		-0.4998,  0.375137,   1.89908, -0.344608,  -1.51928,
		-0.536911,  0.224298,   1.94065, -0.605244,  -1.58447,
		-0.539652,  0.315621,   1.78926, -0.245722,  -1.64045,
		-0.539019,   0.14266,   1.83253, -0.489996,  -1.58672,
		-0.351916,-0.0830833,   1.85081,   0.60093,   1.32687,
		-1.0484, -0.241262,  0.742599,   1.46282,   3.87295,
		-1.35898, -0.340186,  0.900608,   1.21857,   3.81141,
		-1.38195, -0.540471,   1.88927,  0.223779,   1.13263,
		-0.818318, -0.247488,   1.74727, 0.0753026, -0.732543,
		-0.509848,  -0.30297,   1.76822,  -0.19236, -0.825612,
		-0.447295, -0.181871,   1.68317, -0.380819,  -1.23736,
		-0.579275,-0.0562248,    1.6311, -0.214838,  -1.23589,
		-0.723594,-0.0343166,   1.59821, -0.325477,  -1.38142,
		-1.00854, -0.286968,   1.65717,  0.573855,   1.61084,
		-1.46868, -0.254196,   1.44782,    1.1548,   2.74387,
		-1.43153, -0.303927,   1.58076,  -0.24465, -0.665129

	};

	Result r;

	r.periodic_indices = {3, 4};
	r.RV_idx = 0;
	r.LV_idx = 1;
	r.baseline_idx = -2;
	r.perfusion_idx = 2;

	r.RV_peak = 12;
	r.LV_peak = 28;

	run( rdlength2, rdcomponents, init_real_data5,  r);
}

BOOST_FIXTURE_TEST_CASE( test_fftclassifier_6_mean_stripped_normalized_case_f2, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 6;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-0.0373807,  -4.18527, -0.686903,  -2.63995,  -1.67434,-0.00110557,
		0.0236276,  -4.12739, -0.796188,  -2.67137,  -1.62579, -0.248649,
		-1.62019,  -4.12748,   2.26083,  -2.49568,  -1.38468,  0.465086,
		-1.25458,  -4.22421,   3.92422,   -2.7127,  -1.54194,  0.132833,
		-2.0193,  -4.26209,   1.63983,  -2.68066,  -1.43048, -0.154394,
		-0.86388,  -4.19246, -0.426197,  -2.68815,  -1.67336, -0.143629,
		0.0190689,  -4.17904, -0.984704,  -2.72385,  -1.70306,   -0.3224,
		0.289025,  -4.01542,  -1.02702,  -2.69576,  -1.61967, -0.466418,
		0.578694,  -3.09308, -0.913035,  -2.64605,  -1.63932,-0.0565045,
		0.837545, -0.658173,  -1.08055,  -2.68373,  -1.64212, -0.261218,
		1.20074,   1.88561,  -1.16035,  -2.63517,  -1.71596, -0.274403,
		1.35258,   4.72022,   -0.9334,   -2.4707,   -1.7438, -0.022225,
		-1.40339,   6.72783,-0.0333185,  -2.57035,  -1.35463,  0.114755,
		-1.43987,   5.90898,   3.69907,  -2.77716,  -1.01336, 0.0431541,
		-1.26201,    4.6759,   4.70323,  -2.68408, -0.949179,  0.410318,
		-2.02651,   4.64995,   1.12904,  -2.45725,  -1.43287,  -0.16972,
		0.112964,   5.34536,  -1.02052,  -2.13507,  -1.99676, -0.450821,
		1.28364,   5.53008, -0.892741,  -1.39031,  -2.07345, -0.185848,
		1.78533,   4.89402,  -0.91606, -0.487319,  -2.10627,   -0.3476,
		1.94631,    4.1751, -0.890983,  0.447307,  -1.98905, -0.527793,
		2.23064,   3.64447, -0.650377,   1.58216,  -1.98419, -0.131954,
		2.1565,   2.76558,  -0.64347,   2.17588,  -1.84531, -0.309823,
		2.28891,   2.72632,   -0.8333,   2.58855,  -1.71113, -0.449884,
		2.24214,   2.27309, -0.508402,   3.01874,   -1.5238,-0.0984935,
		-1.54671,   2.20009, -0.146462,    3.2712,  -1.22101,  0.222185,
		-2.63321,  0.477856,   3.22352,    2.6806, -0.752986,  0.198693,
		-3.01052, -0.399914,   2.88267,   2.91929, -0.528215,  0.419559,
		-1.70721, -0.453899, -0.385827,    3.5072, -0.586215,-0.0274249,
		0.306157, -0.137232, -0.997533,   3.52278,  -0.23366, -0.130097,
		1.15186, 0.0383483, -0.709346,   3.36416,  0.158113,  0.131845,
		1.45642, -0.237384, -0.906283,   3.18566,  0.445438, 0.0135435,
		1.44832, -0.651919,  -0.98334,    2.8234,  0.831839, -0.182321,
		1.58229, -0.783643, -0.798565,   2.65454,     1.032,  0.235779,
		1.49647, -0.863179, -0.953665,   2.24022,     1.295,-0.0113776,
		1.52985, -0.780355,  -1.16143,   1.92488,   1.52177, -0.114418,
		1.50088,  -1.02468, -0.848729,    1.6231,   1.64088,  0.168343,
		1.49708, -0.921941,   -1.1063,   1.35972,   1.77577, 0.0662969,
		1.33874, -0.925786,   -1.2226,   1.08416,   1.87025, -0.167997,
		-1.83307, -0.890486,  0.535345,  0.906876,   1.64622,  0.586622,
		-1.95347,  -1.59779,   3.19251,  0.349631,   1.26453,  0.343759,
		-1.94824,  -1.59629, -0.271505,  0.466357,   1.79028, 0.0289491,
		0.16367,  -1.15362,  -1.04003,  0.485425,   1.84162,   0.20883,
		0.848024, -0.520415,  -1.33747,  0.385846,   1.87622, 0.0632085,
		1.00806, -0.564287,  -1.38213,  0.227136,   1.93292, -0.167356,
		1.24627, -0.583939,  -1.13443,  0.284786,   1.84599,  0.208038,
		1.08564, -0.572155,  -1.28385,  0.133768,   1.84692,-0.0507602,
		-1.7878, -0.288662,   0.37671,-0.0321515,    1.6914,  0.185304,
		-1.5391,  -1.08015,   4.03591, -0.476839,   1.07636,  0.443942,
		-1.63763,  -1.38149,   3.79764, -0.553968,   1.19215,   0.21551,
		-1.98626,  -1.29855, -0.145989, -0.439824,   1.63619, -0.146761,
		-0.136431, -0.800614,  -1.01861, -0.197837,    1.6309,  0.237863,
		0.0456405, -0.498634,  -1.08453,  -0.25906,   1.66591,0.00797333,
		0.535531,  -0.45422,  -1.26118, -0.154093,   1.62252,-0.0544712,
		0.74116,  -0.60146,  -1.03328,-0.0596466,   1.62872,  0.117547,
		0.8366, -0.746834,  -1.17068, -0.032676,   1.59038, 0.0486963,
		-2.01953, -0.938069,  0.541287, -0.232146,   1.48536, 0.0810022,
		-2.28046,  -1.41931,   1.93837,  -0.29015,    1.4216,  0.369608,
		-0.219623,  -1.40719,  -1.09892, -0.239694,   1.43935,-0.0934383
	};

	Result r;

	r.periodic_indices = {0,2};
	r.RV_idx = 1;
	r.LV_idx = 3;
	r.baseline_idx = 6;
	r.perfusion_idx = 4;

	r.RV_peak = 12;
	r.LV_peak = 28;

	run( rdlength2, rdcomponents, init_real_data5,  r);
}

#if 0 
// This doesn't work, because the periodicity of the breathing part of the curve is not sufficient to 
// obtain a proper analysis 
BOOST_FIXTURE_TEST_CASE( test_fftclassifier_4_bh_mean_stripped_normalized, SlopeClassifierFixture) 
{
	const size_t rdlength2 = 58;
	const size_t rdcomponents = 4;
	const float init_real_data5[rdlength2 * rdcomponents] = {
		-8.56684,  -1.90767,  -6.02376, -0.706818,
		-9.60308,  -1.08342,  -6.06315,  0.164947,
		-9.63118, -0.973474,  -5.87313,  0.101761,
		-9.58921,  -1.01241,  -6.04252,  0.306249,
		-9.56992, -0.961478,  -5.87508,  0.200129,
		-9.62613,   -1.0604,  -6.03469,   0.31587,
		-9.5395, -0.964425,  -5.88022,  0.164905,
		-9.62577, -0.982226,  -6.01143,  0.319313,
		-9.53061, -0.955043,  -5.90961,  0.181825,
		-9.5628,  -1.01693,  -6.01172,  0.280174,
		-9.48211, -0.966595,  -5.90629,  0.144495,
		-9.38117, -0.963373,   -6.0163,  0.255743,
		-8.8198, -0.901888,  -6.00255, 0.0956278,
		-6.29543, -0.949093,  -6.18035, 0.0711044,
		-1.48513, -0.861925,  -6.26619,-0.0748906,
		2.60793, -0.870087,  -6.48608,   0.22775,
		4.56216, -0.718052,  -6.39048,  0.284032,
		5.19869, -0.728567,  -6.51902,  0.499917,
		5.61227,  -0.58259,  -6.35593,  0.525581,
		5.86013, -0.561412,  -6.48839,  0.813976,
		6.04881,  -0.42319,  -6.27649,  0.900544,
		6.13859, -0.384883,  -6.39301,   1.07462,
		6.18972, -0.189638,   -6.1816,   1.11928,
		6.23865,  -0.20875,  -6.22936,   1.27834,
		6.25178,-0.0729162,  -5.93668,   1.39621,
		6.23483,-0.0427135,  -5.83968,   1.60967,
		6.06277,  0.141884,  -5.25434,   1.67855,
		6.11783,   0.13854,  -4.66806,   1.98503,
		5.96582,  0.232577,  -3.53718,   2.07106,
		5.66384,  0.233609,  -2.35346,   2.39435,
		5.46306,  0.286983, -0.839841,   2.53855,
		5.20551,   0.21582,  0.407695,   2.87972,
		4.74894,  0.231797,   1.92906,   2.94508,
		4.57836,  0.202364,   3.28487,   3.17398,
		4.19513,  0.235586,   4.30583,   3.10971,
		3.78891,  0.240463,   5.10161,   3.35773,
		3.19424,  0.265933,   6.22925,    3.2003,
		3.16143,  0.153174,   6.62723,   3.38331,
		2.7963,  0.286422,   7.29373,   3.21944,
		2.64416,  0.191374,   7.43069,   3.31956,
		2.39422,  0.302261,   7.92139,   3.09638,
		1.97891,  0.211834,   7.91403,    3.0396,
		2.05474,  0.426838,   8.41595,   3.24159,
		1.3774,  0.802978,   8.21854,   3.89325,
		1.17378,   1.00198,   8.47994,    3.9056,
		0.720233,  0.630129,   8.52481,   1.90649,
		-0.660702,  0.960972,   8.07346,   -3.5779,
		-3.22219,   1.29651,   5.11785,  -11.3922,
		-3.57143,   2.16356,   6.26505,  -11.1233,
		-2.91369,   1.40707,   7.47706,   -6.6121,
		-1.43233,  0.807226,   8.11355,  -2.61964,
		0.188354,  0.644374,   7.97506, -0.296111,
		2.0228,  0.280846,   8.03186, -0.268231,
		2.27915,  0.463546,   7.79169,  0.554053,
		3.04691,  0.702453,   7.88107,  -2.66721,
		1.29872,   1.68689,   5.17184,  -11.2943,
		0.0183071,   2.12505,   5.21134,   -12.387,
		-0.974358,   1.37198,   6.65216,   -8.2057
	};

	Result r;

	r.periodic_indices = {3};
	r.RV_idx = 0;
	r.LV_idx = 3;
	r.baseline_idx = 1;
	r.perfusion_idx = -1;

	r.RV_peak = 24;
	r.LV_peak = 44;

	run( rdlength2, rdcomponents, init_real_data5,  r);
}
#endif
