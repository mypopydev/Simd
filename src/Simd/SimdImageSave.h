/*
* Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2021 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#ifndef __SimdImageSave_h__
#define __SimdImageSave_h__

#include "Simd/SimdMemoryStream.h"
#include "Simd/SimdArray.h"

namespace Simd
{
    typedef uint8_t* (*ImageSaveToMemoryPtr)(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);

    SimdBool ImageSaveToFile(const ImageSaveToMemoryPtr saver, const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, const char* path);

    //---------------------------------------------------------------------

    struct ImageSaverParam
    {
        size_t width, height;
        SimdPixelFormatType format;
        SimdImageFileType file;
        int quality;

        SIMD_INLINE ImageSaverParam(size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality)
        {
            this->width = width;
            this->height = height;
            this->format = format;
            this->file = file;
            this->quality = quality;
        }

        bool Validate()
        {
            if (file == SimdImageFileUndefined)
            {
                if (format == SimdPixelFormatGray8)
                    file = SimdImageFilePgmBin;
                else
                    file = SimdImageFilePpmBin;
            }            
            if (!(format == SimdPixelFormatGray8 || format == SimdPixelFormatBgr24 || 
                format == SimdPixelFormatBgra32 || format == SimdPixelFormatRgb24))
                return false;
            return true;
        }
    };

    class ImageSaver
    {
    protected:
        ImageSaverParam _param;
        OutputMemoryStream _stream;
    public:
        ImageSaver(const ImageSaverParam& param)
            : _param(param)
        {
        }

        virtual ~ImageSaver()
        {
        }

        virtual bool ToStream(const uint8_t* src, size_t stride) = 0;

        SIMD_INLINE uint8_t* Release(size_t* size)
        {
            return _stream.Release(size);
        }
    };
       
    namespace Base
    {
        class ImagePxmSaver : public ImageSaver
        {
        public:
            ImagePxmSaver(const ImageSaverParam& param);

        protected:
            typedef void (*ConvertPtr)(const uint8_t* src, size_t width, size_t height, size_t srcStride, uint8_t* dst, size_t dstStride);
            ConvertPtr _convert;
            Array8u _buffer;
            size_t _block, _size;

            void WriteHeader(size_t version);
        };

        class ImagePgmTxtSaver : public ImagePxmSaver
        {
        public:
            ImagePgmTxtSaver(const ImageSaverParam& param);

            virtual bool ToStream(const uint8_t* src, size_t stride);
        };

        class ImagePgmBinSaver : public ImagePxmSaver
        {
        public:
            ImagePgmBinSaver(const ImageSaverParam& param);

            virtual bool ToStream(const uint8_t* src, size_t stride);
        };

        class ImagePpmTxtSaver : public ImagePxmSaver
        {
        public:
            ImagePpmTxtSaver(const ImageSaverParam& param);

            virtual bool ToStream(const uint8_t* src, size_t stride);
        };

        class ImagePpmBinSaver : public ImagePxmSaver
        {
        public:
            ImagePpmBinSaver(const ImageSaverParam& param);

            virtual bool ToStream(const uint8_t* src, size_t stride);
        };

        //---------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);
    }

#ifdef SIMD_SSE41_ENABLE    
    namespace Sse41
    {
        class ImagePgmTxtSaver : public Base::ImagePgmTxtSaver
        {
        public:
            ImagePgmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePgmBinSaver : public Base::ImagePgmBinSaver
        {
        public:
            ImagePgmBinSaver(const ImageSaverParam& param);
        };

        class ImagePpmTxtSaver : public Base::ImagePpmTxtSaver
        {
        public:
            ImagePpmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePpmBinSaver : public Base::ImagePpmBinSaver
        {
        public:
            ImagePpmBinSaver(const ImageSaverParam& param);
        };

        //---------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);
    }
#endif// SIMD_SSE41_ENABLE

#ifdef SIMD_AVX2_ENABLE    
    namespace Avx2
    {
        class ImagePgmTxtSaver : public Sse41::ImagePgmTxtSaver
        {
        public:
            ImagePgmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePgmBinSaver : public Sse41::ImagePgmBinSaver
        {
        public:
            ImagePgmBinSaver(const ImageSaverParam& param);
        };

        class ImagePpmTxtSaver : public Sse41::ImagePpmTxtSaver
        {
        public:
            ImagePpmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePpmBinSaver : public Sse41::ImagePpmBinSaver
        {
        public:
            ImagePpmBinSaver(const ImageSaverParam& param);
        };

        //---------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);
    }
#endif// SIMD_AVX2_ENABLE

#ifdef SIMD_AVX512BW_ENABLE    
    namespace Avx512bw
    {
        class ImagePgmTxtSaver : public Avx2::ImagePgmTxtSaver
        {
        public:
            ImagePgmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePgmBinSaver : public Avx2::ImagePgmBinSaver
        {
        public:
            ImagePgmBinSaver(const ImageSaverParam& param);
        };

        class ImagePpmTxtSaver : public Avx2::ImagePpmTxtSaver
        {
        public:
            ImagePpmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePpmBinSaver : public Avx2::ImagePpmBinSaver
        {
        public:
            ImagePpmBinSaver(const ImageSaverParam& param);
        };

        //---------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);
    }
#endif// SIMD_AVX512BW_ENABLE

#ifdef SIMD_NEON_ENABLE    
    namespace Neon
    {
        class ImagePgmTxtSaver : public Base::ImagePgmTxtSaver
        {
        public:
            ImagePgmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePgmBinSaver : public Base::ImagePgmBinSaver
        {
        public:
            ImagePgmBinSaver(const ImageSaverParam& param);
        };

        class ImagePpmTxtSaver : public Base::ImagePpmTxtSaver
        {
        public:
            ImagePpmTxtSaver(const ImageSaverParam& param);
        };

        class ImagePpmBinSaver : public Base::ImagePpmBinSaver
        {
        public:
            ImagePpmBinSaver(const ImageSaverParam& param);
        };

        //---------------------------------------------------------------------

        uint8_t* ImageSaveToMemory(const uint8_t* src, size_t stride, size_t width, size_t height, SimdPixelFormatType format, SimdImageFileType file, int quality, size_t* size);
    }
#endif// SIMD_NEON_ENABLE
}

#endif//__SimdImageSave_h__
