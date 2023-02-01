#pragma once
#include <iostream>
#include <fstream>
#include "Utill_FreeMemory.h"

namespace imgform {
#define imgform_init freemem::FM_Model0 PixelImageObject::ImageOperateData_tempFM;

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
                    file.put(ImageData[(y * width + x)].r);
                    file.put(ImageData[(y * width + x)].g);
                    file.put(ImageData[(y * width + x)].b);
                    file.put(ImageData[(y * width + x)].a);
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

