/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2012 Gert Wollny
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


#ifndef mia_3d_landmarks_hh
#define mia_3d_landmarks_hh

#include <memory>
#include <vector>
#include <boost/filesystem.hpp>
#include <mia/3d/defines3d.hh>
#include <mia/3d/camera.hh>
#include <mia/2d/2DImage.hh>

NS_MIA_BEGIN

/**
   @ingroup basic 
   \brief a named 3D landmark

   This class provides the basis for named a 3D landmark 
 */

class EXPORT_3D C3DLandmark {
public: 

	/// Shared pointer type of the landmark 
	typedef std::shared_ptr<C3DLandmark> Pointer; 

	/**
	   constructor to initialize the landmark. 
	   \param name Landmark identifier 
	   \param position 3D coordinate of the landmark 
	 */
	C3DLandmark(const std::string& name, const C3DFVector& position); 

	/// Standard copy constructor 
	C3DLandmark(const C3DLandmark& other) = default; 


	/**
	   \returns the identifier  of the landmark 
	 */
	const std::string& get_name() const; 

	/// \returns the position of the landmark 
	const C3DFVector& get_position() const; 


	/// \returns the view camera 
	const C3DCamera& get_view() const;
	
	
	/// \returns the file name of the picture illustrating the landmark 
	const std::string& get_picture_file()const; 

	/**
	   Get the picture illustrating the landmark. If no file name is given in the landmark, then 
	   an empty 
	   \param root_path root path for the landmark set, the picture is stored relative to this path  
	   \returns the picture - if available, otherwise returns an empty pointer
	*/
	P2DImage get_picture(const boost::filesystem::path& root_path) const; 

	/// \returns the intensity value expected for this landmark 
	float get_isovalue()const; 
	

	/**
	   (Re-)set the position of the landmark 
	   \param pos new position 
	 */
	void set_position(const C3DFVector& pos); 

	/** set view 
	    \param view 
	 */
	void set_view(const C3DCamera& view); 
	
	/**
	   set picture file name
	   \param picfile 
	*/
	void set_picture_file(const std::string& picfile); 

	/**
	   set the intensity value that correspondts to this landmark 
	   \param value 
	*/
	void set_isovalue(float value); 
	
private: 
	// required attributes 
	std::string m_name; 
	C3DFVector m_position; 

	// additional attributes
	std::string m_picfile;
	float m_isovalue; 
	C3DCamera m_view; 
	
	mutable P2DImage m_picture;
}; 


typedef C3DLandmark::Pointer P3DLandmark; 

NS_MIA_END

#endif

