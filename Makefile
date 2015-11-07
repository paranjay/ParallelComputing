all:
	g++ -O3 -pthread -o persistent PersistentThread.cc
	./persistent
