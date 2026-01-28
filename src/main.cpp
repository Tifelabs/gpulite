#include <iostream>


void options(){
    std::cout << "\t\t SELECT GPU OPTION\n";
    std::cout << R"(
                    "[1] ::VIEW GPU STATUS"
                    "[2] ::GPU BENCHMARK"
                    "[3] ::GPU PROCESS"
                    "[Q] ::QUIT"
    )" << "\n";
}

int main(int argc, char *argv[]){
    
        options();

    return 0;
}