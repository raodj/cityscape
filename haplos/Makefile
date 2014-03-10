CC = g++
TARGET= haplos
CFLAGS = -o $(TARGET) #-I/usr/local/include/ -L/opt/local/lib/ 
SRC_FILES= src/Location.cpp src/Person.cpp src/Population.cpp src/SedacReader.cpp src/ImageGen.cpp src/haplos.cpp

all: 
	sudo $(CC) `Magick++-config --cxxflags --cppflags --ldflags --libs` $(CFLAGS) $(SRC_FILES) $(LIBINCLUDES)

clean:
	$(RM) $(TARGET)
	$(RM) output
