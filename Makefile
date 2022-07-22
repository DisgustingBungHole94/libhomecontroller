SRCDIR = src
STRUCTURE = $(shell find $(SRCDIR) -type d)

DEPSDIR = thirdparty

INCLUDES += -Iinclude/
INCLUDES += -I$(DEPSDIR)/rapidjson/include/

CXX ?= g++
CXXFLAGS ?= -g -fPIC $(INCLUDES)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGET = $(BINARYDIR)/libhomecontroller.so

LIBS += -lllhttp
LIBS += -lpthread
LIBS += -lssl
LIBS += -lcrypto
LIBS += -lboost_system
LIBS += -lboost_random

LIBDIR = /usr/local/lib
INCDIR = /usr/local/include

SOURCES := $(shell find $(SRCDIR) -name '*.cpp')

OBJECTS := $(addprefix $(OBJECTDIR)/,$(SOURCES:%.cpp=%.o))

$(OBJECTDIR)/%.o: | $(OBJECTDIR)
	$(CXX) -c -o $@ $*.cpp $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

install: $(TARGET)
	cp -r include/homecontroller $(INCDIR)
	cp $(TARGET) $(LIBDIR)/libhomecontroller.so

uninstall:
	rm -r $(INCDIR)/homecontroller
	rm $(LIBDIR)/libhomecontroller.so

clean:
	rm -rf bin

.PHONY: clean