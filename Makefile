CXXFLAGS="-DSIMULATOR"
ddbot-simulator: stub.o ddbot.ino
	g++ -Wall stub.o -x c++ ddbot.ino $(CXXFLAGS) -o $@
