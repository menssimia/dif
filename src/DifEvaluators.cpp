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
#include <dif_evaluators.h>
#include <dif_pixel.h>

#include <assert.h>

#define NEAR_LOOP(a,b,d, max)                                                  \
	for(unsigned j = 0; j < max; j++) {                                        \
		if(dp[j]->depth() <= d) a = j;                                         \
		if(dp[j]->depth() >= d) b = j;                                         \
                                                                               \
		if(a > -1 && b > -1) break;                                            \
	}

DifPixel& DifEvaluatorConstantMedian(DifDeepPixel& dp, double depth) {
	if(dp.samples() == 0) {
		return *(new DifPixel(0.0, 0.0, 0.0, 0.0, 0.0));
	} else if(dp.samples() == 1) {
		return *(new DifPixel(*(dp[0])));
	}

	if(dp.depth() < depth) {
		return *(new DifPixel(*(dp[dp.samples()-1])));
	} else if(dp[0]->depth() > depth) {
		return *(new DifPixel(*(dp[0])));
	}
	
	unsigned int samples = dp.samples();
	int idxa = -1;
	int idxb = -1;

	NEAR_LOOP(idxa, idxb, depth, samples);

	assert(idxa > -1);
	assert(idxb > -1);
	assert(idxb < samples);

	double median = ((dp[idxb]->depth() - dp[idxa]->depth()) / 2.0);

	if((dp[idxa]->depth() + median) >= depth) {
		return *(new DifPixel(*(dp[idxa])));
	} else {
		return *(new DifPixel(*(dp[idxb])));
	}
}



#undef NEAR_LOOP
