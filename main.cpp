#include <iostream>
#include "triador.h"

int main(int argc, char** argv) {
    if (argc!=2) {
        std::cerr << "Usage: " << argv[0] << " ../prog/add.txt" << std::endl;
        return 1;
    }

    Triador triador;
    triador.load_program(argv[1]);
    triador.run();

    return 0;
}

