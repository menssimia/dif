/*
 * Copyright (C) 2010, Jan Adelsbach and other authors and contributors
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the software's owners nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef DIF_H
#define DIF_H

#include <Field3D/Field3DFile.h>
#include <Field3D/SparseField.h>

FIELD3D_NAMESPACE_OPEN

#define _DIF_TYPE SparseField< FIELD3D_VEC3_T<T> >

template<typename T> class DifField : public SparseField< FIELD3D_VEC3_T<T> > {
	public:
		typedef boost::intrusive_ptr<DifField> Ptr;

		DifField(const V2i& size);
		DifField(const DifField<T>& o);
		DifField(const _DIF_TYPE& o);

		DifField& operator=(const DifField<T>& o);

		bool writePixel(const V2i& pos, const FIELD3D_VEC3_T<T> data, unsigned int dpt = 0);
		FIELD3D_VEC3_T<T> readPixel(const V2i& pos, unsigned int dpt = 0, bool *retval = NULL);

		int depth() const;

		const V3i& getSize() const;

	protected:
		virtual void sizeChanged();

	private:
		V3i   m_vSize; // So we dont need recopmputation through dataResolution()
};

template<typename T> DifField<T>::DifField(const V2i& size) : _DIF_TYPE() {
	m_vSize.x = size.x;
	m_vSize.y = size.y;

	m_vSize.z = 1;

	_DIF_TYPE::setSize(m_vSize);
	_DIF_TYPE::clear( FIELD3D_VEC3_T<T>(0) );
}

template<typename T> DifField<T>::DifField(const DifField<T>& o) : _DIF_TYPE(o), m_vSize(o.m_vSize) {
	// Nothing
}

template<typename T> DifField<T>::DifField(const _DIF_TYPE& o) : _DIF_TYPE(o), m_vSize(0) {
	m_vSize = _DIF_TYPE::dataResolution();
}

template<typename T> DifField<T>& DifField<T>::operator=(const DifField<T>& o) {
	_DIF_TYPE::operator=(o);

	m_vSize = o.m_vSize;

	return *this;
}

template<typename T> FIELD3D_VEC3_T<T> DifField<T>::readPixel(const V2i& pos, unsigned int dpt, bool *retval) {
	if(m_vSize.x <= pos.x || m_vSize.y <= pos.y || m_vSize.z <= dpt) {
		if(retval != NULL) {
			(*retval) = false;
		}
		
		return FIELD3D_VEC3_T<T> (0);
	}

	if(retval != NULL) {
		(*retval) = true;
	}

	return _DIF_TYPE::value(pos.x, pos.y, dpt);
}

template<typename T> int DifField<T>::depth() const {
	return m_vSize.z;
}

template<typename T> bool DifField<T>::writePixel(const V2i& pos, const FIELD3D_VEC3_T<T> data, unsigned int dpt) {
	if(pos.x >= m_vSize.x || pos.y >= m_vSize.y) {
		return false;
	}

	if(depth() <= dpt) {
		m_vSize.z = dpt+1;

		SparseField< FIELD3D_VEC3_T<T> > tmp(*this);

		_DIF_TYPE::setSize(m_vSize);

		V3i dims = tmp.dataResolution();

		for(int i = 0; i < dims.x; i++) {
			for(int j = 0; j < dims.y; j++) {
				for(int k = 0; k < dims.z; k++) {
					FIELD3D_VEC3_T<T> handle = tmp.value(i, j, k);

					// So we don't waste much RAM
					if(handle.x != 0.0f || handle.y != 0.0f || handle.z != 0.0f) {
						writePixel(V2i(i, j), handle, k);
					}
				}
			}
		}
	}

	_DIF_TYPE::lvalue(pos.x, pos.y, dpt) = data;
}

template<typename T> const V3i& DifField<T>::getSize() const {
	return m_vSize;
}

/* Protected */ template<typename T> void DifField<T>::sizeChanged() {
	m_vSize = _DIF_TYPE::dataResolution();

	_DIF_TYPE::sizeChanged();
}

#undef _DIF_TYPE

FIELD3D_NAMESPACE_HEADER_CLOSE 

#endif // DIF_H
