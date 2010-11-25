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

#include <dif_pixel.h>

/**
 * @class DifPixel
 *
 * Pixel object with an undetermined amount of channels.
 * @note The Z value is not treated as channel.
 */

/** Constructor for user's convenience
 * @param[in] r Red channel
 * @param[in] g Green channel
 * @param[in] b Blue channel
 * @param[in] a Alpha channel
 * @param[in] z Depth
 */
DifPixel::DifPixel(double r, double g, double b, double a, double z) : m_pData(NULL), m_iChannels(4), m_iAlpha(3), m_dDepth(z) {
	m_pData = new double[4];

	m_pData[0] = r;
	m_pData[1] = g;
	m_pData[2] = b;
	m_pData[3] = a;
}

/** Constructor
 * @param[in] nchannels The number of channels withhin this pixel
 * @param[in] alpha index of the alpha channel (-1 if no alpha channel exists)
 * @param[in] indata An array of doubles to copy values from
 * @param[in] z Depth
 */
DifPixel::DifPixel(unsigned int nchannels, int alpha, double *indata, double z) : m_pData(NULL), m_iChannels(nchannels), 
																				  m_iAlpha(alpha), m_dDepth(z) {
	m_pData = new double[nchannels];

	for(unsigned j=0; j < nchannels; j++) {
		m_pData[j] = indata[j];
	}
}

/// Copy constructor
DifPixel::DifPixel(const DifPixel& src) : m_pData(NULL), m_iChannels(src.m_iChannels), m_iAlpha(src.m_iAlpha), m_dDepth(src.m_dDepth) {
	m_pData = new double[src.m_iChannels];

	for(unsigned j=0; j < src.m_iChannels; j++) {
		m_pData[j] = src.m_pData[j];
	}	
}

/// Destructor
DifPixel::~DifPixel() {
	delete[] m_pData;
}

bool DifPixel::set(const DifPixel& src) {
	if(src.m_iChannels != m_iChannels || src.m_iAlpha != m_iAlpha) return false;

	for(unsigned j=0; j < src.m_iChannels; j++) {
		m_pData[j] = src.m_pData[j];
	}

	m_dDepth = src.m_dDepth;

	return true;
}

/**
 * Array accessor
 * @warning There no index range checks. (valid range is 0..nchannel-1)
 */
double& DifPixel::operator[](unsigned int idx) {
	return m_pData[idx];
}

/// Returns the number of channels
unsigned int DifPixel::channels() const {
	return m_iChannels;
}

/// Returns the index of the alpha channel or -1 if not present 
int DifPixel::alphaChannel() const {
	return (m_iAlpha >= 0) ? m_iAlpha : -1; // To be sure it will return -1 in case
}

/// Determines whether the alpha channel is present
bool DifPixel::hasAlpha() const {
	return (m_iAlpha >= 0) ? true : false;
}

/// Returns a reference to the depth(distance) of this pixel.
double& DifPixel::depth() {
	return m_dDepth;
}
