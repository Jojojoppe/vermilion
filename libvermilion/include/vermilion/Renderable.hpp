#pragma once

#include <vermilion/logger.hpp>
#include <vermilion/Buffer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Vermilion{
namespace Core{

class Renderable{
    public:
        std::shared_ptr<Buffer> vertexBuffer;
        std::shared_ptr<Buffer> indexBuffer;

        unsigned int vertexOffset;
        unsigned int indexOffset;
        unsigned int length;

        ~Renderable() = default;
};

}}