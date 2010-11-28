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

using namespace Internal;

// TODO Error handling

DifChannel::DifChannel(const std::string& name, hid_t id) : m_eFormat(DifImage::fInvalid), m_sName(name), m_hId(-1), m_iNumberOfLayers(0) {
	if(id != -1) {
		m_hId = open(id);
		reloadMeta();
	}
}

DifChannel::~DifChannel() {
	if(m_hId != -1) {
		close(m_hId);
	}
}

unsigned long DifChannel::size() const {
	return DifImage::formatToSize(m_eFormat);
}

const std::string& DifChannel::name() const {
	return m_sName;
}

hid_t  DifChannel::open(hid_t parent, bool cin) {
	hid_t grp = -1;

	if(linkExists(parent, name().c_str())) {
		grp = H5Gopen(parent, name().c_str());
	} else if(cin == true) {
		grp = H5Gcreate(parent, name().c_str(), H5P_DEFAULT);
	}

	return grp;
}

herr_t DifChannel::close(hid_t ch) {
	return H5Gclose(ch);
}

/// Whether the channel is represented in layer @a lay
bool DifChannel::inLayer(const std::string& lay) {
	if(m_hId == -1) return false;

	if(linkExists(m_hId, lay.c_str())) {
		return true;
	}

	return false;
}

/// Reload Metadata associated with the channel
void DifChannel::reloadMeta() {
	if(m_hId == -1) return;

	m_eFormat = DifImage::numberToFormat(readIntegerAttribute(m_hId, "StorageFormat", -1));

	H5G_info_t info;
	H5Gget_info(m_hId, &info); //FIXME Are attributes counting as links?
	m_iNumberOfLayers = info.nlinks;
}

DifChannel* DifChannel::create(hid_t parent, const std::string& name, const DifImage::DifDataFormat t) {
	if(linkExists(parent, name.c_str())) {
		return NULL;
	}
		
	DifChannel *handle = new DifChannel(name, -1);

	handle->m_hId =  handle->open(parent, true);

	writeIntegerAttribute(handle->m_hId, "StorageFormat", t);

	handle->reloadMeta();

	return handle;
}

DifImage::DifDataFormat DifChannel::format() const {
	return m_eFormat;
}
