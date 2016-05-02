CC = g++
TARGET= haplos
CFLAGS = -g -O3 -Wall -std=c++11 -DDEVELOPER_ASSERTIONS -o $(TARGET) #-I/usr/local/include/ -L/opt/local/lib/
SRC_FILES= src/Location.cpp src/Buildings/Building.cpp src/Person.cpp src/Population.cpp src/SedacReader.cpp src/Family.cpp src/main.cpp src/ImageGeneration/XFigHelper.cpp src/ImageGeneration/ImageGenerator.cpp src/ImageGeneration/XFigImageGenerator.cpp src/Files/ConfigFile.cpp src/Files/TimelineFile.cpp src/Files/ImageFileGenerator.cpp src/Buildings/Daycare.cpp src/Buildings/TransportHub.cpp src/Buildings/Business.cpp src/Buildings/Medical.cpp src/Buildings/School.cpp src/Schedule/ScheduleGenerator.cpp src/Schedule/Schedule.cpp src/Schedule/TimeSlot.cpp src/Buildings/BuildingGenerator.cpp src/Policy.cpp src/Haplos.cpp src/Files/ArgParser.cpp

all: 
	$(CC) `Magick++-config --cxxflags --cppflags --ldflags --libs` $(CFLAGS) $(SRC_FILES) $(LIBINCLUDES)

clean:
	$(RM) $(TARGET)
	$(RM) output
