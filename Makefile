all: ray_tracer.o 
	g++ -g -o ray_tracer ray_tracer.o

ray_tracer.o: ray_tracer.cpp
	g++ -c ray_tracer.cpp

clean:
	rm *.o 

