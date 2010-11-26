/*
Copyright (C) 2010, Jan Adelsbach and other authors and contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the software's owners nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
#include <dif_image.h>

#include <stdint.h>

typedef struct DifFileHeader {
	uint32_t magic;
	uint32_t x;
	uint32_t y;
	uint32_t nchannel;
	int32_t  alphaidx;
	uint32_t datasize;
} DifFileHeader;

static int32_t DifHeaderMagic =  'D' | ('I' << 8) | ('F' << 16) | ('F' << 24);

DifImage* DifImage::open(const char *path) {
	FILE *fp = fopen(path, "rb");

	if(!fp) return NULL;

	DifImage *handle = new DifImage;

	handle->m_pFile = fp;
	handle->m_pHeader = new DifFileHeader;

	fread(handle->m_pHeader, sizeof(DifFileHeader), 1, fp);

	if(handle->m_pHeader->magic != DifHeaderMagic) return NULL;

	return handle;
}

DifImage* DifImage::open(const char *path, unsigned int xres, unsigned int yres, unsigned int nchannels, unsigned int alphaindex, DifImage::DifDataFormat format) {
	FILE *fp = fopen(path, "w+b");

	if(!fp) return NULL;

	DifImage *handle = new DifImage;

	handle->m_pFile = fp;
	handle->m_pHeader = new DifFileHeader;

	handle->m_pHeader->magic = DifHeaderMagic;

	handle->m_pHeader->x = xres;
	handle->m_pHeader->y = yres;

	handle->m_pHeader->nchannel = nchannels;
	handle->m_pHeader->alphaidx = alphaindex;
	handle->m_pHeader->datasize = formatToSize(format);

	handle->updateHeader();

	return handle;	
}

unsigned long DifImage::formatToSize(DifImage::DifDataFormat format) {
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
			return 4;
	}
}

DifImage::DifImage() : m_pHeader(NULL), m_pFile(NULL) {

}

DifImage::~DifImage() {
	if(m_pFile) fclose(m_pFile);
}

void DifImage::updateHeader() {
	long int orgpos = ftell(m_pFile);

	fseek(m_pFile, 0, SEEK_SET);
	
	fwrite(m_pHeader, sizeof(DifFileHeader), 1, m_pFile);

	fseek(m_pFile, orgpos, SEEK_SET);
}

void DifImage::printHeader() {
	printf("Magic=%d\n", m_pHeader->magic);
	printf("XRes=%d\n", m_pHeader->x);
	printf("YRes=%d\n", m_pHeader->y);
}
