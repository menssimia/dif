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

#include <dif.h>

#define DIF_INTERN
#include <dif_internal.h>
#undef DIF_INTERN

DifImage::DifImage() : m_pInternal(NULL) {

}

DifImage::~DifImage() {
	if(m_pInternal) delete m_pInternal;
}

DifImage* DifImage::open(const char *path) {
	DifImage *handle = new DifImage();

	handle->m_pInternal = new DifImageInternal(path, false);

	if(handle->m_pInternal->valid() == false) return NULL;

	return handle;
}

DifImage* DifImage::open(const char *path, unsigned int xres, unsigned int yres, DifDataFormat format) {
	DifImage *handle = new DifImage();

	handle->m_pInternal = new DifImageInternal(path, true);

	if(handle->m_pInternal->valid() == false) return NULL;

	return handle;
}

void DifImage::release() {
	sync();

	delete this;
}

void DifImage::sync() {
	m_pInternal->sync();
}

const char* DifImage::meta(const char* key, const char *dflt) const {
	std::map<std::string, std::string>::iterator it;
		
	if((it = m_pInternal->m_mAttributes.find(std::string(key))) != m_pInternal->m_mAttributes.end()) {
		return (it->second).c_str();
	}

	return dflt;
}


bool DifImage::deleteMeta(const char* key) {
	std::map<std::string, std::string>::iterator it;
		
	if((it = m_pInternal->m_mAttributes.find(std::string(key))) != m_pInternal->m_mAttributes.end()) {
		(it->second) = ""; // This will delete the attribute
		return true;
	}

	return false;
}

void DifImage::writeMeta(const char* key, const char *value) {
	std::map<std::string, std::string>::iterator it;
		
	if((it = m_pInternal->m_mAttributes.find(std::string(key))) != m_pInternal->m_mAttributes.end()) {
		(it->second) = std::string(value);
	} else {
		m_pInternal->m_mAttributes[std::string(key)] = std::string(value);
	}
}
