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

/*
  LatexBeginPluginDescription{2D neighborhood shapes}


  \subsection{Basic shapes}
  \label{shapes2d:basic}

  \begin{description}
   
   \item [Plugin:] 1n, 4n, 8n 
   \item [Description:] provide the simple neighbourhood shapes, 'o' indicates the center (that belongs to the shape) 
       and 'x' the additional elements. 
   \begin{itemize}
   \item 1n 
   \begin{tabular}{|c|}
   \hline 
   x \\
   \hline 
   \end{tabular}
   \item 4n 
     \begin{tabular}{|c|c|c|}
     \hline 
     & x & \\
     \hline 
     x & o & x \\
     \hline 
     & x & \\
     \hline 
     \end{tabular}
   \item 8n 
     \begin{tabular}{|c|c|c|}
     \hline 
     x & x & x\\
     \hline 
     x & o & x \\
     \hline 
     x & x & x\\
     \hline 
     \end{tabular}
   \end{itemize}
   
   \end{description}
   These plug-ins don't take parameters. 
   
  LatexEnd
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits>
#include <mia/2d/shape.hh>
#include <mia/2d/shapes/basic_shapes.hh>

NS_BEGIN(basic_2dshape_creator)
NS_MIA_USE;
using namespace std;

C1n2DShape::C1n2DShape()
{
	insert(C2DShape::Flat::value_type( 0, 0));
}	

C1n2DShapeFactory::C1n2DShapeFactory():
	C2DShapePlugin("1n")
{
}	

const string C1n2DShapeFactory::do_get_descr() const
{
	return "A shape that only contains the central point";  
}

C2DShape *C1n2DShapeFactory::do_create()const
{
	return new C1n2DShape();
}


C4n2DShape::C4n2DShape()
{
	insert(C2DShape::Flat::value_type(-1, 0));
	insert(C2DShape::Flat::value_type( 1, 0));
	insert(C2DShape::Flat::value_type( 0,-1));
	insert(C2DShape::Flat::value_type( 0, 1));
}

C4n2DShapeFactory::C4n2DShapeFactory():
	C2DShapePlugin("4n")
{
}

C2DShape *C4n2DShapeFactory::do_create()const
{
	return new C4n2DShape();
}


const string C4n2DShapeFactory::do_get_descr()const
{
	return string("4n neighborhood 2D shape");
}


C8n2DShape::C8n2DShape()
{
	insert(C2DShape::Flat::value_type(-1,-1));
	insert(C2DShape::Flat::value_type(-1, 1));
	insert(C2DShape::Flat::value_type( 1,-1));
	insert(C2DShape::Flat::value_type( 1, 1));
}

C8n2DShapeFactory::C8n2DShapeFactory():
	C2DShapePlugin("8n")
{
}

C2DShape *C8n2DShapeFactory::do_create()const
{
	return new C8n2DShape();
}


const string C8n2DShapeFactory::do_get_descr()const
{
	return string("8n neighborhood 2D shape");
}

extern "C" EXPORT CPluginBase *get_plugin_interface()
{
	CPluginBase *p = new C1n2DShapeFactory();
	p->append_interface(new C4n2DShapeFactory());
	p->append_interface(new C8n2DShapeFactory());
	return p;
}

NS_END
// end namespace _8n_2dshape_creator
