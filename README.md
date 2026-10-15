# threader.std

The library is designed to create stable multithreaded applications based on data exchange using 
unified messages. The library supports working with external devices and system descriptors through 
the poll mechanism and minimizes processor load.

## Download and install

```bash
git clone https://github.com/99nails/threader.std.git
cd threader.std
mkdir build
cd build
cmake ..
make -j`nproc`
sudo make install
cpack -G DEB
```

## Deploying

```bash
sudo dpkg -i *.deb
libthreader-*-Linux-std-lib.deb
libthreader-*-Linux-std-dev.deb
libthreader-*-Linux-qt-lib.deb
libthreader-*-Linux-qt-dev.deb
```

## Examples

Usage examples are located in ```threader.std/sources/libraries/threader.std/examples/```

Example binary files are located in ```threader.std\bin\debug```

### List of examples

1. Example #1 demonstrates the general principles of thread message transmission and processing 
without support of waiting subsystem.
1. Example #2 shows simple message processing in a thread using library classes.
1. Example #3 implements a message exchange between two threads.
1. Example #4 performs data transfer between two threads of the same application via serial ports.
1. Example #5 consist of 3 applications:
    - GPS emulator that sends data in NMEA format into serial port;
    - Application that receive NMEA data and distribute GPS date and time through the TCP socket 
    like a TCP server;
    - TCP client that receive date and time information from remote server and set system time on 
    local host.
1. Example #6 waits for udev events of plug-in USB device with specified pid/vid.
1. Example #7-1 demostrates ability of using qt eventloop with threader-std library if it is 
necessary.
1. Example #7-2 shows udev events in QMainWindow.
1. Example #8 is not complited....
1. Example #9 shows timer usage in ThreadBase class.
