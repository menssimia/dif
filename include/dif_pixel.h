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

#ifndef DIF_PIXEL_H
#define DIF_PIXEL_H

#include <deque>
#include <stdio.h>

class DifPixel;
class DifDeepPixel;
class DifFile;

class DifPixel {
	public:
		DifPixel(double r, double g, double b, double a = 1.0, double z = 0.0);
		DifPixel(unsigned int nchannels = 1, int alpha = -1, double *indata = NULL, double z = 0.0);
		DifPixel(const DifPixel& src);
		~DifPixel();

		double& operator[](unsigned int idx);

		unsigned int channels() const;
		int          alphaChannel() const;
		bool         hasAlpha() const;

		double& depth();

		bool set(const DifPixel& src);

	private:
		double *m_pData;
		double m_dDepth;
		unsigned int m_iChannels;
		int m_iAlpha;
};

class DifDeepPixel {
	friend class DifFile;
	public:
		double depth() const;
		DifPixel* operator[](unsigned int idx);

		void sort();

		bool pixel(DifPixel& tw);
		DifPixel& evaluate(float depth);

		bool canHandle(const DifPixel& px) const;

	public:
		static DifDeepPixel* create(unsigned int x, unsigned int y, unsigned int nchannels, int alpha = -1);
	
	protected:
		static bool sortComparator(DifPixel* a, DifPixel* b);
		
		DifPixel* getAtDepth(double dpt);

	private:
		DifDeepPixel(unsigned int x, unsigned int y, unsigned int nchannels, int alpha);
		virtual ~DifDeepPixel();

	private:
		std::deque<DifPixel*> m_qValues;
		int                   m_iAlpha;
		unsigned int          m_iChannels;
		unsigned int          m_iX;
		unsigned int          m_iY;
		bool                  m_bDirty;
};

#endif // DIF_PIXEL_H
