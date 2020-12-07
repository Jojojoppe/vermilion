#include <vermilion/vermilion.hpp>
#include <vermilion/core/instance.hpp>
#include <vermilion/window/window.hpp>

#include <memory>

int main(int argc, char ** argv){

	// Create Vermilion renderer instance
	auto vmInstance = std::make_shared<Vermilion::Core::Instance>();
	// Create Vermilion window
	Vermilion::Window::WindowProperties windowProperties(400, 400);
	windowProperties.title = "Test";
	Vermilion::Window::Window vmWindow(vmInstance, windowProperties);
	// Get the window context hints
	Vermilion::Core::ContextProperties * contextProperties = vmWindow.getContextProperties();
	contextProperties->hintAPI = VMCORE_API_OPENGL;
	// Create the renderer context
	vmInstance->createContext(contextProperties);
	// Post the renderer context settings to the window and open window
	vmWindow.open(contextProperties);
	// Call vmInstance->initContext(contextProperties) if not using Vermilion::Window
	//contextProperties.procAddress = ....
	//vmInstance->initContext(contextProperties);


	while(true){
	}

	return 0;
}
