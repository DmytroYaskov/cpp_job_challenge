TARGET = findfile

$(TARGET): main.o libfindfile.a
	g++ $^ -o $@ -std=c++11 -pthread

main.o: main.cpp
	g++ -c $< -o $@

libfindfile.a: libfindfile.o
	ar rcs $@ $^

libfindfile.o: findfile.cpp findfile.h
	g++ -c -o $@ $<

#clean:
#    rm -f *.o *.a $(TARGET)