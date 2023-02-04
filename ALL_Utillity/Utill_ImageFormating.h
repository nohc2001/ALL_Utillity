#pragma once
#include <iostream>
#include <fstream>
#include "Utill_FreeMemory.h"
#include "Utill_Reform.h"

namespace imgform {
#define imgform_init freemem::FM_Model0 PixelImageObject::ImageOperateData_tempFM;

    constexpr uint32_t crc_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
    };

    typedef struct RGBA_pixel {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    class PixelImageObject {
    public:
        static constexpr int maxManageTempData = 3640 * 3640; // 3640 * 3640 * 4 byte -> 52MB
        static freemem::FM_Model0 ImageOperateData_tempFM;
        int width = 0;
        int height = 0;
        freemem::InfiniteArray<RGBA_pixel> ImageData;

        PixelImageObject() :
            width(0), height(0)
        {
            if (ImageOperateData_tempFM.isHeap == false) {
                ImageOperateData_tempFM.ClearAll();
                ImageOperateData_tempFM.SetHeapData(new byte8[maxManageTempData], maxManageTempData);
            }

            ImageData.NULLState();
            ImageData.SetFM((freemem::FM_Model*)&ImageOperateData_tempFM);
            //ImageData.SetVPTR();
            ImageData.Init(0);
        }

        PixelImageObject(int wid, int hei) :
            width(wid), height(hei)
        {
            if (ImageOperateData_tempFM.isHeap == false) {
                ImageOperateData_tempFM.ClearAll();
                ImageOperateData_tempFM.SetHeapData(new byte8[maxManageTempData], maxManageTempData);
            }

            ImageData.NULLState();
            ImageData.SetFM((freemem::FM_Model*)&ImageOperateData_tempFM);
            //ImageData.SetVPTR();
            ImageData.Init(wid * hei);
            ImageData.up = wid * hei;
        }

        virtual ~PixelImageObject() {
            ImageData.clear();
        }

        void SetData(int x, int y, RGBA_pixel pixel) {
            ImageData[y * width + x] = pixel;
        }

        RGBA_pixel GetData(int x, int y) {
            return ImageData[y * width + x];
        }

        void rawDataToPNG(const std::string& filename) {
            std::ofstream file(filename, std::ios::binary);
            if (!file) {
                std::cerr << "Error opening file for writing." << std::endl;
                return;
            }

            file.put(0x89); file.put('P'); file.put('N'); file.put('G');
            file.put(0x0D); file.put(0x0A); file.put(0x1A); file.put(0x0A);

            /*unsigned int chunkLength = 0;
            unsigned int chunkType = 0;
            file.write((char*)&chunkLength, 4);
            file.write((char*)&chunkType, 4);*/

            // IHDR chunk
            std::vector<unsigned char> ihdr;
            ihdr.resize(13);
            ihdr[0] = width >> 24;
            ihdr[1] = width >> 16;
            ihdr[2] = width >> 8;
            ihdr[3] = width;
            ihdr[4] = height >> 24;
            ihdr[5] = height >> 16;
            ihdr[6] = height >> 8;
            ihdr[7] = height;
            ihdr[8] = 8; // bit depth
            ihdr[9] = 6; // color type: RGB
            ihdr[10] = 0; // compression method: deflate
            ihdr[11] = 0; // filter method: adaptive
            ihdr[12] = 0; // interlace method: none
            file.put(0); file.put(0); file.put(0); file.put(13);
            file.put('I'); file.put('H'); file.put('D'); file.put('R');
            file.write(reinterpret_cast<const char*>(ihdr.data()), 13);
            unsigned int crc = 0xffffffff;
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'I'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'H'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'D'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'R'];
            for (int i = 0; i < 13; ++i) {
                crc = (crc << 8) ^ crc_table[(crc >> 24) ^ ihdr[i]];
            }
            crc ^= 0xffffffff;
            file.put(crc >> 24); file.put(crc >> 16);
            file.put(crc >> 8); file.put(crc);


            // Write IDAT chunk

            //여기서 부터 이어서 작성

            unsigned int IDAT_chunkSiz = 4 + height * (1 + width * 4);
            file.write((char*)&IDAT_chunkSiz, 4); // chunk length
            file.write("IDAT", 4);

            crc = 0xffffffff;
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'I'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'D'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'A'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'T'];

            for (int y = 0; y < height; y++) {
                file.write("\x00", 1); // filter type
                crc = (crc << 8) ^ crc_table[(crc >> 24) ^ '0'];
                for (int x = 0; x < width; x++) {
                    file.put(ImageData[(y * width + x)].r);
                    file.put(ImageData[(y * width + x)].g);
                    file.put(ImageData[(y * width + x)].b);
                    file.put(ImageData[(y * width + x)].a);
                    crc = (crc << 8) ^ crc_table[(crc >> 24) ^ (ImageData[(y * width + x)].r)];
                    crc = (crc << 8) ^ crc_table[(crc >> 24) ^ (ImageData[(y * width + x)].g)];
                    crc = (crc << 8) ^ crc_table[(crc >> 24) ^ (ImageData[(y * width + x)].b)];
                    crc = (crc << 8) ^ crc_table[(crc >> 24) ^ (ImageData[(y * width + x)].a)];
                    //file.write((char*)&rgba_data[(y * width + x) * 4], 4);
                }
            }
            crc ^= 0xffffffff;
            file.put(crc >> 24); file.put(crc >> 16);
            file.put(crc >> 8); file.put(crc);

            // Write IEND chunk
            file.write("\x00\x00\x00\x00\x49\x4E\x44", 7);
            crc = 0xffffffff;
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'I'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'E'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'N'];
            crc = (crc << 8) ^ crc_table[(crc >> 24) ^ 'D'];
            crc ^= 0xffffffff;
            file.put(crc >> 24); file.put(crc >> 16);
            file.put(crc >> 8); file.put(crc);
            //file.write("IEND", 4);
            //file.write("\xAE\x42\x60\x82", 4); // chunk crc

            file.close();
        }

        void rawDataToBMP(const std::string& filename) {
            std::ofstream file(filename, std::ios::binary);
            if (!file) {
                std::cerr << "Error opening file for writing." << std::endl;
                return;
            }

            int row_size = (width * 4 + 3) & ~3;
            int image_size = row_size * height;
            int file_size = 14 + 40 + image_size;

            // Write BMP file header
            file.write("BM", 2);
            file.write((char*)&file_size, 4);
            file.write("\x00\x00\x00\x00", 4); // reserved
            unsigned int n = (14 + 40);
            file.write((char*)&n, 4); // offset to image data

            // Write BMP image header
            file.write("\x28\x00\x00\x00", 4); // header size
            file.write((char*)&width, 4);
            file.write((char*)&height, 4);
            file.write("\x01\x00", 2); // number of planes
            file.write("\x20\x00", 2); // bits per pixel
            file.write("\x00\x00\x00\x00", 4); // compression
            file.write((char*)&image_size, 4); // image size
            file.write("\x13\x0B\x00\x00", 4); // horizontal resolution
            file.write("\x13\x0B\x00\x00", 4); // vertical resolution
            file.write("\x00\x00\x00\x00", 4); // colors in color table
            file.write("\x00\x00\x00\x00", 4); // important color count

            // Write BMP image data
            for (int y = height - 1; y >= 0; y--) {
                for (int x = 0; x < width; x++) {
                    file.put(ImageData[(y * width + x)].b);
                    file.put(ImageData[(y * width + x)].g);
                    file.put(ImageData[(y * width + x)].r);
                    file.put(ImageData[(y * width + x)].a);
                }
                for (int i = width * 4; i < row_size; i++) {
                    file.write("\x00", 1);
                }
            }

            file.close();
        }
    };

    inline void rawDataToPNG(const std::string& filename, int width, int height, const RGBA_pixel* rgba_data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file for writing." << std::endl;
            return;
        }

        // Write PNG signature
        file.write("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8);

        // Write IHDR chunk
        file.write("\x00\x00\x00\x0D", 4); // chunk length
        file.write("IHDR", 4);
        file.write((char*)&width, 4);
        file.write((char*)&height, 4);
        file.write("\x08\x06\x00\x00\x00", 5); // bit depth, color type, compression, filter, interlace
        file.write("\xC3\x3C\x26\x5A", 4); // chunk crc

        // Write IDAT chunk
        file.write("\x00\x00\x00\x1F", 4); // chunk length
        file.write("IDAT", 4);
        for (int y = 0; y < height; y++) {
            file.write("\x00", 1); // filter type
            for (int x = 0; x < width; x++) {
                file.put(rgba_data[(y * width + x)].r);
                file.put(rgba_data[(y * width + x)].g);
                file.put(rgba_data[(y * width + x)].b);
                file.put(rgba_data[(y * width + x)].a);
                //file.write((char*)&rgba_data[(y * width + x) * 4], 4);
            }
        }
        file.write("\xCD\xAD\x31\xE5", 4); // chunk crc

        // Write IEND chunk
        file.write("\x00\x00\x00\x00", 4);
        file.write("IEND", 4);
        file.write("\xAE\x42\x60\x82", 4); // chunk crc

        file.close();
    }
}

