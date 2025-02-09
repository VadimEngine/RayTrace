#pragma once
// standard lib
#include <memory>

struct FileData {
    std::unique_ptr<unsigned char[]> data;
    std::size_t size;
}; 

struct ImageData {
    unsigned char* pixels = nullptr;
    int width;
    int height;
    int channels;
};
