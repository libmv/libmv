all : debug release

debug :
	mkdir -p bin-dbg
	cd bin-dbg && cmake -DCMAKE_BUILD_TYPE=Debug ../src && make

release :
	mkdir -p bin-opt
	cd bin-opt && cmake -DCMAKE_BUILD_TYPE=Release ../src && make
