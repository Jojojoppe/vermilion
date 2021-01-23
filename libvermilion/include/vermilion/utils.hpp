#pragma once

#include <vermilion/vermilion.hpp>
#include <vector>
#include <unordered_map>
#include <string>

namespace Vermilion{
namespace Utils{

struct mesh{
    size_t length;
    size_t offset;
    std::string materialName;
};

struct material{
    // Diffuse texture
    bool use_tex_d = false;
    std::string tex_d;
    // Diffuse color
    float Kdr, Kdg, Kdb;
    // Ambient color
    float Kar, Kag, Kab;
};

void loadObj(std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<mesh>& meshes, std::unordered_map<std::string, material>& materials, const std::string& objname, const std::string& path);

}}