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

#ifndef DIFINTERNAL_H
#define DIFINTERNAL_H

#ifndef DIF_INTERN
	#warning This (internal) file is always subject to change. Include at your own risk. I mean it.
#endif //DIF_INTERN

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <assert.h>
#include <stdbool.h>
#include <cstring>
#include <list>

#include <dif.h>

namespace Internal {

// HF5's C++ API sucks
#include <hdf5.h>
#include <hdf5_hl.h>

#ifdef HDF5_1_80
	#include <H5Opublic.h>
#endif // HDF5_1_80


#define ATTRIBUTE_NS   "dif_meta_attributes"
#define LAYER_NAMING_SCHEME "layer:%f"

class DifImageInternal;

// TODO Error managment

/*!
 * @brief Internal class do not use directly.
 */
class DifHD5Util {
	public:
		static bool linkExists(hid_t loc, const std::string& name);
		static void writeIntegerAttribute(hid_t grp, const std::string& attrname, int value);
		static int  readIntegerAttribute(hid_t grp, const std::string& attrname, int defval=0);

		static hid_t formatToHDF5Type(const DifImage::DifDataFormat t);

		static bool gzipAvailable();

		static bool write(const DifImage::DifDataFormat t, hid_t loc, 
						  const char *name, int rank, const hsize_t *dims, 
						  void *buffer, unsigned char compression = 0);

		static bool read(const DifImage::DifDataFormat t, hid_t loc, const char *name,
						 void *buffer);
};


/*!
 * @brief Internal class do not use directly.
 */
class DifChannel : public DifHD5Util {
	public:
		DifChannel(const std::string& name, hid_t id = -1);
		~DifChannel();

		unsigned long size() const;
		const std::string& name() const;

		hid_t  open(hid_t parent, bool cin = false);
		static herr_t close(hid_t ch);

		void reloadMeta();

		static DifChannel* create(hid_t parent, const std::string& name, const DifImage::DifDataFormat t);

		DifImage::DifDataFormat format() const;

		hid_t getID() const;

		bool layerExists(double depth) const;
		static std::string getLayerName(double dpt);


		bool readLayer(double depth, void* buffer);
		bool writeLayer(DifImageInternal& intr, double depth, void* buffer);

	private:
		DifImage::DifDataFormat m_eFormat;
		std::string m_sName;
		hid_t m_hId;
		unsigned int m_iNumberOfLayers;
};

/*!
 * @brief Internal class do not use directly.
 */
class DifChannelList : public std::list<DifChannel*> {
	public:
		DifChannelList() {}
		~DifChannelList() {
			release();
		}
		
		void release() {
			std::list<DifChannel*>::iterator it;

			for(it = begin(); it != end(); it++) {
				delete *it;
			}

			clear();
		}

		DifChannel* operator[](const std::string& in) {
			std::list<DifChannel*>::iterator it;

			for(it = begin(); it != end(); it++) {
				if(in == (*it)->name()) return *it;
			}

			return NULL;
		}

		DifChannel* operator[](unsigned int idx) {
			assert(idx < size());

			std::list<DifChannel*>::iterator it = begin();
			std::advance(it, idx);

			return *it;
		}

		bool exists(const std::string& in) {
			return ((*this)[in] != NULL) ? true : false;
		}
};

/*!
 * @brief Internal class do not use directly.
 */
class DifImageInternal : public DifHD5Util {
	public:
		DifImageInternal(const char *filename, bool create = false, int xres = 0, 
						 int yres = 0, int format = 1);
		~DifImageInternal();

		bool valid() const;

		void sync();

		void writeMetadata();
		void writeMetadata(hid_t grp, const std::string& key, const std::string& value);
		void deleteMetadata(hid_t grp, const std::string& key);
		
		void loadMetadata();
	
		hid_t openCreateGroup(hid_t loc, const char *name);

		void loadHeader();
		void updateHeader();

		bool addChannel(const std::string& name, const DifImage::DifDataFormat t, unsigned int& idx);

		void loadChannelMeta();
	
	public:
		hid_t m_hFile; // File instance
		std::map<std::string, std::string> m_mAttributes;
		DifChannelList m_lChannels; 

		unsigned int m_iX;
		unsigned int m_iY;
		unsigned char m_iCompression;
};

} // Internal
#endif //DIFINTERNAL
