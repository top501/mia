/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 2004-2010
 * Max-Planck-Institute for Human Cognitive and Brain Science
 * Max-Planck-Institute for Evolutionary Anthropology
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


#ifndef mia_core_fullstats_hh
#define mia_core_fullstats_hh

#include <vector>
#include <iosfwd>
#include <mia/core/defines.hh>

NS_MIA_BEGIN

class CFullStats {
public:
	template <typename InputIterator>
	CFullStats(InputIterator begin, InputIterator end);

	void print(std::ostream& os) const;

	double mean()const;
	double sigma()const;
	double median()const;
	double max()const;
	double min()const;


private:
	typedef std::vector<double> Vector;
	void evaluate(Vector& tmp);
	double m_mean;
	double m_sigma;
	double m_median;
	double m_max;
	double m_min;
};

template <typename InputIterator>
CFullStats::CFullStats(InputIterator begin, InputIterator end):
	m_mean(0.0),
	m_sigma(0.0),
	m_median(0.0),
	m_max(*begin),
	m_min(*begin)
{

	Vector tmp;
	while (begin != end) {
		if (m_min > *begin)
			m_min = *begin;

		if (m_max < *begin)
			m_max = *begin;

		m_mean += *begin;
		m_sigma += *begin * *begin;

		tmp.push_back(*begin);
		++begin;
	}
	evaluate(tmp);
}

std::ostream& operator << (std::ostream& os, const CFullStats& stats)
{
	stats.print(os);
	return os;
}

NS_MIA_END

#endif
