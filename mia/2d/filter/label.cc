/* -*- mia-c++  -*-
 * Copyright (c) Leipzig, Madrid 2004-2010
 * Biomedical Image Technologies, Universidad Politecnica de Madrid
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

#include <stdexcept>
#include <queue>
#include <limits>

#include <mia/core/filter.hh>
#include <mia/core/msgstream.hh>
#include <mia/2d/filter/label.hh>




NS_BEGIN(label_2dimage_filter)

NS_MIA_USE;
using namespace std;
using namespace boost;
namespace bfs=boost::filesystem;



CLabel::CLabel(P2DShape& mask):
	m_mask(mask)
{
}

void CLabel::grow_region(const C2DBounds& loc, const C2DBitImage& input, C2DUSImage& result, unsigned short label)const
{
	queue<C2DBounds> neighbors;
	neighbors.push(loc);
	result(loc) = label;
	C2DBounds size = result.get_size();

	while (!neighbors.empty()) {
		C2DBounds  l = neighbors.front();

		neighbors.pop();
		for (C2DShape::const_iterator s = m_mask->begin(); s != m_mask->end(); ++s) {
			C2DBounds  pos(l.x + s->x, l.y + s->y);
			if (pos.x < size.x && pos.y < size.y  && input(pos) && result(pos) == 0) {
				result(pos) = label;
				neighbors.push(pos);
			}
		}
	}
}

CLabel::result_type CLabel::do_filter(const C2DImage& image) const
{
	if (image.get_pixel_type() != it_bit){
		throw invalid_argument("Label: Only bit input images are allowed");
	}

	const C2DBitImage& input = dynamic_cast<const C2DBitImage&>(image);

	unsigned short current_label = 1;
	C2DUSImage *result = new C2DUSImage(image.get_size(), image.get_attribute_list());
	P2DImage presult(result);
	fill(result->begin(), result->end(), 0);

	C2DBitImage::const_iterator  i = input.begin();
	C2DUSImage::const_iterator   r = result->begin();

	C2DBounds size(image.get_size());
	C2DBounds loc;

	for (loc.y = 0; loc.y < size.y; ++loc.y)
		for (loc.x = 0; loc.x < size.x; ++loc.x, ++i, ++r) {
			if (*i && !*r) {
				cvdebug() << "label:" << current_label << " seed at " << loc << "\n";
				grow_region(loc, input, *result, current_label);
				++current_label;

			}
		}

	cvmsg() << "\n";
	if (current_label < 257) {
		C2DUBImage *real_result = new C2DUBImage(image.get_size(), image);
		copy(result->begin(), result->end(), real_result->begin());
		presult.reset(real_result);
	}
	return presult;
}


class C2DLabelFilterPlugin: public C2DFilterPlugin {
public:
	C2DLabelFilterPlugin();
private:
	virtual C2DFilterPlugin::ProductPtr do_create()const;
	virtual const string do_get_descr()const;

	virtual bool do_test() const;
	virtual void prepare_path() const;


	string m_mask_descr;

};


C2DLabelFilterPlugin::C2DLabelFilterPlugin():
	C2DFilterPlugin("label"),
	m_mask_descr("4n")
{
	add_parameter("n", new CStringParameter(m_mask_descr, false, "neighborhood mask")) ;
}

C2DFilterPlugin::ProductPtr C2DLabelFilterPlugin::do_create()const
{
	P2DShape mask = C2DShapePluginHandler::instance().produce(m_mask_descr.c_str());
	if (!mask)
		return C2DFilterPlugin::ProductPtr();
	return C2DFilterPlugin::ProductPtr(new CLabel(mask));
}

const string C2DLabelFilterPlugin::do_get_descr()const
{
	return "a 2D conected component labeler";
}

void C2DLabelFilterPlugin::prepare_path() const
{
	list< bfs::path> kernelsearchpath;
	kernelsearchpath.push_back(bfs::path("..")/bfs::path("shapes"));
	C2DShapePluginHandler::set_search_path(kernelsearchpath);

}



bool  C2DLabelFilterPlugin::do_test() const
{
	return true;
}

extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	return new C2DLabelFilterPlugin();
}


NS_END

