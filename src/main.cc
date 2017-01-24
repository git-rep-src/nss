#include "nss.h"

#include <iostream>

int main (int argc, char *argv[])
{	
    if (argc > 1) {
        std::cout << std::endl;
        std::cout << "Exploit-DB's Offensive Security Ncurses search tool " <<
                     "(https://github.com/git-rep/nss)" << std::endl;
        std::cout << "Version 0.1" << std::endl;
        std::cout << std::endl;
        return 0;
    }
    
    Nss nss;
    
    return 0;
}
