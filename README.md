# cinemo-test

trunk
├── bin
├── lib
│   └── project
│       └── libvector3.so
│       └── libvector3.a        products of installation / building
├── docs
│   └── Doxyfile
├── include
│   └── project
│       └── vector3.hpp
│_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
│
├── src
│   └── CMakeLists.txt
│   └── Doxyfile.in
│   └── project                 part of version-control / source-distribution
│       └── CMakeLists.txt
│       └── vector3.hpp
│       └── vector3.cpp
│       └── test
│           └── test_vector3.cpp
│_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
│
├── build
└── test                        working directories for building / testing
    └── test_vector3