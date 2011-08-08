/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2011
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

#include <limits>
#include <boost/algorithm/minmax_element.hpp>
#include <mia/core/msgstream.hh>
#include <mia/core/meanvar.hh>

NS_MIA_BEGIN

template <class Image>
TConvert<Image>::TConvert(EPixelType pt, EPixelConversion ct, float a, float b):
	m_pt(pt), 
	m_ct(ct), 
	m_a(a), 
	m_b(b)
{
}

template <typename T, bool is_float> 
struct __get_range {
	static double apply() {
		return double(numeric_limits<T>::max()) - double(numeric_limits<T>::min()) + 1; 
	}
};

template <typename T> 
struct __get_range<T, true> {
	static double apply() {
		return 2.0; 
	}
};

template <> 
struct __get_range<bool, false> {
	static double apply() {
		return 1; 
	}
};



template <typename  T, bool is_float>
struct __dispatch_min {
	static double apply(long double x) {
		return (x > numeric_limits<T>::min()) ? x : numeric_limits<T>::min(); 
	}
}; 

template <typename  T>
struct __dispatch_min<T, true> {
	static double apply(long double x) {
		return (x > -numeric_limits<T>::max()) ? x : -numeric_limits<T>::max(); 
	}
}; 


template <typename  T, typename  S>
struct  FPixelConverter {

	FPixelConverter(long double a, long double mx, long double my):m_a(a), m_mx(mx), m_my(my)
	{
	}
	
	T operator() (const S x)
	{
		long double y = m_a * (x - m_mx) + m_my; 
		y = __dispatch_min<T, std::is_floating_point<T>::value>::apply(y); 
		if (y > std::numeric_limits<T>::max())
			return std::numeric_limits<T>::max(); 
		return __mia_round<T, std::is_floating_point<T>::value>::apply(y); 
	}

private:
	long double m_a, m_mx, m_my; 
}; 


template <class Image>
template <template  <typename> class Data, typename  S, typename  T>
typename TConvert<Image>::result_type TConvert<Image>::convert(const Data<S>& src) const
{
	TRACE("TConvert::convert"); 

	long double a = 1.0, mx, my; 
	Data<T> *result = new Data<T>(src.get_size(), src.get_attribute_list()); 

	
	
	switch (m_ct) {
	case pc_copy:
		a = 1.0; 
		mx = 0.0; 
		my = 0.0; 
		break; 
	case pc_range: {
		pair<S, S> src_minmax = get_minmax<S>::apply(); 
		pair<T, T> trgt_minmax = get_minmax<T>::apply(); 

		cvdebug() << "src_minmax = (" << src_minmax.first << ", " << src_minmax.second << ")\n"; 
		cvdebug() << "trgt_minmax = (" << trgt_minmax.first << ", " << trgt_minmax.second << ")\n"; 

		a = __get_range<T, std::is_floating_point<T>::value>::apply() / 
			__get_range<S, std::is_floating_point<S>::value>::apply(); 

		mx = src_minmax.first;
		my = trgt_minmax.first;
		break; 
	}
	case pc_opt: {
		pair<T, T> trgt_minmax = get_minmax<T>::apply(); 
		pair<typename T2DImage<S>::const_iterator, typename T2DImage<S>::const_iterator> 
			src_minmax = ::boost::minmax_element(src.begin(), src.end()); 
		
		cvdebug() << "src_minmax = (" << *src_minmax.first << ", " << *src_minmax.second << ")\n"; 
		cvdebug() << "trgt_minmax = (" << trgt_minmax.first << ", " << trgt_minmax.second << ")\n"; 

		if (*src_minmax.second != *src_minmax.first) {
			a = __get_range<T, std::is_floating_point<T>::value>::apply() / 
				(double(*src_minmax.second) - double(*src_minmax.first)); 
			my = trgt_minmax.first; 
			mx = *src_minmax.first; 
		} else {
			cvwarn() << "CConvert2D: Input image has only one value\n"; 
			a = mx = my = 0; 
		}
		break; 
	}
	case pc_opt_stat: {
		const pair<T, T> trgt_minmax = get_minmax<T>::apply(); 
		const double q_trgt_range = 0.25 * (trgt_minmax.second - trgt_minmax.first); 
		const auto meanvar = mean_var(src.begin(), src.end()); 
		if (meanvar.second > 0) 
			a = q_trgt_range / meanvar.second; 
		mx = -meanvar.first; 
		my = 0.5 * (trgt_minmax.second + trgt_minmax.first);
		break; 
	}
	default: 
		a = m_a; 
		mx = 0.0f; 
		my = m_b; 
	}
	cvdebug() << "a=" << a << ", mx=" << mx << ", my= "<< my << '\n'; 
	FPixelConverter<T,S> cv(a, mx, my);
	
	transform(src.begin(), src.end(), result->begin(), cv); 
	
	return typename TConvert<Image>::result_type(result); 
}

template <class Image>
template <template  <typename> class Data, typename T>
typename TConvert<Image>::result_type TConvert<Image>::operator () (const Data<T>& data) const
{
	TRACE("TConvert<Image>::operator ()"); 
	
	switch (m_pt) {
	case it_bit:   return convert<Data, T, bool>(data); 
	case it_sbyte: return convert<Data, T, signed char>(data); 
	case it_ubyte: return convert<Data, T, unsigned char>(data); 
	case it_sshort:return convert<Data, T, signed short>(data); 
	case it_ushort:return convert<Data, T, unsigned short>(data); 
	case it_sint:  return convert<Data, T, signed int>(data); 
	case it_uint:	 return convert<Data, T, unsigned int>(data); 
#ifdef HAVE_INT64
	case it_slong: return convert<Data, T, mia_int64>(data); 
	case it_ulong: return convert<Data, T, mia_uint64>(data); 
#endif
	case it_float: return convert<Data, T, float>(data); 
	case it_double:return convert<Data, T, double>(data); 
	default:
		assert(!"unsupported pixel type in image"); 
		// dummy to avoid a warning
		return typename TConvert<Image>::result_type(); 
	}
}

template <class Image>
typename TConvert<Image>::result_type TConvert<Image>::do_filter(const Image& image) const
{
	return mia::filter(*this, image); 
}


template <class Image>
TConvertFilterPlugin<Image>::TConvertFilterPlugin():
	TImageFilterPlugin<Image>("convert"), 
	m_pixeltype("ubyte"), 
	m_convert("opt"),  
	m_a(1.0), 
	m_b(0.0)
{
	this->add_parameter("a", new CFloatParameter(m_a, -std::numeric_limits<float>::max(), 
					       std::numeric_limits<float>::max(), false, 
					       "linear conversion parameter a")); 
	this->add_parameter("b", new CFloatParameter(m_b, -std::numeric_limits<float>::max(), 
					       std::numeric_limits<float>::max(), false, 
					       "linear conversion parameter b")); 
	this->add_parameter("repn",new CStringParameter(m_pixeltype, false, "output pixel type")); 
	this->add_parameter("map", new CStringParameter(m_convert, false, "conversion mapping"));

}

template <class Image>
TImageFilter<Image> *TConvertFilterPlugin<Image>::do_create()const
{
	EPixelType pt = CPixelTypeDict.get_value(m_pixeltype.c_str());
	EPixelConversion ct = CPixelConversionDict.get_value(m_convert.c_str()); 

	if (pt == it_bit)
		throw invalid_argument("TConvert: for conversion to bit images you better use the 'binarize' filter"); 

	return new TConvert<Image>(pt,ct,m_a,m_b);
}

template <class Image>
const std::string TConvertFilterPlugin<Image>::do_get_descr()const
{
	return "image pixel format conversion filter";
}

NS_MIA_END
