.SILENT:

.PHONY: all libvermilion sandbox clean run

all: libvermilion sandbox

libvermilion:
	cd libvermilion && ${MAKE} ${MFLAGS} all

sandbox:
	cd sandbox && ${MAKE} ${MFLAGS} all

clean:
	cd libvermilion && ${MAKE} ${MFLAGS} clean
	cd sandbox && ${MAKE} ${MFLAGS} clean

run:
	echo 'RUN'
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:libvermilion/build sandbox/build/sandbox
