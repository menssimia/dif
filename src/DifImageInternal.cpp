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

#define DIF_INTERN
#include <dif_internal.h>
#undef DIF_INTERN
#include <dif.h>

using namespace Internal;

DifImageInternal::DifImageInternal(const char *filename, bool create, int xres, 
								   int yres, int format) : m_hFile(-1) {

	if(create == true) {
		m_hFile = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

		writeIntegerAttribute(m_hFile, "XRes", xres);
		writeIntegerAttribute(m_hFile, "YRes", yres);

	} else {
		m_hFile = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
	}

	loadHeader();
	loadMetadata();
	loadChannelMeta();
}

DifImageInternal::~DifImageInternal() {
	H5Fclose(m_hFile);

	m_lChannels.release();
}

bool DifImageInternal::valid() const {
	return (m_hFile > 0 && m_iX > 0 && m_iY > 0) ? true : false;
}

void DifImageInternal::sync() {
	writeMetadata();

	H5Fflush(m_hFile, H5F_SCOPE_GLOBAL);
}

hid_t DifImageInternal::openCreateGroup(hid_t loc, const char *name) {
	hid_t grp = -1;

	if(linkExists(loc, name)) {
		grp = H5Gopen(loc, name);
	} else {
		grp = H5Gcreate(loc, name, H5P_DEFAULT);
	}

	return grp;
}

void DifImageInternal::deleteMetadata(hid_t grp, const std::string& key) {
	hid_t attr = -1;

	if(H5Aexists(grp, key.c_str())) {
		H5Adelete(grp, key.c_str());
	} 
}

void DifImageInternal::writeMetadata(hid_t grp, const std::string& key, const std::string& value) {
	hid_t attr = -1;
	hid_t sp = -1;
	hid_t att = -1;

	att = H5Tcopy(H5T_C_S1);
	H5Tset_size(att, value.size());

	if(H5Aexists(grp, key.c_str())) {
		attr = H5Aopen(grp, key.c_str(), H5P_DEFAULT);
	} else {
		sp  = H5Screate(H5S_SCALAR);

		attr = H5Acreate(grp, key.c_str(), att, sp, H5P_DEFAULT);
	}

	//printf("metawr %s -> %s\n", key.c_str(), value.c_str()); /// DEBUG

	H5Awrite(attr, att, value.c_str());

	H5Aclose(attr);
	H5Tclose(att);

	if(sp != -1) {
		H5Sclose(sp);
	}
}

void DifImageInternal::writeMetadata() {
	if(m_mAttributes.size() == 0) return;

	hid_t grp = openCreateGroup(m_hFile, ATTRIBUTE_NS);

	assert(grp > -1);	

	std::map<std::string, std::string>::iterator it;

	for(it = m_mAttributes.begin(); it != m_mAttributes.end(); it++) {
		if((it->second).length() == 0) {
			deleteMetadata(grp, (it->first));
		} else {
			writeMetadata(grp, (it->first), (it->second));
		}
	}

	H5Gclose(grp);
}

void DifImageInternal::loadMetadata() {
	if(!linkExists(m_hFile, ATTRIBUTE_NS)) return;

	hid_t grp = H5Gopen(m_hFile, ATTRIBUTE_NS);
	unsigned long nattr = 0;

#ifdef HDF5_1_80
	H5Oget_info info;
	H5Oget_info(grp, &info);
	
	nattr = info.num_attrs;
#else
	nattr = H5Aget_num_attrs(grp);
#endif //HDF5_1_80

	for(unsigned long j = 0; j < nattr; j++) {
#ifdef HDF5_1_80
		#error HDF Version 1.80.X is not yet supported.
		//TODO	
		/*
		hid_t attr = H5Aopen_by_idx(grp, ".", j);
	
		nattr = info.num_attrs;*/
#else
		hid_t attr = H5Aopen_idx(grp, j);

		char key[255];
		char val[255];

		hid_t att = H5Tcopy(H5T_C_S1);
		H5Tset_size(att, 255);

		H5Aget_name(attr, 255, key);
		H5Aread(attr, att, val);

//		printf("metald %s -> %s\n", key, val);

		m_mAttributes[std::string(key)] = std::string(val);

		H5Aclose(attr);
#endif //HDF5_1_80
	}

	H5Gclose(grp);
}

void DifImageInternal::loadHeader() {
	m_iX           = readIntegerAttribute(m_hFile, "XRes", 0);
	m_iY           = readIntegerAttribute(m_hFile, "YRes", 0);
	m_iCompression = readIntegerAttribute(m_hFile, "Compression", 0);
}

void DifImageInternal::updateHeader() {
	writeIntegerAttribute(m_hFile, "Compression", m_iCompression);
}

bool DifImageInternal::addChannel(const std::string& name, const DifImage::DifDataFormat t, unsigned int& idx) {
	if(m_lChannels.exists(name) == true) return false;

	DifChannel *ch = DifChannel::create(m_hFile, name, t);

	if(ch == NULL) return false;

	m_lChannels.push_back(ch);

	idx = (m_lChannels.size() - 1);

	return true;
}

void DifImageInternal::loadChannelMeta() {

	H5G_info_t info;
	H5Gget_info(m_hFile, &info);

	for(unsigned int j = 0; j < info.nlinks; j++) {

		// Based upon the h5ex_g_corder.c example
		size_t sz = H5Lget_name_by_idx(m_hFile, ".", H5_INDEX_NAME, H5_ITER_INC, j, NULL, 0, H5P_DEFAULT)+1;

		char *buff = new char[sz];

		H5Lget_name_by_idx(m_hFile, ".", H5_INDEX_NAME, H5_ITER_INC, j, buff, sz, H5P_DEFAULT);

		if(strcmp(buff, ATTRIBUTE_NS) != 0) {
			DifChannel *ch = new DifChannel(std::string(buff), m_hFile);

			m_lChannels.push_back(ch);
		}

		delete[] buff;
	}
}
