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


DifImageInternal::DifImageInternal(const char *filename, bool create) : m_hFile(-1) {
	if(create == true) {
		m_hFile = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	} else {
		m_hFile = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
	}
}

DifImageInternal::~DifImageInternal() {
	H5Fclose(m_hFile);
}

bool DifImageInternal::valid() const {
	return (m_hFile > 0) ? true : false;
}

void DifImageInternal::sync() {
	writeMetadata();

	H5Fflush(m_hFile, H5F_SCOPE_GLOBAL);
}

void DifImageInternal::writeMetadata(hid_t grp, const std::string& key, const std::string& value) {
	hid_t attr = -1;

	if(H5Aexists(grp, key.c_str())) {
		attr = H5Aopen(grp, key.c_str(), H5P_DEFAULT);
	} else {

	}

	H5Aclose(attr);
}

void DifImageInternal::writeMetadata() {
	if(m_mAttributes.size() == 0) return;

	hid_t grp = -1;

	if(H5Lexists(m_hFile, ATTRIBUTE_NS, H5P_DEFAULT)) {
		grp = H5Gcreate(m_hFile, ATTRIBUTE_NS, H5P_DEFAULT);
	} else {
		grp = H5Gopen(m_hFile, ATTRIBUTE_NS);
	}

	assert(grp > -1);	

	std::map<std::string, std::string>::iterator it;

	for(it = m_mAttributes.begin(); it != m_mAttributes.end(); it++) {
		writeMetadata(grp, (it->first), (it->second));
	}

	H5Gclose(grp);
}
