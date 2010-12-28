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

using namespace Internal;

DifImage::DifImage() : m_pInternal(NULL) {

}

DifImage::~DifImage() {

}

DifImage* DifImage::open(const char *path) {
	DifImage *img = new DifImage;
	
	img->m_pInternal = new DifImageInternal(path, false);

	return (img->m_pInternal->valid()) ? img : NULL;
}

DifImage* DifImage::open(const char *path, unsigned int xres, unsigned int yres, unsigned char compression) {
	if(xres < 1 || yres < 1 || compression > 9) return NULL;

	DifImage *img = new DifImage;	

	img->m_pInternal = new DifImageInternal(path, true, xres, yres, compression);

	img->setCompression(compression);

	return (img->m_pInternal->valid()) ? img : NULL;
}

unsigned long DifImage::formatToSize(DifDataFormat format) {
	switch(format) {
		case f8Bit:
			return 1;
		case f16Bit:
			return 2;
		case f32Bit:
			return 4;
		case f64Bit:
			return 8;

		case fSReal:
			return 4;
		case fDReal:
			return 8;
	}

	return 0;
}

DifImage::DifDataFormat DifImage::numberToFormat(unsigned char num) {
	switch(num) {
		case 0:
			return f8Bit;
		case 1:
			return f16Bit;
		case 2:
			return f32Bit;
		case 3:
			return f64Bit;

		case 4:
			return fSReal;
		case 5:
			return fDReal;
	}

	return fInvalid;
}

unsigned long DifImage::numberToSize(unsigned char num) {
	return formatToSize(numberToFormat(num));
}

const char *DifImage::formatToString(DifDataFormat format) {
	switch(format) {
		case fInvalid:
			return "fInvalid";
		case f8Bit:
			return "f8Bit";
		case f16Bit:
			return "f16Bit";
		case f32Bit:
			return "f32Bit";
		case f64Bit:
			return "f64Bit";
		case fSReal:
			return "fSReal";
		case fDReal:
			return "fDReal";
	}
}

void DifImage::release() {
	sync();

	delete m_pInternal;
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

void DifImage::resolution(unsigned int &x, unsigned int &y) const {
	x = m_pInternal->m_iX;
	y = m_pInternal->m_iY;
}

unsigned char DifImage::compression() const {
	return m_pInternal->m_iCompression;
}

bool DifImage::setCompression(unsigned char compression) {
	if(compression > 9) return false;

	m_pInternal->m_iCompression = compression;
	m_pInternal->updateHeader();

	return true;
}

unsigned int DifImage::channels() const {
	return m_pInternal->m_lChannels.size();
}

unsigned int DifImage::channelSize(unsigned int idx) const {
	if(idx > channels()) return 0;

	return m_pInternal->m_lChannels[idx]->size();	
}

const char * DifImage::channelName(unsigned int idx) const {
	if(idx > channels()) return "";

	return m_pInternal->m_lChannels[idx]->name().c_str();
}

bool DifImage::addChannel(const char *name, DifDataFormat format, unsigned int& idx) {
	return m_pInternal->addChannel(name, format, idx);
}

DifImage::DifDataFormat DifImage::channelFormat(unsigned int idx) const {
	if(idx > channels()) return fInvalid;

	return m_pInternal->m_lChannels[idx]->format();
}

bool DifImage::channelAtDepth(unsigned int idx, double dpt) const {
	if(idx > channels()) return false;

	return m_pInternal->m_lChannels[idx]->layerExists(dpt);
}

bool DifImage::dataRead(unsigned int idx, double depth, void *buffer) {
	if(idx > channels()) return false;

	return m_pInternal->m_lChannels[idx]->readLayer(depth, buffer);
}

bool DifImage::dataWrite(unsigned int idx, double depth, void *buffer) {
	if(idx > channels()) return false;

	return m_pInternal->m_lChannels[idx]->writeLayer(*m_pInternal, depth, buffer);
}
