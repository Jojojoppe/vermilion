#include <cstdio>

void vermilion_window(){
	printf("vermilion_window()\r\n");

	#ifdef VMWINDOW_UNIX_X11
	printf("X11 supported\r\n");
	#endif
	#ifdef VMWINDOW_UNIX_GLFW
	printf("GLFW supported\r\n");
	#endif

}
