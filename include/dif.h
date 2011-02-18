/*
 * Copyright (C) 2010, 2011 Jan Adelsbach and other authors and contributors
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
#include <Field3D/FieldInterp.h>

#include <map>
#include <vector>

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

		bool writePixel(const V2i& pos, unsigned int dpt, const T data);
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
		if(retval) {
			(*retval) = false;
		}
		
		return T(0);
	}

	if(retval) {
		(*retval) = true;
	}

	return _DIF_TYPE::value(pos.x, pos.y, dpt);
}

template<typename T> int DifField<T>::depth() const {
	return m_vSize.z;
}

template<typename T> bool DifField<T>::writePixel(const V2i& pos, unsigned int dpt, const T data) {
	if(pos.x >= m_vSize.x || pos.y >= m_vSize.y) {
		return false;
	}

	//std::cout << "Write: " << data << " dpt=" << dpt << std::endl;

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
						writePixel(V2i(i, j), k, handle);
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
		typedef boost::intrusive_ptr<DifImage> Ptr;

		DifImage(const V2i& size);
		~DifImage();

		bool addChannel(const std::string& name, const DifField<T>& i, unsigned int& retid);
		bool addChannel(const std::string& name, unsigned int& retid);

		unsigned int numberOfChannels() const;

		void save(Field3DOutputFile& ofp);
		bool load(Field3DInputFile& ifp);

		const std::string& channelName(unsigned int idx) const;

		float depthAtIndex(unsigned int idx, bool* retval = 0) const;
		unsigned int indexAtDepth(float dpt, bool* retval = 0) const;

		unsigned int depthLevels() const;

		enum DifImageInterpolation {
			eNone   = 0,
			eLinear = 1,
		};

		// data must be at least sizeof(T)*numberOfChannels()
		void writeData(const V2i& pos, float depth, T* data);
		bool readData(const V2i& pos, float depth, T *buffer, enum DifImageInterpolation type = eLinear);

		enum DifImageGetType {
			eBefore,
			eAfter
		};

		void getNearestDepthIndex(float dpt, enum DifImageGetType type, unsigned int& retid);

	protected:
		void loadDepthMapping(const SparseField<float>::Ptr field);
		
	private:
		typedef std::map<std::string, DifField<T> * > ChannelList;
		typedef typename std::map<std::string, DifField<T> * >::iterator ChannelListIter;

		ChannelList m_lChannels;
	
		typedef std::vector<float> DepthMappingList;
		typedef std::vector<float>::iterator DepthMappingListIter;

		DepthMappingList m_lDepthMapping;

		V3i m_vSize;

		static const char *m_scDepthMappingName;
};

template<typename T> const char * DifImage<T>::m_scDepthMappingName = "depthMapping";

/*!
 * @brief Assignment constructor
 *
 * @param[in] size The Initial size. Note that the size cannot be altered after
 *                 this point except for if you're loading a Dif file through
 *                 DifImage::load()
 */
template<typename T> DifImage<T>::DifImage(const V2i& size) {
	m_vSize.x = size.x;
	m_vSize.y = size.y;
	m_vSize.z = 1;
}

/// Default destructor
template<typename T> DifImage<T>::~DifImage() {
	m_lChannels.clear();
}

/*!
 * @brief Adds a channel by using an existing DifField
 * @param[in] name    Name of the channel
 * @param[in] i       A DifField to copy values from
 * @param[out] retid  Identification number of the channel
 * @retval true  Success
 * @retval false Size mismatch or channel of the same name already existing  
 */
template<typename T> bool DifImage<T>::addChannel(const std::string& name, const DifField<T>& i, unsigned int& retid) {
	if(i.getSize() != m_vSize) {
		return false;
	}

	if(m_lChannels.find(name) != m_lChannels.end()) {
		return false;
	}

	DifField<T> * handle = new DifField<T>(i);

	m_lChannels[name] = handle;

	return true;
}

/*!
 * @brief Adds a channel
 * @param[in] name Name of the channel
 * @param[out] retid Identification number of the channel
 * @retval true Success
 * @retval false Channel of the same name existing
 */
template<typename T> bool DifImage<T>::addChannel(const std::string& name, unsigned int& retid) {
	if(m_lChannels.find(name) != m_lChannels.end()) {
		return false;
	}

	DifField<T> * handle = new DifField<T>(V2i(m_vSize.x, m_vSize.y));

	m_lChannels[name] = handle;

	return (numberOfChannels() - 1);
}

/*!
 * @brief Returns the name of the channel at index @a idx
 * @param[in]  The Index (0..numberOfChannels()-1)
 * @return A String (empty if @a idx is out of range)
 */
template<typename T> const std::string& DifImage<T>::channelName(unsigned int idx) const {
	if(idx >= numberOfChannels() || m_lChannels.size() < 1) {
		return std::string();
	}
	
	ChannelListIter it = m_lChannels.begin();

	std::advance(it, idx);

	return it.first;
}

/// Returns the number of channels.
template<typename T> unsigned int DifImage<T>::numberOfChannels() const {
	return m_lChannels.size();
}

/*!
 * @brief Returns the associated depth value to a depth index
 * @param[in]  idx The Index (range 0..depthLevels()-1)
 * @param[out] retval (Optimal) Pointer to a bool for the return code
 * @return A float
 */
template<typename T> float DifImage<T>::depthAtIndex(unsigned int idx, bool* retval) const {
	if(idx >= m_lDepthMapping.size()) {
		if(retval) {
			(*retval) = false;
		}

		return 0.0f;
	}

	if(retval) {
		(*retval) = true;
	}

	return m_lDepthMapping.at(idx);
}

/*!
 * @brief Returns the index of the given depth
 * @param[in]  dpt The depth
 * @param[out] retval (Optimal) Pointer to a bool for the return code
 * @return An unsigned integer in range 0..depthLevels()-1 also 0 on error 
 */
template<typename T> unsigned int DifImage<T>::indexAtDepth(float dpt, bool* retval) const {

	for(unsigned int i = 0; i < m_lDepthMapping.size(); i++) {
		if(m_lDepthMapping.at(i) == dpt) {
			if(retval) {
				(*retval) = true;
			}

			return i;
		}
	}

	if(retval) {
		(*retval) = false;
	}

	return 0;
}

/// Returns the number of depth levels
template<typename T> unsigned int DifImage<T>::depthLevels() const {
	return m_lDepthMapping.size();
}

/*!
 * @brief Write Data to the image
 *
 * This function has no return value and will simply ignore the data if the given
 * Position @a pos is invalid. 
 * @param[in] pos The Position
 * @param[in] depth The depth level
 * @param[in] data The Data (must be at least sizeof(T)* numberOfChannels())
 */
template<typename T> void DifImage<T>::writeData(const V2i& pos, float depth, T* data) {
	unsigned int i   = 0;
	unsigned int idx = 0;
	bool status = false;

	idx = indexAtDepth(depth, &status);

	if(!status) {
		m_lDepthMapping.push_back(depth);
		idx = (m_lDepthMapping.size()-1);
	}

	ChannelListIter it = m_lChannels.begin();

	for(; it != m_lChannels.end(); it++, i++) {
		it->second->writePixel(pos, idx, data[i]);
	}
}

template<typename T> bool DifImage<T>::readData(const V2i& pos, float depth, T *buffer, enum DifImage<T>::DifImageInterpolation type) {
	unsigned int i = 0;

	// No channels available
	if(m_lChannels.size() == 0) {
		return false;
	}

	if(type == eNone) {
		unsigned int idx = 0;
		bool status = false;

		idx = indexAtDepth(depth, &status);

		// Must have at least one channel at the given depth
		if(!status) {
			return false;
		}

		ChannelListIter it = m_lChannels.begin();
	
		for(; it != m_lChannels.end(); it++, i++) {
			buffer[i] = it->second->readPixel(pos, idx);
		}
	}
	else if(type == eLinear) {
		unsigned int bfr, aftr;

		getNearestDepthIndex(depth, eBefore, bfr);
		getNearestDepthIndex(depth, eAfter,  aftr);

		if(bfr == aftr) {
			return readData(pos, depth, buffer, eNone);
		}

		T *a = new T[numberOfChannels()];
		T *b = new T[numberOfChannels()];

		ChannelListIter it = m_lChannels.begin();

		for(; it != m_lChannels.end(); it++, i++) {
			a[i] = it->second->readPixel(pos, bfr);
			b[i] = it->second->readPixel(pos, aftr);
		}

		{
			float d_bfr  = depthAtIndex(bfr);
			float d_aftr = depthAtIndex(aftr);

			float dt = d_aftr - d_bfr;
			float t  = (depth - d_bfr) / dt;

//			std::cout << "DepthBefore: " << d_bfr << std::endl
//					  << "DepthAfter: " << d_aftr << std::endl
//					  << "dt: " << dt << " t: " << t << std::endl;

			for(unsigned long i = 0; i < numberOfChannels(); i++) {
				buffer[i] = Imath::lerp(a[i], b[i], t);
			}
		}

		delete[] a;
		delete[] b;
	}

	return true;
}

template<typename T> void DifImage<T>::save(Field3DOutputFile& ofp) {
//	bool scalar = true;

	ChannelListIter it = m_lChannels.begin();

//	if(field_dynamic_cast< SparseField<T> >(it->second)) {
//		scalar = true;
//	}


	{
		SparseField<float>::Ptr dptmapping = new SparseField<float>();
		dptmapping->setSize(V3i(1, 1, m_lDepthMapping.size()));

		DepthMappingListIter dit;
		unsigned int i = 0;

		for(dit = m_lDepthMapping.begin(); dit != m_lDepthMapping.end(); dit++) {
			dptmapping->lvalue(0, 0, i) = (*dit);
			++i;

			//std::cout << "[" << i << "] = " << (float)*dit << std::endl;
		}

		ofp.writeScalarLayer<float>(m_scDepthMappingName, dptmapping);

	}

	for(; it != m_lChannels.end(); it++) {
		typename DifField<T>::Ptr ptr = (it->second);

		ofp.writeScalarLayer<T>(std::string(it->first), ptr);	
	}
}

template<typename T> void DifImage<T>::loadDepthMapping(const SparseField<float>::Ptr field) {
	V3i dptDim = field->dataResolution();

	for(int i = 0; i < dptDim.z; i++) {
		m_lDepthMapping.push_back(field->fastValue(0, 0, i));
	}
}

template<typename T> bool DifImage<T>::load(Field3DInputFile& ifp) {
	typedef typename Field< T >::Vec           FieldVector;
	typedef typename Field< T >::Vec::iterator FieldVectorIterator;
	typedef typename SparseField< T >::Ptr     SparseFieldPtr;

	// giving a layerName does not work for some reason so we look manually for our structure
	Field<float>::Vec dptMappings = ifp.readScalarLayers<float>();
	bool depthLoaded = false;

	if(dptMappings.size() < 1) {
		return false;
	}

	{
		Field<float>::Vec::iterator it;

		for(it = dptMappings.begin(); it != dptMappings.end();  it++) {
			if((*it)->name == m_scDepthMappingName) {
				SparseField<float>::Ptr depthField = field_dynamic_cast< SparseField<float> >(dptMappings[0]);

				if(!depthField) {
					return false;
				}

				loadDepthMapping(depthField);
				depthLoaded = true;

				break;
			}
		}
	}

	// No depth mapping available
	if(!depthLoaded) {
		return false;
	}

	

	FieldVector fields = ifp.readScalarLayers<T>();

	if(fields.size() < 1) {
		return false;
	}

	{
		FieldVectorIterator it;
		V3i initialSize;
		bool sizeSet = false;

		for(it = fields.begin(); it != fields.end();  it++) {
			if((*it)->name == m_scDepthMappingName || (*it)->name.length() == 0) {
				continue;
			}

			SparseFieldPtr handle = field_dynamic_cast< SparseField<T> >(*it);

			if(!handle) {
				continue;
			}

			if(!sizeSet) {
				m_vSize = handle->dataResolution();
				sizeSet = true;
			}

			// check for size mismatch
			if(m_vSize != handle->dataResolution()) {
				continue;
			}

			unsigned int retid = 0;

			addChannel((*it)->name, DifField<T>(*handle), retid);
		}
	
	}

	return (m_lChannels.size() > 0) ? true : false;
}

template<typename T> void DifImage<T>::getNearestDepthIndex(float dpt, DifImage<T>::DifImageGetType type, unsigned int& retid) {
	DepthMappingListIter it;

	float current     = 0.0f;
	unsigned int lidx = 0;
	unsigned int i    = 0;
	bool aset         = false;

	for(it = m_lDepthMapping.begin(); it != m_lDepthMapping.end(); it++, i++) {
		if((*it) == dpt) {
			retid = i;
			return;
		}

		if(type == eBefore) {
			if((*it) < dpt && (*it) > current) {
				lidx = i;
				current = *it;
			}
		} else {
			if((*it) > dpt) {
				if(!aset) {
					current = *it;
					lidx = i;
					aset = true;
				} else if ((*it) < current) {
					lidx = i;
					current = *it;
				}
			}
		}
	}

	retid = lidx;
}

#undef _DIF_TYPE
FIELD3D_NAMESPACE_HEADER_CLOSE 

#endif // DIF_H
