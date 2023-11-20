#include "../include/file/tableBlocks.h"
#include "../include/util/unitTests.h"
#include "../include/util/testPerfomance.h"

static void unitTest(void ) {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    test11();
    test12();
}

static void performanceTest(void ) {
    testInsertPerformance(1000);
    testUpdatePerformance();
    testDeletePerformance();
}

int main(void) {
    unitTest();
//    performanceTest();
//    testInsertPerformance(1000000);
    return 0;
}
