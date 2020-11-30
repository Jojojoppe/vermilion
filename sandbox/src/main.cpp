#include <vermilion/vermilion.hpp>
#include <vermilion/core/instance.hpp>
#include <vermilion/window/window.hpp>

#include <memory>

int main(int argc, char ** argv){

	auto vmInstance = std::make_shared<Vermilion::Core::Instance>();
	auto vmWindow = std::make_shared<Vermilion::Window::Window>(vmInstance);

	return 0;
}
