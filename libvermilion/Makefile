.SILENT:

.PHONY: all core window clean

all: core window

core:
	cd core && ${MAKE} ${MFLAGS}
	-mkdir -p build
	cp core/build/libvermilion_core.so build

window:
	cd window && ${MAKE} ${MFLAGS}
	-mkdir -p build
	cp window/build/libvermilion_window.so build

clean:
	cd core && ${MAKE} ${MFLAGS} clean
	cd window && ${MAKE} ${MFLAGS} clean
	-rm build/*
