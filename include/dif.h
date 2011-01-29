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

#include <map>

FIELD3D_NAMESPACE_OPEN

#define _DIF_TYPE SparseField<T>

template<typename T> class DifField : public SparseField<T> {
	public:
		typedef boost::intrusive_ptr<DifField> Ptr;

		DifField(const V2i& size);
		DifField(const DifField<T>& o);
		DifField(const _DIF_TYPE& o);
		~DifField();

		DifField& operator=(const DifField<T>& o);

		bool writePixel(const V2i& pos, const T data, unsigned int dpt = 0);
		T readPixel(const V2i& pos, unsigned int dpt = 0, bool *retval = NULL);

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
	_DIF_TYPE::clear(T(0));
}

template<typename T> DifField<T>::DifField(const DifField<T>& o) : _DIF_TYPE(o), m_vSize(o.m_vSize) {
	// Nothing
}

template<typename T> DifField<T>::DifField(const _DIF_TYPE& o) : _DIF_TYPE(o), m_vSize(0) {
	m_vSize = _DIF_TYPE::dataResolution();
}

template<typename T> DifField<T>::~DifField() {

}

template<typename T> DifField<T>& DifField<T>::operator=(const DifField<T>& o) {
	_DIF_TYPE::operator=(o);

	m_vSize = o.m_vSize;

	return *this;
}

template<typename T> T DifField<T>::readPixel(const V2i& pos, unsigned int dpt, bool *retval) {
	if(m_vSize.x <= pos.x || m_vSize.y <= pos.y || m_vSize.z <= dpt) {
		if(retval != NULL) {
			(*retval) = false;
		}
		
		return T(0);
	}

	if(retval != NULL) {
		(*retval) = true;
	}

	return _DIF_TYPE::value(pos.x, pos.y, dpt);
}

template<typename T> int DifField<T>::depth() const {
	return m_vSize.z;
}

template<typename T> bool DifField<T>::writePixel(const V2i& pos, const T data, unsigned int dpt) {
	if(pos.x >= m_vSize.x || pos.y >= m_vSize.y) {
		return false;
	}

	if(depth() <= dpt) {
		m_vSize.z = dpt+1;

		SparseField<T> tmp(*this);

		_DIF_TYPE::setSize(m_vSize);

		V3i dims = tmp.dataResolution();

		for(int i = 0; i < dims.x; i++) {
			for(int j = 0; j < dims.y; j++) {
				for(int k = 0; k < dims.z; k++) {
					T handle = tmp.value(i, j, k);

					// So we don't waste much RAM
					if(handle != T(0)) {
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



template<typename T> class DifImage {
	public:
		DifImage(const V2i& size);
		~DifImage();

		bool addChannel(const std::string& name, const DifField<T>& i, unsigned int& retid);
		bool addChannel(const std::string& name, unsigned int& retid);

		unsigned int numberOfChannels() const;

		void save(Field3DOutputFile& ofp);

		const std::string& channelName(unsigned int idx) const;
		
	private:
		typedef std::map<std::string, DifField<T> * > ChannelList;
		typedef typename std::map<std::string, DifField<T> * >::iterator ChannelListIter;

		ChannelList m_lChannels;
		V3i m_vSize;
};

template<typename T> DifImage<T>::DifImage(const V2i& size) {
	m_vSize.x = size.x;
	m_vSize.y = size.y;
	m_vSize.z = 1;
}

template<typename T> DifImage<T>::~DifImage() {
	ChannelListIter it;
	
	for(it = m_lChannels.begin(); it != m_lChannels.end(); it++) {
		//std::cout << "." << std::endl;
		//delete (it->second);
	}

	m_lChannels.clear();
}

template<typename T> bool DifImage<T>::addChannel(const std::string& name, const DifField<T>& i, unsigned int& retid) {
	if(i.getSize().x != m_vSize.x || i.getSize().y != m_vSize.y) {
		return false;
	}

	if(m_lChannels.find(name) != m_lChannels.end()) {
		return false;
	}

	DifField<T> * handle = new DifField<T>(i);

	m_lChannels[name] = handle;

	return true;
}

template<typename T> bool DifImage<T>::addChannel(const std::string& name, unsigned int& retid) {
	if(m_lChannels.find(name) != m_lChannels.end()) {
		return false;
	}

	DifField<T> * handle = new DifField<T>(V2i(m_vSize.x, m_vSize.y));

	m_lChannels[name] = handle;

	return (numberOfChannels() - 1);
}

template<typename T> const std::string& DifImage<T>::channelName(unsigned int idx) const {
	if(idx >= numberOfChannels()) {
		return std::string();
	}
	
	ChannelListIter it = m_lChannels.begin();

	std::advance(it, idx);

	return it.first;
}

template<typename T> unsigned int DifImage<T>::numberOfChannels() const {
	return m_lChannels.size();
}

template<typename T> void DifImage<T>::save(Field3DOutputFile& ofp) {
	bool scalar = true;

	ChannelListIter it = m_lChannels.begin();

//	if(field_dynamic_cast< SparseField<T> >(it->second)) {
//		scalar = true;
//	}

	for(; it != m_lChannels.end(); it++) {
		typename DifField<T>::Ptr ptr = (it->second);

		if(scalar) {
			ofp.writeScalarLayer<T>(std::string(it->first), ptr);
		} else {
			//ofp.writeVectorLayer<T>(std::string(it->first), typename DifField<T>::Ptr((it->second)));
		}
	}
}

#undef _DIF_TYPE
FIELD3D_NAMESPACE_HEADER_CLOSE 

#endif // DIF_H
