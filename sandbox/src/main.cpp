#include <vermilion/vermilion.hpp>
#include <vermilion/core/instance.hpp>
#include <vermilion/window/window.hpp>

#include <memory>

int main(int argc, char ** argv){

	/* ---
	 * Vermilion initialisation with Vermilion::Window
	 * -- */
	// Create Vermilion renderer instance
	auto vmInstance = std::make_shared<Vermilion::Core::Instance>();
	// Create Vermilion window
	Vermilion::Window::WindowProperties windowProperties(400, 400);
	windowProperties.title = "Test";
	Vermilion::Window::Window vmWindow(vmInstance, windowProperties);
	// Get the window context hints
	Vermilion::Core::ContextProperties * contextProperties = vmWindow.getContextProperties();
//	contextProperties->hintAPI = VMCORE_API_VULKAN;
	contextProperties->hintAPI = VMCORE_API_OPENGL;
	// Create the renderer context
	vmInstance->createContext(contextProperties);
	// Post the renderer context settings to the window and open window
	vmWindow.open(contextProperties);

	// DO SETUP STUFF
	// CREATE BUFFERS
	// SET SETTINGS

	while(vmWindow.shouldClose()){
		// SET RENDERABLE
		// SEND RENDER COMMANDS
		// (SET RENDERABLE AND SEND MORE COMMANDS)
		// PRESENT FINAL RENDERABLE
		vmWindow.present();
	}

	/* ---
	 * Vermilion initialisation with own windowing interface
	 * --- */
	/*
	// Create Vermilion renderer instance
	auto vmInstance = std::make_shared<Vermilion::Core::Instance>();
	// Initialize windowing
	glfwInit();
	// Get the window context hints
	Vermilion::Core::ContextProperties contextProperties;
	// Set contextProperties with window info
	contextProperties.hintAPI = VMCORE_API_OPENGL;
	contextProperties.width = ...;
	contextProperties.height = ...;
	// Create the renderer context
	vmInstance->createContext(&contextProperties);
	// Create window itself with contextProperties.API context
	// Check for VMCORE_API_OPENGL in contextProperties.API
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	window = glfwCreateWindow(....);
	glfwMakeContextCurrent(window);
	contextProperties.opengl_procAddress = (void*)glfwGetProcAddress;
	// Set needed fields in contextProperties
	vmInstance->initContext(&contextProperties);

	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		glfwSwapBuffers(window); // IF OPENGL
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	*/
	

	return 0;
}
