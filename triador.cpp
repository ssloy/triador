#undef NDEBUG
#include <cassert>
#include <iostream>
#include <cstring>

int main(int argc, char** argv) {
    if (2>argc) {
        std::cerr << "Usage: " << argv[0] << " program.txt" << std::endl;
        return 1;
    }

    return 0;
}

