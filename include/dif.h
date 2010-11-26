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

#ifndef DIF_H
#define DIF_H

class DifImageInternal; // So we don't waste the interface

/*!
 * Deep Image File Format interface class
 * @note The storage format, X and Y resolution cannot be altered once set. 
 */
class DifImage {
	public:
		/// Storage formats
		typedef enum {
			f8Bit,   ///< 8 Bit
			f16Bit,  ///< 16 Bit
			f32Bit,  ///< 32 Bit
			f64Bit,  ///< 64 Bit
			fSReal,  ///< Single precision real
			fDReal   ///< Double precision real
		} DifDataFormat;

		/*! 
		 * @brief Open an existing Dif File
		 * @param[in] path a valid file path
		 * @return Pointer to a DifImage instance or 0
		 */
		static DifImage* open(const char *path);

		/*! 
		 * @brief Creates a new  Dif File
		 * @param[in] path A valid file path
		 * @param[in] xres X Resolution of the image
		 * @param[in] yres Y Resolution of the image
		 * @param[in] format Storage format
		 * @return Pointer to a DifImage instance or 0
		 */
		static DifImage* open(const char *path, unsigned int xres, unsigned int yres, DifDataFormat format);

		/*! 
		 * @brief Returns the size in bytes of the given storage format
		 * @param[in] format Storage format
		 * @return Size of the format
		 */	
		static unsigned long formatToSize(DifDataFormat format);

	public:
		/// Returns the amount of channels.
		unsigned int channels() const;
		
		/// Synchronizes any changes to disk
		void sync();

		/// Closes the DifImage
		void release();

		/*!
		 * @brief Retrieves a Metadata attribute
		 * @param[in] key Attribute key
		 * @param[in] dflt Return value if attribute does not exists.
		 * @return the attributes content.
		 */
		const char* attribute(const char* key, const char *dflt = "") const;

		/*!
		 * @brief Deletes a Metatdata attribute
		 * @param[in] key Attribute key
		 * @return True on succes or false if the attribute does not exists.
		 */
		bool deleteAttribute(const char* key);

		/*!
		 * @brief Adds or updates a Metadata attribute
		 * @param[in] key Attribute key
		 * @param[in] value Value
		 */
		void writeAttribute(const char* key, const char *value);

	protected:
		DifImage();
		~DifImage();

	private:
		DifImageInternal *m_pInternal;
};

#endif // DIF_H
