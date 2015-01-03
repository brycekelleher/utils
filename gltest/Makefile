OBJECTS	= gltest.o

#ifeq ($(APPLE),1)
CFLAGS += -I/usr/X11R6/include -DGL_GLEXT_PROTOTYPES
LDFLAGS = -L/usr/X11R6/lib
LDLIBS  = -lGL -lglut
#endif

gltest: $(OBJECTS)

clean:
	rm -rf gltest gltest.o
