mkdir build 2>/dev/null || true
mkdir build/Main 2>/dev/null || true
cp -r src/. build/Main

arduino compile --fqbn arduino:avr:uno /home/thepigcat/coding/arduino/Transmitter/build/Main/
arduino upload --fqbn arduino:avr:uno -p /dev/ttyACM0 /home/thepigcat/coding/arduino/Transmitter/build/Main/