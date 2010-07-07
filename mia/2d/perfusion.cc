/* -*- mona-c++  -*-
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

#include <memory>
#include <fstream>
#include <boost/lambda/lambda.hpp>
#include <mia/2d/perfusion.hh>
#include <mia/2d/ica.hh>
#include <mia/2d/2dimageio.hh>

NS_MIA_BEGIN
using namespace std; 
using boost::lambda::_1;
using boost::lambda::_2;



/* Implementation class */
struct C2DPerfusionAnalysisImpl {
	C2DPerfusionAnalysisImpl(size_t components, bool normalize, 
				 bool meanstrip); 
	
	vector<C2DFImage> get_references() const; 
	void run_ica(const vector<C2DFImage>& series);
	C2DFilterPlugin::ProductPtr get_crop_filter(float scale, C2DBounds& crop_start,
						    C2DPerfusionAnalysis::EBoxSegmentation approach, 
						    const std::string& save_features) const; 
	C2DFilterPlugin::ProductPtr create_LV_cropper_from_delta(P2DImage rvlv_feature,
						      float LV_mask_amplify,
						      C2DBounds& crop_start,
						      const std::string& save_features)const; 

	C2DFilterPlugin::ProductPtr create_LV_cropper_from_features(float LV_mask_amplify,
								    C2DBounds& crop_start, 
								    const string& save_features)const; 
		
	CICAAnalysis::IndexSet get_all_without_periodic()const; 
	void save_feature(const string& base, const string& feature, const C2DImage& image)const; 
	P2DImage get_rvlv_delta_from_feature(const string& save_features)const; 
	P2DImage get_rvlv_delta_from_peaks(const string& save_features)const; 

	size_t _M_components;
	bool _M_normalize;  
	bool _M_meanstrip; 
	int _M_max_iterations; 
	unique_ptr<C2DImageSeriesICA> _M_ica; 
	vector<C2DFImage> _M_series; 
	C2DBounds _M_image_size; 
	CSlopeClassifier _M_cls; 
	size_t _M_length; 
};



C2DPerfusionAnalysis::C2DPerfusionAnalysis(size_t components, bool normalize, 
					   bool meanstrip):
	impl(new C2DPerfusionAnalysisImpl(components, normalize,  meanstrip))
{
}

C2DPerfusionAnalysis::~C2DPerfusionAnalysis()
{
	delete impl; 
}

void C2DPerfusionAnalysis::set_max_ica_iterations(size_t maxiter)
{
	assert(impl); 
	impl->_M_max_iterations = maxiter; 
}

C2DFilterPlugin::ProductPtr C2DPerfusionAnalysis::get_crop_filter(float scale, C2DBounds& crop_start,
								  EBoxSegmentation approach, 
								  const std::string& save_features) const

{
	assert(impl); 
	return impl->get_crop_filter(scale, crop_start, approach, save_features); 
}

void C2DPerfusionAnalysis::run(const vector<C2DFImage>& series)
{
	assert(impl); 
	impl->run_ica(series); 
}

vector<C2DFImage> C2DPerfusionAnalysis::get_references() const
{
	assert(impl); 
	return impl->get_references(); 
}


C2DPerfusionAnalysisImpl::C2DPerfusionAnalysisImpl(size_t components, 
						   bool normalize, 
						   bool meanstrip):
	_M_components(components), 
	_M_normalize(normalize), 
	_M_meanstrip(meanstrip),
	_M_max_iterations(0),
	_M_length(0)
{
}

P2DImage C2DPerfusionAnalysisImpl::get_rvlv_delta_from_feature(const string& save_features) const 
{
	C2DImageSeriesICA::IndexSet plus;
	C2DImageSeriesICA::IndexSet minus;
	
	plus.insert(_M_cls.get_RV_idx());
	minus.insert(_M_cls.get_LV_idx());
	
	P2DImage result = _M_ica->get_delta_feature(plus, minus); 
	if (!save_features.empty()) {
		save_feature(save_features, "RVic", *_M_ica->get_feature_image(_M_cls.get_RV_idx())); 
		save_feature(save_features, "LVic", *_M_ica->get_feature_image(_M_cls.get_LV_idx())); 
		save_feature(save_features, "RVLVica", *result); 
	}
	
	return result; 
}

P2DImage C2DPerfusionAnalysisImpl::get_rvlv_delta_from_peaks(const string& save_features) const 
{
	const int RV_peak = _M_cls.get_RV_peak();
	const int LV_peak = _M_cls.get_LV_peak();
	
	if (RV_peak < 0 || LV_peak < 0)
		return P2DImage(); 

	C2DFImage *prvlv_diff= new C2DFImage(_M_image_size);
	P2DImage result(prvlv_diff); 

	transform(_M_series[RV_peak].begin(), _M_series[RV_peak].end(),
		  _M_series[LV_peak].begin(), prvlv_diff->begin(), _1 - _2);

	if (!save_features.empty()) {
		save_feature(save_features, "RVpeak", _M_series[RV_peak]); 
		save_feature(save_features, "LVpeak", _M_series[LV_peak]); 
		save_feature(save_features, "RVLVdelta", *result); 
	}

	return result; 
}

C2DFilterPlugin::ProductPtr C2DPerfusionAnalysisImpl::get_crop_filter(float scale, C2DBounds& crop_start,
								      C2DPerfusionAnalysis::EBoxSegmentation approach, 
								      const string& save_features) const
{
	switch (approach) {
	case C2DPerfusionAnalysis::bs_delta_feature: 
		return create_LV_cropper_from_delta(get_rvlv_delta_from_feature(save_features), 
					     scale, crop_start, save_features);
		break; 
	case C2DPerfusionAnalysis::bs_delta_peak: 
		return create_LV_cropper_from_delta(get_rvlv_delta_from_peaks(save_features), 
						    scale, crop_start, save_features);
	case C2DPerfusionAnalysis::bs_features: 
		return create_LV_cropper_from_features(scale, crop_start, save_features);
	default: 
		throw invalid_argument("C2DPerfusionAnalysis::get_crop_filter: unkonwn segmentatoin method given");
	}
}


vector<C2DFImage> C2DPerfusionAnalysisImpl::get_references() const
{
	vector<C2DFImage> result(_M_length); 
	CICAAnalysis::IndexSet component_set = get_all_without_periodic(); 
	
	for (size_t i = 0; i < _M_length; ++i) 
		result[i] = _M_ica->get_partial_mix(i, component_set); 
	return result; 
}

void C2DPerfusionAnalysisImpl::run_ica(const vector<C2DFImage>& series) 
{
	_M_series = series; 
	_M_length = series.size(); 
	if (_M_length < _M_components) 
		throw invalid_argument("C2DPerfusionAnalysis::run_ica: input series too short"); 

	srand(time(NULL));
	_M_image_size = series[0].get_size(); 

	unique_ptr<C2DImageSeriesICA> ica(new C2DImageSeriesICA(series, false));
	if (_M_components > 0) {
		ica->set_max_iterations(_M_max_iterations);
		ica->run(_M_components, _M_meanstrip, _M_normalize);
	} else {
		// maybe one can use the correlation and create an initial guess by combining
		// highly correlated curves.
		float min_cor = 0.0;
		for (int i = 7; i > 3; --i) {
			unique_ptr<C2DImageSeriesICA> l_ica(new C2DImageSeriesICA(series, false));
			l_ica->set_max_iterations(_M_max_iterations);
			l_ica->run(i, _M_meanstrip, _M_normalize);

			CSlopeClassifier cls(l_ica->get_mixing_curves(), _M_meanstrip);
			float max_slope = /*log2(i) * */ cls.get_max_slope_length_diff();
			cvinfo() << "Components = " << i << " max_slope = " << max_slope << "\n";
			if (min_cor < max_slope) {
				min_cor = max_slope;
				_M_components = i;
				ica.swap(l_ica);
				_M_cls = cls; 
			}
		}
	}
	_M_ica.swap(ica);

	cvinfo() << "Periodic: " << _M_cls.get_periodic_idx() << "\n"; 
	cvinfo() << "RV:       " << _M_cls.get_RV_idx() << "\n"; 
	cvinfo() << "LV:       " << _M_cls.get_LV_idx() << "\n"; 
	cvinfo() << "Baseline: " << _M_cls.get_baseline_idx() << "\n"; 
	cvinfo() << "Perf    : " << _M_cls.get_perfusion_idx() << "\n"; 
}

CICAAnalysis::IndexSet C2DPerfusionAnalysisImpl::get_all_without_periodic()const 
{
	assert(_M_ica); 
	int periodic_index = _M_cls.get_periodic_idx();

	CICAAnalysis::IndexSet result;
	for (int i = 0; i < (int)_M_components; ++i) {
		if (i != periodic_index)
			result.insert(i);
	}
	return result;
}

class GetRegionCenter: public TFilter<C2DFVector> {
public:
	template <typename T>
	C2DFVector operator() (const T2DImage<T>& image) const {
		C2DFVector result;
		size_t n = 0;
		typename T2DImage<T>::const_iterator i = image.begin();
		for (size_t y = 0; y < image.get_size().y; ++y)
			for (size_t x = 0; x < image.get_size().x; ++x, ++i) {
				if (*i) {
					result.x += x;
					result.y += y;
					++n;
				}
			}
		if (!n)
			THROW(invalid_argument, "GetRegionCenter: provided an empty region");
		return result / float(n);
	};
};


class GetRegionSize: public TFilter<size_t> {
public:
	GetRegionSize(int label):_M_label(label) {
	}
	template <typename T>
	size_t operator() (const T2DImage<T>& image) const {
		size_t n = 0;
		typename T2DImage<T>::const_iterator i = image.begin();
		for (size_t y = 0; y < image.get_size().y; ++y)
			for (size_t x = 0; x < image.get_size().x; ++x, ++i) {
				if (*i == (T)_M_label)
					++n;
			}
		return n;
	};
private: 
	int _M_label; 
};

class GetClosestRegionLabel: public TFilter<int> {
public:
	GetClosestRegionLabel(const C2DFVector& point):m_point(point){}
	template <typename T>
	int operator() (const T2DImage<T>& image) const;
private:
	C2DFVector m_point;
	struct Collector {
		size_t size;
		C2DFVector center;
	};

};

typedef pair<float, size_t> element;

static void save_coefs(const string&  coefs_name, const C2DImageSeriesICA& ica)
{
	CSlopeClassifier::Columns mix = ica.get_mixing_curves();
	ofstream coef_file(coefs_name.c_str());

	for (size_t r = 0; r < mix[0].size(); ++r) {
		for (size_t c = 0; c < mix.size(); ++c) {
			coef_file   << setw(10) << mix[c][r] << " ";
		}
		coef_file << "\n";
	}
	if (!coef_file.good())
		THROW(runtime_error, "unable to save coefficients to " << coefs_name);
}


C2DFilterPlugin::ProductPtr C2DPerfusionAnalysisImpl::create_LV_cropper_from_delta(P2DImage rvlv_feature,
									float LV_mask_amplify,
									C2DBounds& crop_start, 
									const string& save_features)const 
{
	const char *kmeans_filter_chain[] = {
		"close:shape=[sphere:r=2]",
		"open:shape=[sphere:r=2]"
	};

	const char *RV_filter_chain[] = {
		"label",
		"selectbig"
	};

	const char *LVcandidate_filter_chain[] = {
		"binarize:min=0,max=0",
		"label",
	};
	P2DImage pre_kmeans = run_filter_chain(rvlv_feature, 2, kmeans_filter_chain);

	P2DImage RV;
	P2DImage kmeans;
	size_t nc = 1;
	size_t npixels;
	do {
		++nc;
		stringstream kfd;
		kfd << "kmeans:c=" << 2*nc + 1;
		kmeans = run_filter(*pre_kmeans, kfd.str().c_str());
		stringstream kfb;
		kfb << "binarize:min="<<2*nc<<",max="<<2*nc;
		P2DImage kmeans_binarized = run_filter(*kmeans, kfb.str().c_str());

		RV = run_filter_chain(kmeans_binarized, 2, RV_filter_chain);

		npixels = ::mia::filter(GetRegionSize(1), *RV);

	} while (10 * npixels > rvlv_feature->get_size().x * rvlv_feature->get_size().y && nc < 5);

	if (nc == 5)
		return C2DFilterPlugin::ProductPtr();

	C2DFVector RV_center = ::mia::filter(GetRegionCenter(), *RV);

	P2DImage LV_candidates = run_filter_chain(kmeans, 2, LVcandidate_filter_chain);


	int label = ::mia::filter(GetClosestRegionLabel(RV_center), *LV_candidates);
	size_t lv_pixels = ::mia::filter(GetRegionSize(label), *LV_candidates);

	if (10 * lv_pixels > rvlv_feature->get_size().x * rvlv_feature->get_size().y) 
		return C2DFilterPlugin::ProductPtr();

	stringstream binarize_lv;
	binarize_lv << "binarize:min="<< label << ",max=" << label;
	cvinfo() << "LV label=  " << label << "\n";
	P2DImage LV = run_filter(*LV_candidates, binarize_lv.str().c_str());

	C2DFVector LV_center = ::mia::filter(GetRegionCenter(), *LV);

	cvinfo() << "RV center = " << RV_center << "\n";
	cvinfo() << "LV center = " << LV_center << "\n";

	float r = LV_mask_amplify * (LV_center - RV_center).norm();
	cvinfo() << "r = " << r << "\n";
	stringstream mask_lv;
	crop_start = C2DBounds(int(LV_center.x - r), int(LV_center.y - r));

	// this is ugly and should be replaced
	if (crop_start.x > LV_center.x ||crop_start.y > LV_center.y)
		return C2DFilterPlugin::ProductPtr();

	mask_lv << "crop:start=[" << crop_start
		<< "],end=[" << C2DBounds(int(LV_center.x + r), int(LV_center.y + r)) << "]";
	cvmsg() << "crop region = '" << mask_lv.str() << "'\n";

	if (!save_features.empty()) {
		save_feature(save_features, "kmeans", *kmeans); 
		save_feature(save_features, "LV_candidates", *LV_candidates); 
		save_feature(save_features, "LV", *LV); 
		save_feature(save_features, "RV", *RV); 
		save_coefs(save_features + ".txt", *_M_ica); 
	}

	return C2DFilterPluginHandler::instance().produce(mask_lv.str().c_str());
}


C2DFilterPlugin::ProductPtr C2DPerfusionAnalysisImpl::create_LV_cropper_from_features(float LV_mask_amplify,
										      C2DBounds& crop_start, 
										      const string& save_features)const 
{
	const char *segment_filter_chain[] = {
		"close:shape=[sphere:r=2]",
		"open:shape=[sphere:r=2]",
		"kmeans:c=7",
		"binarize:min=6,max=6", 
		"label"
	};

	P2DImage RV_candidates = run_filter_chain(_M_ica->get_feature_image(_M_cls.get_RV_idx()),
						  sizeof(segment_filter_chain)/sizeof(const char*), 
						  segment_filter_chain);
	
	P2DImage RV = run_filter(*RV_candidates, "selectbig"); 
	size_t npixels = ::mia::filter(GetRegionSize(1), *RV);
	
	if (10 * npixels > RV->get_size().x * RV->get_size().y)
		return C2DFilterPlugin::ProductPtr();

	C2DFVector RV_center = ::mia::filter(GetRegionCenter(), *RV);

	P2DImage LV_candidates = run_filter_chain(_M_ica->get_feature_image(_M_cls.get_LV_idx()),
						  sizeof(segment_filter_chain)/sizeof(const char*), 
						  segment_filter_chain);

	int label = ::mia::filter(GetClosestRegionLabel(RV_center), *LV_candidates);
	size_t lv_pixels = ::mia::filter(GetRegionSize(label), *LV_candidates);

	if (10 * lv_pixels > RV->get_size().x * RV->get_size().y)
		return C2DFilterPlugin::ProductPtr();

	stringstream binarize_lv;
	binarize_lv << "binarize:min="<< label << ",max=" << label;
	cvinfo() << "LV label=  " << label << "\n";
	P2DImage LV = run_filter(*LV_candidates, binarize_lv.str().c_str());

	C2DFVector LV_center = ::mia::filter(GetRegionCenter(), *LV);

	cvinfo() << "RV center = " << RV_center << "\n";
	cvinfo() << "LV center = " << LV_center << "\n";

	float r = LV_mask_amplify * (LV_center - RV_center).norm();
	cvinfo() << "r = " << r << "\n";
	stringstream mask_lv;
	crop_start = C2DBounds(int(LV_center.x - r), int(LV_center.y - r));

	// this is ugly and should be replaced
	if (crop_start.x > LV_center.x ||crop_start.y > LV_center.y)
		return C2DFilterPlugin::ProductPtr();

	mask_lv << "crop:start=[" << crop_start
		<< "],end=[" << C2DBounds(int(LV_center.x + r), int(LV_center.y + r)) << "]";
	cvmsg() << "crop region = '" << mask_lv.str() << "'\n";

	if (!save_features.empty()) {
		save_feature(save_features, "RVic", *_M_ica->get_feature_image(_M_cls.get_RV_idx())); 
		save_feature(save_features, "LVic", *_M_ica->get_feature_image(_M_cls.get_LV_idx())); 
		save_feature(save_features, "RV_candidates", *RV_candidates); 
		save_feature(save_features, "LV_candidates", *LV_candidates); 
		save_feature(save_features, "LV", *LV); 
		save_feature(save_features, "RV", *RV); 
		save_coefs(save_features + ".txt", *_M_ica); 
	}

	return C2DFilterPluginHandler::instance().produce(mask_lv.str().c_str());
}


template <typename T>
int GetClosestRegionLabel::operator() (const T2DImage<T>& image) const
{
	map<int, Collector> collector_map;
	typedef map<int, Collector>::iterator coll_iterator;
	typename T2DImage<T>::const_iterator i = image.begin();
	for (size_t y = 0; y < image.get_size().y; ++y)
		for (size_t x = 0; x < image.get_size().x; ++x, ++i) {
			if (!*i)
				continue;
			coll_iterator ic = collector_map.find(*i);
			if (ic ==  collector_map.end()) {
				Collector col;
				col.size = 1;
				col.center = C2DFVector(x,y);
				collector_map[*i] = col;
			} else {
				++ic->second.size;
				ic->second.center.x += x;
				ic->second.center.y += y;
			}
		}

	float min_weighted_distance = numeric_limits<float>::max();
	int label = 0;
	for (coll_iterator i = collector_map.begin();
	     i != collector_map.end(); ++i) {
		i->second.center /= i->second.size;
		float wdist = (i->second.center - m_point).norm() / i->second.size;
		if ( min_weighted_distance > wdist ) {
			min_weighted_distance = wdist;
			label = i->first;
		}
	}
	return label;
};


void C2DPerfusionAnalysisImpl::save_feature(const string& base, const string& feature, const C2DImage& image)const
{
	save_image2d(base + feature + ".png", run_filter(image, "convert")); 
}

TDictMap<C2DPerfusionAnalysis::EBoxSegmentation>::Table segmethod_table[] ={
	{"delta-feature", C2DPerfusionAnalysis::bs_delta_feature},
	{"delta-peak", C2DPerfusionAnalysis::bs_delta_peak},
	{"features", C2DPerfusionAnalysis::bs_features},      
	{NULL, C2DPerfusionAnalysis::bs_unknown}
};

TDictMap<C2DPerfusionAnalysis::EBoxSegmentation> C2DPerfusionAnalysis::segmethod_dict(segmethod_table); 

NS_MIA_END
