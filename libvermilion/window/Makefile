.SILENT:

TARGET				:= libvermilion_window.so
BUILDDIR			:= build
SRCDIRS				:= src

SRCFILES			:= $(shell find $(SRCDIRS) -type f -name "*.cpp")
OBJFILES			:= $(SRCFILES:%.cpp=$(BUILDDIR)/%.cpp.o)
DEPFILES			:= $(SRCFILES:%.cpp=$(BUILDDIR)/%.cpp.d)

FILESTOCLEAN		:= $(OBJFILES) $(DEPFILES) $(BUILDDIR)/$(TARGET)

CC					:= g++
AR					:= ar
LD					:= ld
AS					:= as

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:=
CC_FLAGS			:= -fPIC
CC_INCLUDES			:= -I src -I ../../libvermilion/include

LD_FLAGS 			:= -shared

# Add DEPFILE dependencies
-include $(DEPFILES)

.PHONY: $(TARGET) all clean
all: $(TARGET)

# PHONY RULES
# -----------

$(TARGET): $(BUILDDIR)/$(TARGET)

clean:
	echo CLEAN FILES FOR $(TARGET)
	-rm -r $(FILESTOCLEAN)

# SPECIFIC BUILD RULES
# --------------------

$(BUILDDIR)/$(TARGET): $(OBJFILES)
	echo 'LD ' $@
	-mkdir -p $(shell dirname $@)
	$(LD) $(LD_FLAGS) -o $(BUILDDIR)/$(TARGET) $(OBJFILES) $(CC_LIBS)

$(OBJFILES): $(BUILDDIR)/%.cpp.o: %.cpp
	echo 'CC ' $@
	-mkdir -p $(shell dirname $@)
	$(CC) $(CC_FLAGS) $(CC_WARNING) $(CC_INCLUDES) -MD -o $@ -c $<

