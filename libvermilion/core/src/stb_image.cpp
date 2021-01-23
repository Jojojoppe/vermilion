#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.hpp"
#include <string>

unsigned char * Vermilion::Core::loadTextureData(const std::string& path, size_t * width, size_t * height, size_t * channels){
    *width = 0;
    *height = 0;
    *channels = 4;
    Vermilion::Core::flipLoading();
    return stbi_load(path.c_str(), (int*)width, (int*)height, nullptr, STBI_rgb_alpha);
}

void Vermilion::Core::freeTextureData(unsigned char * data){
    stbi_image_free(data);
}

void Vermilion::Core::flipLoading(){
	stbi_set_flip_vertically_on_load(true);  
}