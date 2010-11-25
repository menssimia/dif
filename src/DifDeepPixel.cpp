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

#include <stdint.h>
#include <algorithm>


DifDeepPixel::DifDeepPixel(unsigned int x, unsigned int y, unsigned int nchannels, int alpha) : m_iChannels(nchannels), m_iAlpha(alpha), m_bDirty(false) {

}

DifDeepPixel::~DifDeepPixel() {
	size_t sz = m_qValues.size();

	for(unsigned int j = 0; j < sz; j++) {
		delete m_qValues[j];
	}

	m_qValues.clear();
}

bool DifDeepPixel::sortComparator(DifPixel* a, DifPixel* b) {
	return (a->depth() < b->depth()) ? true : false;
}


DifPixel* DifDeepPixel::getAtDepth(double dpt) {
	size_t sz = m_qValues.size();

	for(unsigned int j = 0; j < sz; j++) {
		if(m_qValues[j]->depth() == dpt) return m_qValues[j];
	}

	return NULL;
}

/**
 * Resort the depth samples. (has to be executed if you change the depth of some pixel elements.)
 */
void DifDeepPixel::sort() {
	if(m_qValues.size() == 0) return;

	std::sort(m_qValues.begin(), m_qValues.end(), sortComparator);

	m_bDirty = false;
}

/**
 * Creates a new deep pixel object
 * @param[in] nchannels Number of channels
 * @param[in] alpha     Index of the alpha channel or -1 if not present.
 * @return A new deep pixel object or NULL on error.
 */
DifDeepPixel* DifDeepPixel::create(unsigned int x, unsigned int y, unsigned int nchannels, int alpha) {
	if(nchannels < 1) return NULL;

	return new DifDeepPixel(x, y, nchannels, alpha);
}


/// Returns the greatest depth sample of this deep pixel.
double DifDeepPixel::depth() {
	if(m_qValues.size() < 1) return 0.0;

	if(m_bDirty == true) {
		sort();
	}

	return m_qValues[m_qValues.size()-1]->depth();
}

bool DifDeepPixel::canHandle(const DifPixel& px) const {
	return (px.channels() == m_iChannels && px.alphaChannel() == m_iAlpha) ? true : false;
}

unsigned int DifDeepPixel::samples() const {
	return m_qValues.size();
}

/**
 * @name Pixel Querying
 */
//@{

/**
 * Array accessor
 * @warning There no index range checks. (valid range is 0..depth-1)
 * @return Pointer to a DifPixel object
 */
DifPixel* DifDeepPixel::operator[](unsigned int idx) {
	if(m_bDirty == true) {
		sort();
	}

	return m_qValues[idx];
}

/**
 * Adds or updates a pixel depth sample
 * @param[in] tw a pixel
 * @return true or false on error (either channel amount mismatch or alpha index mismatch)
 */
bool DifDeepPixel::pixel(DifPixel& tw) {
	if(canHandle(tw) == false) return false;

	DifPixel *handle = getAtDepth(tw.depth());

	if(handle != NULL) {
		return handle->set(tw);
	} else {
		handle = new DifPixel(tw);
		m_qValues.push_back(handle);
		m_bDirty = true;
	}
}

DifPixel& DifDeepPixel::evaluate(float depth, DifDepthEvaluator eval) {
	if(m_bDirty == true) {
		sort();
	}

	return eval(*this, depth);
}

//@}
