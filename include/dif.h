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
		void setContainsData();
		
		void updateDepth(unsigned int dpt);
		
	protected:
		virtual void sizeChanged();
		
	private:
		V3i   m_vSize; // So we dont need recopmputation through dataResolution()
		bool  m_bHasData;
};

template<typename T> DifField<T>::DifField(const V2i& size) : _DIF_TYPE() {
	m_vSize.x = size.x;
	m_vSize.y = size.y;

	m_vSize.z = 1;

	m_bHasData = false;

	_DIF_TYPE::setSize(m_vSize);
	_DIF_TYPE::clear(T(0));
}

template<typename T> DifField<T>::DifField(const DifField<T>& o) 
	: _DIF_TYPE(o), m_vSize(o.m_vSize), m_bHasData(o.m_bHasData) {
	// Nothing
}

template<typename T> DifField<T>::DifField(const _DIF_TYPE& o) 
	: _DIF_TYPE(o), m_vSize(0), m_bHasData(true) {
	m_vSize = _DIF_TYPE::dataResolution();
}

template<typename T> DifField<T>::~DifField() {

}

template<typename T> DifField<T>& DifField<T>::operator=(const DifField<T>& o) {
	_DIF_TYPE::operator=(o);

	m_vSize = o.m_vSize;
	m_bHasData = o.m_bHasData;

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

	updateDepth(dpt);

	m_bHasData = true;
	_DIF_TYPE::lvalue(pos.x, pos.y, dpt) = data;
}

template<typename T> void DifField<T>::setContainsData() {
	m_bHasData = true;
}


template<typename T> const V3i& DifField<T>::getSize() const {
	return m_vSize;
}

template<typename T> void DifField<T>::updateDepth(unsigned int dpt) {
	if(depth() <= dpt) {
		m_vSize.z = dpt+1;

		if(m_bHasData) { 
			
			// If we have data copy it to a tmp SparseField
			SparseField<T> tmp(*this);

			_DIF_TYPE::setSize(m_vSize);
			
			V3i dims = tmp.dataResolution();

			// And Copy it back onto the resized field.
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
		} else {
			// The Field is empty, hence simply resize
			_DIF_TYPE::setSize(m_vSize);
		}
	}
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
		unsigned int channelIndex(const std::string& name, bool *retval=0);

		float depthAtIndex(unsigned int idx, bool* retval = 0) const;
		unsigned int indexAtDepth(float dpt, bool* retval = 0) const;
		
		void addDepth(float dpt, bool sync=true);

		bool validChannelId(unsigned int id) const;

		bool hasChannel(const std::string& name);

		unsigned int depthLevels() const;

		enum DifImageInterpolation {
			eNone     = 0,
			eLinear   = 1,
		};

		// data must be at least sizeof(T)*numberOfChannels()
		void writeData(const V2i& pos, float depth, T* data);
		bool readData(const V2i& pos, float depth, T *buffer, enum DifImageInterpolation type = eLinear);

		bool readChannelData(unsigned int channelid, const V2i& pos, float depth, T& retval, enum DifImageInterpolation type = eLinear);
		bool readChannelData(const std::string& channelname, const V2i& pos, float depth, T& retval, enum DifImageInterpolation type = eLinear);

		enum DifImageGetType {
			eBefore,
			eAfter
		};

		void getNearestDepthIndex(float dpt, enum DifImageGetType type, unsigned int& retid);

#ifndef _NEXCEPTIONS
		bool exceptionsEnabled() const;
		void setExceptionsEnabled(bool yorn);

		
		class DifException : public std::exception {
			public:
				DifException(const std::string& what) throw() : m_sWhat(what) {}
				~DifException() throw() {}
				virtual const char* what() const throw() {return m_sWhat.c_str();}
			private:
				std::string m_sWhat;
		};

		#define _THROW(what) \
			if(m_bExceptionsEnabled) {\
				throw DifException(#what);\
			}
#else
		#define _THROW(what)
#endif //_NEXCEPTIONS

	protected:
		void loadDepthMapping(const SparseField<float>::Ptr field);
	
		DifField<T>* getField(unsigned int channelid);
		DifField<T>* addChannelIntern(const std::string& name, const DifField<T>& i, unsigned int& retid);
		
	private:
		typedef std::map<std::string, DifField<T> * > ChannelList;
		typedef typename std::map<std::string, DifField<T> * >::iterator ChannelListIter;

		ChannelList m_lChannels;
	
		typedef std::vector<float> DepthMappingList;
		typedef std::vector<float>::iterator DepthMappingListIter;

		DepthMappingList m_lDepthMapping;

		V3i m_vSize;


		unsigned int m_ulChannelIndex;

#ifndef _NEXCEPTIONS
		bool m_bExceptionsEnabled;
#endif //_NEXCEPTIONS

		static const char *m_scDepthMappingName;
		static const char *m_scChannelIndexName;
};

template<typename T> const char * DifImage<T>::m_scDepthMappingName = "depthMapping";
template<typename T> const char * DifImage<T>::m_scChannelIndexName = "channelIndex";

/*!
 * @brief Assignment constructor
 *
 * @param[in] size The Initial size. Note that the size cannot be altered after
 *                 this point except for if you're loading a Dif file through
 *                 DifImage::load()
 */
template<typename T> DifImage<T>::DifImage(const V2i& size) : m_ulChannelIndex(0) {
	m_vSize.x = size.x;
	m_vSize.y = size.y;
	m_vSize.z = 1;

#ifndef _NEXCEPTIONS
	m_bExceptionsEnabled = false;
#endif //_NEXCEPTIONS
}

/// Default destructor
template<typename T> DifImage<T>::~DifImage() {
	m_lChannels.clear();
}

#ifndef _NEXCEPTIONS
template<typename T> bool DifImage<T>::exceptionsEnabled() const {
	return m_bExceptionsEnabled;
}

template<typename T> void DifImage<T>::setExceptionsEnabled(bool yorn) {
	m_bExceptionsEnabled = yorn;
}
#endif //_NEXCEPTIONS

/*!
 * @brief Adds a channel by using an existing DifField
 * @param[in] name    Name of the channel
 * @param[in] i       A DifField to copy values from
 * @param[out] retid  Identification number of the channel
 * @retval true  Success
 * @retval false Size mismatch or channel of the same name already existing  
 */
template<typename T> bool DifImage<T>::addChannel(const std::string& name, const DifField<T>& i, unsigned int& retid) {
	DifField<T> * handle = addChannelIntern(name, i, retid);

	if(!handle) {
		_THROW("addChannel() : addChannelIntern() returned an invalid pointer.");
		return false;
	}

	handle->metadata().setIntMetadata(m_scChannelIndexName, m_ulChannelIndex);
	handle->setSize(V3i(m_vSize.x, m_vSize.y, depthLevels()));

	return true;
}

template<typename T> DifField<T>* DifImage<T>::addChannelIntern(const std::string& name, const DifField<T>& i, unsigned int& retid) {
	if(i.getSize() != m_vSize) {
		_THROW("addChannelIntern() : size mismatch with provided channel.");
		return false;
	}

	if(m_lChannels.find(name) != m_lChannels.end()) {
		_THROW("addChannelIntern() : channel of the same name exists.");
		return false;
	}

	DifField<T> * handle = new DifField<T>(i);

	m_lChannels[name] = handle;

	retid = m_ulChannelIndex;

	++m_ulChannelIndex;

	return handle;
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
		_THROW("addChannel() : channel of the same name exists.");
		return false;
	}

	DifField<T> * handle = new DifField<T>(V2i(m_vSize.x, m_vSize.y));

	handle->metadata().setIntMetadata(m_scChannelIndexName, m_ulChannelIndex);
	handle->setSize(V3i(m_vSize.x, m_vSize.y, depthLevels()));

	m_lChannels[name] = handle;

	retid = m_ulChannelIndex;

	++m_ulChannelIndex;

	return (numberOfChannels() - 1);
}

/*!
 * @brief Returns the name of the channel at index @a idx
 * @param[in]  The Index (0..numberOfChannels()-1)
 * @return A String (empty if @a idx is out of range)
 */
template<typename T> const std::string& DifImage<T>::channelName(unsigned int idx) const {
	if(m_lChannels.size() == 0 || !validChannelId(idx)) {
		return std::string();
	}
	
	ChannelListIter it = m_lChannels.begin();

	std::advance(it, idx);

	return (*it).first;
}

template<typename T> unsigned int DifImage<T>::channelIndex(const std::string& name, bool *retval) {
	if(numberOfChannels() == 0) {
		return false;
	}

	ChannelListIter it;

	for(it = m_lChannels.begin(); it != m_lChannels.end(); it++) {
		if((*it).first == name) {
			if(retval) {
				(*retval) = true;
			}

			return ((*it).second)->metadata().intMetadata(m_scChannelIndexName, 0);
		}
	}

	if(retval) {
		(*retval) = false;
	}

	return 0;
}

/*!
 * @brief Determines whether the Image as a achannel named @a name
 * @param[in] name Channel's Name
 * @return boolean
 */
template<typename T> bool DifImage<T>::hasChannel(const std::string& name) {
	return validChannelId(channelIndex(name));
}

/// Returns the number of channels.
template<typename T> unsigned int DifImage<T>::numberOfChannels() const {
	return m_lChannels.size();
}

/*!
 * @brief Determines whether the given channel index @a id is valid
 * @param[in] id Channel ID
 * @return boolean
 */
template<typename T> bool DifImage<T>::validChannelId(unsigned int id) const {
	return (id < m_lChannels.size());
}

template<typename T> DifField<T>* DifImage<T>::getField(unsigned int channelid) {
	if(!validChannelId(channelid)) {
		return NULL;
	}

	ChannelListIter it = m_lChannels.begin();

	for(; it != m_lChannels.end(); it++) {
		if(((*it).second)->metadata().intMetadata(m_scChannelIndexName, -1) == channelid) {
			return (*it).second;
		}
	}

	return NULL;
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
 * @param[in] depth The depth level.
 * @param[in] data Data to write (must be at least sizeof(T)* numberOfChannels())
 */
template<typename T> void DifImage<T>::writeData(const V2i& pos, float depth, T* data) {
	unsigned int idx = 0;
	bool status = false;

	idx = indexAtDepth(depth, &status);

	if(!status) {
		m_lDepthMapping.push_back(depth);
		idx = (m_lDepthMapping.size()-1);
	}

	unsigned int current = 0;

	for(; current < numberOfChannels(); current++) {
		DifField<T>* field = getField(current);

		if(field) {
			field->writePixel(pos, idx, data[current]);
		} else {
			_THROW("writeData() : channel invalid");
		}
	}
}

/*!
 * @brief Reads data from the image
 * @param[in] pos    Position
 * @param[in] depth  Desired depth
 * @param[in] buffer Buffer to write data to
 * @param[in] type   Interpolation type
 * @return boolean
 *
 * If no data is available at the given @a depth it will be interpolated by the nearest two 
 * depths available if @a type is eLinear or will return false otherwise.
 */
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

		for(; i < numberOfChannels(); i++) {
			DifField<T>* field = getField(i);

			if(field) {
				buffer[i] = field->readPixel(pos, idx);
			}
		}
	}
	else if(type == eLinear) {
		unsigned int bfr, aftr;

		getNearestDepthIndex(depth, eBefore, bfr);

		if(depthAtIndex(bfr) >= depth || (bfr == aftr) || (bfr + 1) >=  depthLevels()) {
			return readData(pos, depth, buffer, eNone);
		}

		aftr = (bfr + 1);

		T *a = new T[numberOfChannels()];
		T *b = new T[numberOfChannels()];

		for(; i < numberOfChannels(); i++) {
			DifField<T>* field = getField(i);

			if(field) {
				a[i] = field->readPixel(pos, bfr);
				b[i] = field->readPixel(pos, aftr);
			}
		}

		{
			float d_bfr  = depthAtIndex(bfr);
			float d_aftr = depthAtIndex(aftr);

			float dt = d_aftr - d_bfr;
			float t  = (depth - d_bfr) / dt;

			for(i = 0; i < numberOfChannels(); i++) {
				buffer[i] = Imath::lerp(a[i], b[i], t);
			}
		}

		delete[] a;
		delete[] b;
	}

	return true;
}

/*!
 * @brief Reads the data at the given position and depth of a single channel
 * @param[in] channelid Channel Index
 * @param[in] depth     Desired depth
 * @param[out] retval   Variable to write the data in
 * @param[in] type      Interpolation type
 * @return boolean
 */
template<typename T> bool DifImage<T>::readChannelData(unsigned int channelid, const V2i& pos, float depth, T& retval, enum DifImage<T>::DifImageInterpolation type) {
	DifField<T> *field = getField(channelid);

	if(!field) {
		return false;
	}

	if(type == eNone) {
		bool status = false;
		unsigned int depthid = 0;

		depthid = indexAtDepth(depth, &status);

		if(!status) {
			return false;	
		}
			
		T value = field->readPixel(pos, depthid, &status);

		if(!status) {
			return false;
		}

		retval = value;

		return true;
	}
	else if(type == eLinear) {
		unsigned int bfr, aftr;

		getNearestDepthIndex(depth, eBefore, bfr);

		if(depthAtIndex(bfr) >= depth || (bfr == aftr) || (bfr + 1) >=  depthLevels()) {
			return readChannelData(channelid, pos, depth, retval, eNone);
		}

		{
			aftr = (bfr + 1);

			float d_bfr  = depthAtIndex(bfr);
			float d_aftr = depthAtIndex(aftr);

			float dt = d_aftr - d_bfr;
			float t  = (depth - d_bfr) / dt;
			
			bool stata = false;
			bool statb = false;

			T a = field->readPixel(pos, bfr,  &stata);
			T b = field->readPixel(pos, aftr, &statb);

			if(!stata || !statb) {
				return false;
			}

			retval = Imath::lerp(a, b, t);

			return true;
		}
	}

	return false;
}

template<typename T> bool DifImage<T>::readChannelData(const std::string& channelname, const V2i& pos, float depth, T& retval, enum DifImageInterpolation type) {
	bool status = false;
	unsigned int channelid = channelIndex(channelname, &status);

	if(!status) {
		return false;
	}

	return readChannelData(channelid, pos, depth, retval, type);
}

/*!
 * Saves the Deep image to the given output file.
 */
template<typename T> void DifImage<T>::save(Field3DOutputFile& ofp) {

	ChannelListIter it = m_lChannels.begin();

	{
		SparseField<float>::Ptr dptmapping = new SparseField<float>();
		dptmapping->setSize(V3i(1, 1, m_lDepthMapping.size()));

		DepthMappingListIter dit;
		unsigned int i = 0;

		for(dit = m_lDepthMapping.begin(); dit != m_lDepthMapping.end(); dit++) {
			dptmapping->lvalue(0, 0, i) = (*dit);
			++i;
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

/// Loads the Dif Image from an Input file.
template<typename T> bool DifImage<T>::load(Field3DInputFile& ifp) {
	typedef typename Field< T >::Vec           FieldVector;
	typedef typename Field< T >::Vec::iterator FieldVectorIterator;
	typedef typename SparseField< T >::Ptr     SparseFieldPtr;
	typedef typename std::map<std::string, SparseFieldPtr> SparseFieldList;
	typedef typename std::map<std::string, SparseFieldPtr>::iterator SparseFieldListIterator;

	// giving a layerName does not work for some reason so we look manually for our structure
	Field<float>::Vec dptMappings = ifp.readScalarLayers<float>();
	bool depthLoaded = false;

	if(dptMappings.size() < 1) {
		_THROW("load() : no depth mapping field available");
		return false;
	}

	{
		Field<float>::Vec::iterator it;

		for(it = dptMappings.begin(); it != dptMappings.end();  it++) {
			if((*it)->name == m_scDepthMappingName) {
				SparseField<float>::Ptr depthField = field_dynamic_cast< SparseField<float> >(dptMappings[0]);

				if(!depthField) {
					_THROW("load() : depth field is not a SparseField of type float");
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
		_THROW("load() : couldn't load depth mapping");
		return false;
	}
	

	FieldVector fields = ifp.readScalarLayers<T>();

	if(fields.size() < 1) {
		_THROW("load() : no channels available");
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

			unsigned int retid;
			addChannelIntern((*it)->name, DifField<T>(*handle), retid);
		}
	}

	return (m_lChannels.size() > 0) ? true : false;
}

/*!
 * @brief Computes the nearest depth
 *
 * @param[in] dpt    The Depth
 * @param[in] type   Type
 * @param[out] retid The nearest depth Index
 */
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

template<typename T> void DifImage<T>::addDepth(float dpt, bool sync) {
	unsigned int idx = 0;
	bool status = false;

	idx = indexAtDepth(dpt, &status);

	if(!status) {
		m_lDepthMapping.push_back(dpt);
		idx = (m_lDepthMapping.size()-1);
	}

	if(sync) {
		unsigned int current = 0;

		for(; current < numberOfChannels(); current++) {
			DifField<T>* field = getField(current);

			if(field) {
				field->updateDepth(idx);
			} else {
				_THROW("addDepth() : channel invalid");
			}
		}
	}
}


#undef _THROW
#undef _DIF_TYPE
FIELD3D_NAMESPACE_HEADER_CLOSE 

#endif // DIF_H
