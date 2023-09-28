#include <iostream>
#include "Application.h"

int main() {
    try {
        return Application().run();
    } catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
