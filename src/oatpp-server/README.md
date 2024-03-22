# oatpp-starter

## Overview

### Project layout

```
|- CMakeLists.txt                        // projects CMakeLists.txt
|- src/
|    |
|    |- controller/                      // Folder containing MyController where all endpoints are declared
|    |- dto/                             // DTOs are declared here
|    |- AppComponent.hpp                 // Service config
|    |- App.cpp                          // main() is here
|
|- test/                                 // test folder
|- utility/install-oatpp-modules.sh      // utility script to install required oatpp-modules.  
```

---

### Build and Run

In order to build and run the oat++ server, locate the `oatpp-starter` folder, then follow the guide below (run in Git Bash).

```
$ mkdir build
$ git clone https://github.com/oatpp/oatpp.git
$ cd oatpp 
$ mkdir build && cd build
$ cmake -G "Visual Studio 17 2022" ..
$ cmake --build . --target INSTALL
$ cd ../
$ cd ../
$ cd build
$ cmake -G "Visual Studio 17 2022" ..
$ cmake --build .
$ cd Debug
$ ./my-project-exe  # - run application.
```

If you make any changes and want to rerun the server, follow the guide below.

```
$ rm -rf cmakecache.txt
$ cmake -G "Visual Studio 17 2022" ..
$ cmake --build .
$ cd Debug
$ ./my-project-exe
```
