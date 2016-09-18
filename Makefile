ifndef OBS_INCLUDE
OBS_INCLUDE = /usr/include/obs
endif
ifndef OBS_LIB
OBS_LIB = /usr/lib
endif

CXX = g++
CXXFLAGS = -g -Wall -std=c++11 -fPIC

CCC = gcc
CCCFLAGS = -g -Wall -std=c11 -fPIC

INCLUDE = -I$(OBS_INCLUDE)
LDFLAGS = -L$(OBS_LIB)
LDLIBS_LIB   = -lobs	#libs for obs

LIB = TS3Plugin.so
LIB_OBJ = ts3plugin.o ts3plugin_settings.o isStreamRecord.o utility.o slothSock.o overlayComs.o originalFeatures.o
LIB_OBJ0 = ts3plugin.o
LIB_OBJ1 = ts3plugin_settings.o
LIB_OBJ2 = isStreamRecord.o
LIB_OBJ3 = utility.o
LIB_OBJ4 = slothSock.o
LIB_OBJ5 = overlayComs.o
LIB_OBJ6 = originalFeatures.o
SRC0 = ts3plugin.cpp
SRC1 = ts3plugin_settings.c
SRC2 = isStreamRecord.c
SRC3 = utility.cpp
SRC4 = slothSock.cpp
SRC5 = overlayComs.cpp
SRC6 = originalFeatures.cpp

all: $(LIB)

$(LIB): $(LIB_OBJ)
	$(CXX) -shared $(LDFLAGS) $^ $(LDLIBS_LIB) -o $@

$(LIB_OBJ0): $(SRC0)
	$(CXX) -c $(CXXFLAGS) $^ $(INCLUDE) -o $@

$(LIB_OBJ1): $(SRC1)
	$(CCC) -c $(CCCFLAGS) $^ $(INCLUDE) -o $@

$(LIB_OBJ2): $(SRC2)
	$(CCC) -c $(CCCFLAGS) $^ $(INCLUDE) -o $@

$(LIB_OBJ3): $(SRC3)
	$(CXX) -c $(CXXFLAGS) $^ $(INCLUDE) -o $@

$(LIB_OBJ4): $(SRC4)
	$(CXX) -c $(CXXFLAGS) $^ $(INCLUDE) -o $@

$(LIB_OBJ5): $(SRC5)
	$(CXX) -c -fpermissive $(CXXFLAGS) $^ $(INCLUDE) -o $@

$(LIB_OBJ6): $(SRC6)
	$(CXX) -c -fpermissive $(CXXFLAGS) $^ $(INCLUDE) -o $@

#Install for obs-studio from PPA
.PHONY: install
install:
	sudo mkdir /usr/share/obs/obs-plugins/TS3Plugin
	sudo cp -r ./locale /usr/share/obs/obs-plugins/TS3Plugin/
	sudo chmod 755 /usr/share/obs/obs-plugins/TS3Plugin/locale
	sudo chmod 644 /usr/share/obs/obs-plugins/TS3Plugin/locale/*
	sudo cp $(LIB) /usr/lib/obs-plugins/

.PHONY: clean
clean:
	$(RM) $(LIB_OBJ) $(LIB)
	sudo rm -r /usr/lib/obs-plugins/$(LIB)
	sudo rm -r /usr/share/obs/obs-plugins/TS3Plugin

