all:
	g++ -O3 -pthread -fopenmp -o omp OmpVersion.cc
	./omp

persistent:
	g++ -O3 -pthread -o persistent PersistentThread.cc
	./persistent

pthreaded:
	g++ -O3 -pthread -o pthreaded PThreadVersion.cc
	./pthreaded

serial:
	g++ -O3 -o serial SerialVersion.cc
	./serial

omp:
	g++ -O3 -pthread -fopenmp -o omp OmpVersion.cc
	./omp
