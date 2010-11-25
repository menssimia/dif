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
#ifndef DIFFILE_H
#define DIFFILE_H

#include <dif_pixel.h>

#include <stdint.h>
#include <stdio.h>

/**
 * The Storage format
 */
enum DifStorageFormat {
	Dif8Bit  = 0, ///< 8 Bit Integer
	Dif16Bit = 1, ///< 16 Bit Integer
	Dif32Bit = 2, ///< 32 Bit Integer
	DifSReal = 3, ///< Single precision real
	DifDReal = 4  ///< Double precision real
};

class DifFile {
	public:
		virtual unsigned int channels() const = 0;
		virtual bool         alphaPresent() const = 0;
		virtual unsigned int alphaChannel() const = 0;

		virtual unsigned int XResolution() const = 0;
		virtual unsigned int YResolution() const = 0;

		virtual DifDeepPixel* getPixel(unsigned int x, unsigned int y) = 0;
		virtual bool          savePixel(unsigned int x, unsigned int y, DifDeepPixel& texel) = 0;

	protected:
		DifFile() {}
		virtual ~DifFile() {}
};

#endif // DIFFILE_H
