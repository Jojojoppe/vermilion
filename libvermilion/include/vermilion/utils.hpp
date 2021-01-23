#pragma once

#include <vector>
#include <string>

namespace Vermilion{
namespace Utils{

void loadObj(std::vector<float>& vertices, std::vector<unsigned int>&indices, const std::string& path);

}}