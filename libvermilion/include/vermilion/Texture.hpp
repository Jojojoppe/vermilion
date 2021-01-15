#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Vermilion{
namespace Core{

class Texture{
    public:
        size_t width, height;
        unsigned int channels;
        unsigned int mipLevels;

        ~Texture() = default;

        virtual void setData(void * data){};
};

class Sampler{
    public:
        ~Sampler() = default;
};

}};