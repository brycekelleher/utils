CC		= clang
CXX		= clang
CFLAGS		= -g -ggdb -DGL_GLEXT_PROTOTYPES
CXXFLAGS	= -g -ggdb -DGL_GLEXT_PROTOTYPES
OBJECTS 	= gl3d.o
LDLIBS		= -lGL -lglut -lm
BIN		= gl3d

ifeq ($(APPLE),1)
CFLAGS += -I/usr/X11R6/include
LDFLAGS = -L/usr/X11R6/lib
endif

$(BIN): $(OBJECTS)

clean:
	rm -rf $(BIN) $(OBJECTS)
