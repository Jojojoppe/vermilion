#ifdef VMCORE_OPENGL
#include "Texture.hpp"
#include "api.hpp"

#include <stdexcept>
#include <cstdint>
#include <cstring>

static const unsigned int OpenGLChannelFormatField[]{
    GL_RGBA,
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,
};

Vermilion::Core::OpenGL::Texture::Texture(Vermilion::Core::OpenGL::API * api, size_t width, size_t height, unsigned int channels){
    this->api = api;
    this->instance = api->instance;

    // Create texture with pixel data
    this->width = width;
    this->height = height;
    this->channels = channels;
    this->size = width*height*channels;

    this->format = OpenGLChannelFormatField[channels];

    glGenTextures(1, &this->texture);
    char * px = new char[this->size];
    for(int i=0; i<this->size; i++){
        px[i] = 0;
    }
    this->setData(px, this->size);
    delete[] px;
}

Vermilion::Core::OpenGL::Texture::~Texture(){
    glDeleteTextures(1, &this->texture);
}

void Vermilion::Core::OpenGL::Texture::setData(void * data, size_t size){
	if(size==0 || size>this->size){
		size = this->size;
	}

    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, this->format, this->width, this->height, 0, this->format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Vermilion::Core::OpenGL::Sampler::Sampler(Vermilion::Core::OpenGL::API * api, std::shared_ptr<Vermilion::Core::Texture> texture){
    this->api = api;
    this->instance = api->instance;
    this->texture = std::static_pointer_cast<Vermilion::Core::OpenGL::Texture>(texture);

    glGenSamplers(1, &this->sampler);
    glBindSampler(0, this->sampler);
    glSamplerParameteri(this->sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(this->sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(this->sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(this->sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Vermilion::Core::OpenGL::Sampler::~Sampler(){
    glDeleteSamplers(1, &this->sampler);
}

#endif