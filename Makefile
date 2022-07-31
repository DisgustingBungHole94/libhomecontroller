SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

INCDIRNAME = homecontroller
INCDIR = include/$(INCDIRNAME)

CXX ?= g++
CXXFLAGS ?= -g -fPIC -I$(INCDIR)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGETNAME = libhomecontroller.so
TARGET = $(BINARYDIR)/$(TARGETNAME)

LIBS += -lllhttp
LIBS += -lssl

LIBINSTALLDIR = /usr/local/lib
INCINSTALLDIR = /usr/local/include

# src root
_OBJECTS += device.o
_HEADERS += device.h

_OBJECTS += http_parser.o
_HEADERS += http_parser.h

_OBJECTS += tls_client.o
_HEADERS += tls_client.h

OBJECTS = $(patsubst %,$(OBJECTDIR)/%,$(_OBJECTS))
HEADERS = $(patsubst %,$(INCDIR)/%,$(_HEADERS))

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJECTDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

install: $(TARGET)
	cp -r $(INCDIR) $(INCINSTALLDIR)
	cp $(TARGET) $(LIBINSTALLDIR)/$(TARGETNAME)

uninstall:
	rm -r $(INCINSTALLDIR)/$(INCDIRNAME)
	rm $(LIBINSTALLDIR)/$(TARGETNAME)

clean:
	rm -rf bin

.PHONY: clean