//
// Created by kingdo on 2021/5/12.
//
#include <RFIT/R/resource.h>

using namespace std;

int main() {
    printf("%ld\n", R::getRCount());
    R::getUniqueR();
    printf("%ld\n", R::getRCount());
    R::getUniqueR();
    printf("%ld\n", R::getRCount());
    return 0;
}

