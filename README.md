# cinemo-test
Write a C/C++ commandline application that encodes a set of WAV files to MP3

## Requirements:

1. application is called with pathname as argument, e.g.
```
$> <application_name> F:\MyWavCollection
```
all WAV-files contained directly in that folder are to be encoded to MP3
2. Use all available CPU cores for the encoding process in an efficient way by utilizing multi-threading
3. Statically link to lame encoder library
4. Application should be compilable and runnable on Windows and Linux
5. The resulting MP3 files are to be placed within the same directory as the source WAV files, the filename extension should be changed appropriately to .MP3
6. Non-WAV files in the given folder shall be ignored
7. Multi-threading shall be implemented in a portable way, for example using POSIX pthreads.
8. The Boost library shall not be used
9. The LAME encoder should be used with reasonable standard settings (e.g. quality based encoding with quality level "good")

## Compilation
The executable will be deployed to the bin directory inside the project folder.

### Debian/Ubuntu
- Install needed tools to compile C/C++ CMake project
```
$> sudo apt install cmake make gcc g++
```
- Install development package for libmp3lame library
```
$> sudo apt install libmp3lame-dev
```
- Generate the binary file
```
$> cd </path/to/project/directory>
$> cmake CMakeLists.txt
$> make
$> make install
```

### Windows with Cygwin
- Install the latest version of Cygwin
- Install package libmp3lame-dev on Cygwin GUI
- Run Cygwin.bat from Cygwin home directory
- Generate the binary file
```
$> cd </path/to/project/directory>
$> cmake CMakeLists.txt
$> make
$> make install
```

## Execution
```
$> </path/to/binary/file> </path/to/process/directory>
```



