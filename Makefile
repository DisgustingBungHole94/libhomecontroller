SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

INCDIR = include
INCDIRNAME = homecontroller

CXX = g++
CXXFLAGS ?= -g -std=c++17 -fPIC -I$(INCDIR)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGETNAME = libhomecontroller.so
TARGET = $(BINARYDIR)/$(TARGETNAME)

LIBS += -lllhttp
LIBS += -lssl

LIBINSTALLDIR = /usr/local/lib
INCINSTALLDIR = /usr/local/include

# app
_OBJECTS += app/device.o
_HEADERS += app/device.h

_OBJECTS += app/session.o
_HEADERS += app/session.h

_OBJECTS += app/user.o
_HEADERS += app/user.h

# exception
_HEADERS += exception/exception.h

# http
_OBJECTS += http/http_request.o
_HEADERS += http/http_request.h

_OBJECTS += http/http_response.o
_HEADERS += http/http_response.h

_OBJECTS += http/http_parser.o
_HEADERS += http/http_parser.h

# net/ssl/connection
_OBJECTS += net/ssl/connection/connection.o
_HEADERS += net/ssl/connection/connection.h

_OBJECTS += net/ssl/connection/server_connection.o
_HEADERS += net/ssl/connection/server_connection.h

_OBJECTS += net/ssl/connection/client_connection.o
_HEADERS += net/ssl/connection/client_connection.h

# net/ssl
_OBJECTS += net/ssl/ssl.o
_HEADERS += net/ssl/ssl.h

_OBJECTS += net/ssl/tls_client.o
_HEADERS += net/ssl/tls_client.h

_OBJECTS += net/ssl/tls_server.o
_HEADERS += net/ssl/tls_server.h

# thread
_OBJECTS += thread/thread_pool.o
_HEADERS += thread/thread_pool.h

# util
_OBJECTS += util/logger.o
_HEADERS += util/logger.h

_OBJECTS += util/string.o
_HEADERS += util/string.h

_OBJECTS += util/config.o
_HEADERS += util/config.h

_OBJECTS += util/timer.o
_HEADERS += util/timer.h

# ws/connection
_OBJECTS += ws/connection/connection.o
_HEADERS += ws/connection/connection.h

_OBJECTS += ws/connection/server_connection.o
_HEADERS += ws/connection/server_connection.h

_OBJECTS += ws/connection/client_connection.o
_HEADERS += ws/connection/client_connection.h

# ws
_OBJECTS += ws/ws_client.o
_HEADERS += ws/ws_client.h

_OBJECTS += ws/ws_server.o
_HEADERS += ws/ws_server.h

OBJECTS = $(patsubst %,$(OBJECTDIR)/%,$(_OBJECTS))
HEADERS = $(patsubst %,$(INCDIR)/$(INCDIRNAME)/%,$(_HEADERS))

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJECTDIR)
	$(CXX) $(CPPFLAGS) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

install: $(TARGET)
	cp -r $(INCDIR)/$(INCDIRNAME) $(INCINSTALLDIR)
	cp $(TARGET) $(LIBINSTALLDIR)/$(TARGETNAME)

uninstall:
	rm -r $(INCINSTALLDIR)/$(INCDIRNAME)
	rm $(LIBINSTALLDIR)/$(TARGETNAME)

clean:
	rm -rf bin

.PHONY: clean install uninstall