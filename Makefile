CC = g++
CFLAGS = -I./include -I./glm-0.9.7.1
LFLAGS = -L./lib/mac -lfreeimage
DEPS = geometry.hpp

raytracer: main.o geometry.o
	$(CC) -o raytracer main.o geometry.o $(CFLAGS) $(LFLAGS)

main.o: main.cpp geometry.hpp
	$(CC) -c -o main.o main.cpp $(CFLAGS)

geometry.o: geometry.cpp geometry.hpp
	$(CC) -c -o geometry.o geometry.cpp $(CFLAGS)