CFLAGS="-DSIMULATOR"
ddbot-simulator: stub.o ddbot.ino
	g++ stub.o -x c++ ddbot.ino $(CFLAGS) -o $@
