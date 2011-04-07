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

// $Id: 2dimagefilter.cc,v 1.12 2006-07-12 13:44:23 wollny Exp $

/*! \brief mia-2dimagestats Evaluate the histogram across various images.

\author Gert Wollny <gw.fossdev at gmail.com>
*/

#include <sstream>
#include <mia/core.hh>
#include <mia/2d.hh>

NS_MIA_USE;
using namespace std;

const char *g_description = 
	"This progranm is used to evaluate some statistics of a series of images. " 
	"Output is Mean, Variation, Median, and Median Average Distance of the intensity values."
	; 
	
	

class CHistAccumulator : public TFilter<bool> {
public:
	CHistAccumulator(float min, float max, size_t bins, float thresh):
		m_histo(CHistogramFeeder<float>(min, max, bins)),
		m_thresh(thresh)
	{
	}

	template <typename T>
	bool operator () (const T2DImage<T>& image) {
		for (typename T2DImage<T>::const_iterator i = image.begin();
		     i != image.end(); ++i)
			if (*i > m_thresh)
				m_histo.push(*i);
		return true;
	}

	void print_stats()const
	{
		print_stats(m_histo);
	}

	void print_stats(double thresh_high)const
	{
		CHistogram<CHistogramFeeder<float > > tmp(m_histo, thresh_high);
		print_stats(tmp);
	}
private:
	void print_stats(const CHistogram<CHistogramFeeder<float > >& tmp)const
	{
		cout   <<  tmp.average() << " " << tmp.deviation()  <<  " "
		       << tmp.median() << " " << tmp.MAD() << '\n';
	}

	CHistogram<CHistogramFeeder<float > > m_histo;
	float m_thresh;
};



int main( int argc, const char *argv[] )
{

	string in_filename;
	float thresh = 10.0;
	float high_thresh = 0.05;
	try {

		const C2DImageIOPluginHandler::Instance& imageio = C2DImageIOPluginHandler::instance();


		CCmdOptionList options(g_description);
		options.push_back(make_opt( in_filename, "in-file", 'i', "input image(s) to be filtered", 
					    CCmdOption::required));
		options.push_back(make_opt( thresh, "thresh", 't', "intensity thresh to ignore"));
		options.push_back(make_opt( high_thresh, "high-thresh", 'g', "upper histogram percentage to ignore"));

		options.parse(argc, argv);

		//CHistory::instance().append(argv[0], "unknown", options);

		// read image
		C2DImageIOPluginHandler::Instance::PData  in_image_list = imageio.load(in_filename);

		if (in_image_list.get() && in_image_list->size()) {
			CHistAccumulator histo(0, 4096, 1024, thresh);
			for (C2DImageIOPluginHandler::Instance::Data::iterator i = in_image_list->begin();
			     i != in_image_list->end(); ++i)
				accumulate(histo, **i);
			histo.print_stats(high_thresh);
		}else
			throw runtime_error(string("No errors found in ") + in_filename);

		return EXIT_SUCCESS;

	}
	catch (const runtime_error &e){
		cerr << argv[0] << " runtime: " << e.what() << endl;
	}
	catch (const invalid_argument &e){
		cerr << argv[0] << " error: " << e.what() << endl;
	}
	catch (const exception& e){
		cerr << argv[0] << " error: " << e.what() << endl;
	}
	catch (...){
		cerr << argv[0] << " unknown exception" << endl;
	}

	return EXIT_FAILURE;
}
