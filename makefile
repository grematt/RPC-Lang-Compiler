rpc:
	g++ -g src/dot_cpp/*.cpp src/dot_h/*.h `llvm-config-17 --cxxflags --ldflags --system-libs --libs all` -o rpc

clean:
	rm rpc
	rm *.o