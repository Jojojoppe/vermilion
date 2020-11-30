#include <vermilion/vermilion.hpp>
#include <vermilion/core/instance.hpp>

int main(int argc, char ** argv){

	Vermilion::Core::Instance vmInstance;

	extern void vermilion_window();
	vermilion_window();

	return 0;
}
