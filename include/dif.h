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

namespace Internal {
class DifImageInternal; // So we don't waste the interface
}

/*!
 * @brief Deep Image File Format interface class.
 * 
 * @note The storage format, X and Y resolution cannot be altered once set. 
 */
class DifImage {
	public:
		/// Storage formats
		typedef enum {
			fInvalid = -1, ///< Invalid
			f8Bit  = 0,    ///< 8 Bit
			f16Bit = 1,    ///< 16 Bit
			f32Bit = 2,    ///< 32 Bit
			f64Bit = 3,    ///< 64 Bit
			fSReal = 4,    ///< Single precision real
			fDReal = 5     ///< Double precision real
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
		 * @param[in] compression Compression level (valid range is 0..9)
		 * @return Pointer to a DifImage instance or 0
		 */
		static DifImage* open(const char *path, unsigned int xres, 
							  unsigned int yres,
							  unsigned char compression = 0);

		/*! 
		 * @brief Returns the size in bytes of the given storage format
		 * @param[in] format Storage format
		 * @return Size of the format (fInvalid will result in 0)
		 */	
		static unsigned long formatToSize(DifDataFormat format);

		/*! 
		 * @brief Returns the Data format associated to the given number
		 * @param[in] num Storage format number
		 * @return The Dataformat or fInvalid
		 */
		static DifDataFormat numberToFormat(unsigned char num);

		/*! 
		 * @brief Returns the Data size of the storage format associated to 
		 * the given number
		 * @param[in] num Storage format number
		 * @return Size of the Storage type(or 0)
		 */
		static unsigned long numberToSize(unsigned char num);

	public:

		//TODO {
		/// Returns the amount of channels.
		unsigned int channels() const;
	
		/*!
		 * @brief Retrieves the size (in bytes) of the numberic format representing the given channel
		 * @param[in] idx Channel Index (valid range is 0..channels()-1)
		 */
		unsigned int channelSize(unsigned int idx) const;

		/*!
		 * @brief Name of the channel at index @a idx 
		 * @param[in] idx Channel Index
		 * @return Name of the channel or null pointer on error
		 */
		const char * channelName(unsigned int idx) const;

		/*!
		 * @brief Add a channel to the file
		 * @warning Channels cannot be deleted yet so be sure you'll definitly want to add one.
		 * @note The Filesize will increase drastically with each channel
		 * @param[in]  name   Channels name
		 * @param[in]  format Channels data format
		 * @param[out] idx Channels Index
		 * @return True on success, false on error
		 */
		bool addChannel(const char *name, DifDataFormat format, unsigned int& idx);


		/*!
		 * @brief Retrieves Raw Channel Data
		 * @param[in] channel Name of the channel
		 * @param[in] x X Position
		 * @param[in] y Y Position
		 * @param[out] outdata Data values; buffer must be the size of 
		 *                     the channel's storage format multiplied by the 
		 *                     samples of the channel on the given position
		 * @param[out] outdepth Depth values; buffer must be at least the size of 
		 *                      depth's storage format multiplied by the amount
		 *                      of samples on the given position
		 * @return Samples readed.
		 */
		unsigned int readData(const char *channel, unsigned int x, 
								 unsigned int y, void* outdata, void *outdepth);

		/*!
		 * @brief Writes raw channel data
		 * @param[in] channel Name of the channel
		 * @param[in] x X Position
		 * @param[in] y Y Position
		 * @param[in] nsamples Number of samples present in the following buffers
		 * @param[in] values Pointer to an array containing @a nsamples values.
		 * @param[in] depth Pointer to an array containing @a nsamples depth values.
		 * @param[in] format (Optional) represantation of the data within the given buffers.
		 *                   (will otherwise take channels data type)
		 * @return true on success false on error
		 */
		bool writeData(const char *channel, unsigned int x, unsigned int y, 
								 unsigned int nsamples, void *values, void *depth,
								 DifDataFormat format = fInvalid);

		//TODO }

		/// Retrieves the compression level
		unsigned char compression() const;
		
		/*!
		 * @brief Sets the compression level to @a compression
		 * @note The compression will append on newly created channels from then 
		 *       on. (Will be saved in file)
		 * @param[in] compression Compression level (valid range is 0..9)
		 * @return true on success false if @a compression is out of range
		 */
		bool setCompression(unsigned char compression);

		/*!
		 * @brief Query image resolution
		 * @param[out] x X Resolution
		 * @param[out] y Y Resolution
		 */
		void resolution(unsigned int &x, unsigned int &y) const;
		
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
		const char* meta(const char* key, const char *dflt = "") const;

		/*!
		 * @brief Deletes a Metatdata attribute
		 * @param[in] key Attribute key
		 * @return True on succes or false if the attribute does not exists.
		 */
		bool deleteMeta(const char* key);

		/*!
		 * @brief Adds or updates a Metadata attribute
		 *
		 * Setting an attribute's value to an empty string will also result in the deletion of the attribute
		 * @param[in] key Attribute key
		 * @param[in] value Value
		 * @note This function does not actually write the attributes to disk till you 
		 *       call DifImage::sync()
		 */
		void writeMeta(const char* key, const char *value);


	protected:
		DifImage();
		~DifImage();

	private:
		Internal::DifImageInternal *m_pInternal;
};

#endif // DIF_H
