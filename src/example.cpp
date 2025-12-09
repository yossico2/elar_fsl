#include "example.h"
#include <iostream>

void exampleFunction() {
#ifdef PETALINUX
    std::cout << "Running on PetaLinux" << std::endl;
#else
    std::cout << "Running on Ubuntu/Linux" << std::endl;
#endif
}
