# threader.std

The library is designed to create stable multithreaded applications based on data exchange using unified messages. The library supports working with external devices and system descriptors through the poll mechanism and minimizes processor load.

## Download and install

```bash
git clone https://github.com/99nails/threader.std.git
cd threader.std
mkdir build
cd build
cmake ..
make -j`nproc`
```

## Examples

Usage examples are located in ```threader.std/sources/libraries/threader.std/examples/```

Example #1 demonstrates the general principles of thread message transmission and processing without a waiting subsystem.

Example #2 shows simple message processing in a thread using library classes.

Example #3 implements a message exchange between two threads.

Example #4 performs data transfer between two threads of the same application via serial ports.

Example binary files are located in ```threader.std\bin\debug```
