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

#include <cassert>
#include <vector>

namespace gsl {
template <typename T> 
TVector<T>::TVector():
	data(NULL), 
	cdata(NULL), 
	owner(false)
{
}

template <typename T> 
TVector<T>::TVector(size_type n, bool clear):
	owner(true)
{
	cdata = data = (vector_type*) (clear ? this->calloc (n) : this->alloc (n));  
}
	
template <typename T> 
TVector<T>::TVector(const TVector<T>& other):
	owner(true)
{
	assert(other.data); 
	
	cdata = data = this->alloc(other.data->size); 
	std::copy(other.begin(), other.end(), begin());
}

template <typename T> 
TVector<T>& TVector<T>::operator = (const TVector<T>& other)
{
	if (&other != this) {
		vector_type *d =  this->alloc(other.data->size); 
		std::copy(other.begin(), other.end(), d->data);
		if (owner)  
			this->free(data); 
		cdata = data = d; 
		owner = true; 
	}
	return *this; 
}

template <typename T> 
TVector<T>::TVector(vector_type *holder):
	data(holder), 
	cdata(holder),
	owner(false)
{
}

template <typename T> 
TVector<T>::TVector(const vector_type *holder):
	data(NULL), 
	cdata(holder),
	owner(false)
{
}

template <typename T> 
TVector<T>::~TVector()
{
	if (owner) 
		this->free(data);
}

template <typename T> 
TVector<T>::operator const typename TVector<T>::vector_type *() const
{
	return cdata; 
}

template <typename T> 
TVector<T>::operator typename TVector<T>::vector_pointer_type ()
{
	return data; 
}

template <typename T> 
typename TVector<T>::iterator TVector<T>::begin()
{
	assert(data); 
	return data->data; 
}

template <typename T> 
typename TVector<T>::iterator TVector<T>::end()
{
	assert(data); 
	return data->data + data->size; 
}


template <typename T> 
typename TVector<T>::const_iterator TVector<T>::begin()const
{
	assert(cdata); 
	return cdata->data; 
}


template <typename T> 
typename TVector<T>::const_iterator TVector<T>::end()const
{
	assert(cdata); 
	return cdata->data + cdata->size; 
}


template <typename T> 
const typename TVector<T>::value_type TVector<T>::operator[](size_t i)const
{
	assert(cdata); 
	return this->get (cdata, i); 
}

template <typename T> 
typename TVector<T>::size_type TVector<T>::size() const
{
	return this->cdata->size; 
}

}
