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
#ifndef DIFIMAGE_H
#define DIFIMAGE_H

#include <dif_pixel.h>

#include <deque>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct DifFileHeader;

class DifImage {
	public:
		typedef enum {
			f8Bit,   ///< 8 Bit
			f16Bit,  ///< 16 Bit
			f32Bit,  ///< 32 Bit
			f64Bit,  ///< 64 Bit
			fSReal,  ///< Single precision real
			fDReal   ///< Double precision real
		} DifDataFormat;

		unsigned int channels() const;
		bool         alphaPresent() const;
		unsigned int alphaChannel() const;

		unsigned int XResolution() const;
		unsigned int YResolution() const;

		DifDeepPixel* readPixel(unsigned int x, unsigned int y);
		bool          saveSave(unsigned int x, unsigned int y, DifDeepPixel& texel);

		DifDeepPixel* at(unsigned int x, unsigned int y);

		void sync();
	
	public:
		static DifImage* open(const char *path);
		static DifImage* open(const char *path, unsigned int xres, unsigned int yres, 
							  unsigned int nchannels, unsigned int alphaindex, DifDataFormat format);
		static unsigned long formatToSize(DifDataFormat format);

	protected:
		DifImage();
		~DifImage();

		void updateHeader();
		void printHeader();

	private:
		FILE *m_pFile;
		DifFileHeader *m_pHeader;

		std::deque<DifDeepPixel*> m_qPixels;
};

#endif // DIFIMAGE_H
