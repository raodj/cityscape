CC = g++
TARGET= haplos
CFLAGS = -O3 -g -Wall -std=c++11 -DDEVELOPER_ASSERTIONS -o $(TARGET) #-I/usr/local/include/ -L/opt/local/lib/ 
SRC_FILES= src/Location.cpp src/Person.cpp src/Population.cpp src/SedacReader.cpp src/Family.cpp src/haplos.cpp src/ImageGeneration/XFigHelper.cpp src/ImageGeneration/ImageGenerator.cpp src/ImageGeneration/XFigImageGenerator.cpp src/ConfigFile.cpp src/Buildings/Building.cpp src/Buildings/Business.cpp src/Buildings/Medical.cpp src/Buildings/School.cpp

all: 
	$(CC) `Magick++-config --cxxflags --cppflags --ldflags --libs` $(CFLAGS) $(SRC_FILES) $(LIBINCLUDES)

clean:
	$(RM) $(TARGET)
	$(RM) output
