INCDIR=include/homecontroller
SRCDIR=src

IDIRS = -I./include -I./rapidjson/include -I./llhttp/build
LDIR = lib

CXX=g++
CXXFLAGS=-g -fPIC $(IDIRS) -L$(LDIR) -Wl,-rpath,./$(LDIR)

TARGET=bin/libhomecontroller.so
ODIR=bin/obj

LIBS= -lllhttp

_DEPS =	http_parser.h tls_client.h device.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ = http_parser.o tls_client.o device.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS) | bin
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): llhttp/build/libllhttp.a $(OBJ)
	$(CXX) -shared -o $@ $^ $(CXXFLAGS) $(LIBS)

bin:
	mkdir $@
	mkdir $@/obj

llhttp/build/libllhttp.a:
	cd llhttp && npm install
	cd llhttp && make
	mkdir lib
	cd lib && ln -s ../llhttp/build/libllhttp.a libllhttp.a

.PHONY: clean

clean:
	find $(ODIR) -type f -name '*.o' -delete
	rm -f $(TARGET)

cleanall: clean
	rm -r bin
	rm -r lib
	cd llhttp && make clean