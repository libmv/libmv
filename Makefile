all : debug release

debug :
	mkdir -p bin-dbg
	cd bin-dbg && cmake -DCMAKE_BUILD_TYPE=Debug ../src && make

release :
	mkdir -p bin-opt
	cd bin-opt && cmake -DCMAKE_BUILD_TYPE=Release ../src && make

clean :
	rm -rf bin-opt
	rm -rf bin-dbg

test : debug
	cd bin-dbg && ctest

test-release : release
	cd bin-opt && ctest
	
