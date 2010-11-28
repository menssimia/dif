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

bool DifHD5Util::linkExists(hid_t loc, const std::string& name) {
	if(H5Lexists(loc, name.c_str(), H5P_DEFAULT)) {
		return true;
	} else {
		return false;
	}
}

void DifHD5Util::writeIntegerAttribute(hid_t grp, const std::string& attrname, int value) {
	hid_t attr = -1;
	hid_t sp = -1;


	if(H5Aexists(grp, attrname.c_str())) {
		attr = H5Aopen(grp, attrname.c_str(), H5P_DEFAULT);
	} else {
		sp  = H5Screate(H5S_SCALAR);

		attr = H5Acreate(grp, attrname.c_str(), H5T_NATIVE_INT, sp, H5P_DEFAULT);
	}

	H5Awrite(attr, H5T_NATIVE_INT, &value);

	H5Aclose(attr);

	if(sp != -1) {
		H5Sclose(sp);
	}
}

int  DifHD5Util::readIntegerAttribute(hid_t grp, const std::string& attrname, int defval) {
	hid_t attr = -1;

	if(H5Aexists(grp, attrname.c_str())) {
		attr = H5Aopen(grp, attrname.c_str(), H5P_DEFAULT);

		int ret;

		H5Aread(attr, H5T_NATIVE_INT, &ret);

		H5Aclose(attr);

		return ret;
	} else {
		return defval;
	}
}
