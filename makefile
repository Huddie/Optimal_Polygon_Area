SOURCES = $(shell find helper -name '*.cpp')
HEADERS = $(shell find helper -name '*.hpp')
OBJ = ${SOURCES:.cpp=.o}

CC = g++
CGALFLAGS =  -lCGAL -I/usr/local/Cellar/cgal/4.14/include/ -I/usr/local/Cellar/boost/1.69.0_2/include -lmpfr -lgmp -std=c++11

.PHONY: main

main: main.cpp ${OBJ}
	${CC} ${CGALFLAGS} ${OBJ} $< -o $@

clean:
	rm -r ${OBJ}

%.o: %.cpp ${HEADERS}
	${CC} ${CGALFLAGS} -c $< -o $@
