//
// Created by kingdo on 2021/5/14.
//

#include <RFIT/F/function.h>
#include "test/test.h"

using namespace std;

int main() {
    F::registerF("taskB", taskT, Resource(), 100);
    F::registerF("taskB", taskT, Resource(), 100);
    F *f_B = F::getF("taskB");
    f_B->getEntryAddr()(reinterpret_cast<FCGX_Request *>(1));
    return 0;
}