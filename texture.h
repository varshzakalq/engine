#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>




struct Texture {
    int width = 0;
    int height = 0;
    std::vector<uint32_t> texels; // Holds the 0xAARRGGBB hex colors

    bool load_bmp(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open texture: " << filename << std::endl;
            return false;
        }

        // Read BMP Header information
        unsigned char header[54];
        file.read(reinterpret_cast<char*>(header), 54);

        // Verify it's actually a BMP file (Starts with "BM")
        if (header[0] != 'B' || header[1] != 'M') {
            std::cerr << "Invalid BMP format!" << std::endl;
            return false;
        }

        // Extract width, height, and bits-per-pixel from byte offsets
        width  = *reinterpret_cast<int*>(&header[18]);
        height = *reinterpret_cast<int*>(&header[22]);
        int bpp = *reinterpret_cast<short*>(&header[28]);
        int data_offset = *reinterpret_cast<int*>(&header[10]);

        if (bpp != 24 && bpp != 32) {
            std::cerr << "Only 24-bit or 32-bit BMPs are supported! Yours is: " << bpp << std::endl;
            return false;
        }

        // Jump directly to where pixel data begins
        file.seekg(data_offset);

        texels.resize(width * height);

        // BMP lines are padded to be multiples of 4 bytes
        int row_stride = (width * (bpp / 8) + 3) & ~3;
        std::vector<unsigned char> row_buffer(row_stride);

        for (int y = 0; y < height; ++y) {
            file.read(reinterpret_cast<char*>(row_buffer.data()), row_stride);
            for (int x = 0; x < width; ++x) {
                int p = x * (bpp / 8);
                uint8_t b = row_buffer[p];
                uint8_t g = row_buffer[p + 1];
                uint8_t r = row_buffer[p + 2];
                uint8_t a = (bpp == 32) ? row_buffer[p + 3] : 0xFF;

                // Pack into standard engine hex format (0xRRGGBB)
                uint32_t color = (a << 24) | (r << 16) | (g << 8) | b;
                
                // Save it to our linear pixel buffer
                texels[y * width + x] = color;
            }
        }
        return true;
    }
};

#endif