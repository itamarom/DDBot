CXXFLAGS="-DSIMULATOR"
ddbot-simulator: stub.o ddbot.ino
	g++ -Wall stub.o -x c++ ddbot.ino $(CXXFLAGS) -o $@

.PHONY: clean prod upload

clean:
	rm -f ddbot-simulator *.o

verify: ddbot.ino build.options.json
	~/arduino/arduino-1.6.9/arduino-builder --build-options-file build.options.json -warnings all ddbot.ino

upload:
	~/arduino/arduino-1.6.9/arduino --upload ddbot.ino
