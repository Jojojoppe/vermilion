#pragma once

#include <vermilion/vermilion.hpp>
#include <vermilion/logger.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Vermilion{
namespace Core{

class Texture{
    public:
        size_t width, height, size;
        unsigned int channels;
        unsigned int mipLevels;

        ~Texture() = default;

        virtual void setData(void * data, size_t size=0){};
};

class Sampler{
    public:
        ~Sampler() = default;
};

unsigned char * loadTextureData(const std::string& path, size_t * width, size_t * height, size_t * channels);
void freeTextureData(unsigned char * data);

}};