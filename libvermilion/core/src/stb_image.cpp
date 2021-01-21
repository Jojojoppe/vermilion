#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.hpp"
#include <string>

unsigned char * Vermilion::Core::loadTextureData(const std::string& path, size_t * width, size_t * height, size_t * channels){
    *width = 0;
    *height = 0;
    *channels = 0;
    return stbi_load(path.c_str(), (int*)width, (int*)height, (int*)channels, STBI_rgb_alpha);
}

void Vermilion::Core::freeTextureData(unsigned char * data){
    stbi_image_free(data);
}