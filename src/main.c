#include "../include/file/tableBlocks.h"
#include "../include/util/unitTests.h"
#include "../include/util/testPerfomance.h"

static void unitTest(void) {
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
    test13();
}

static void performanceTest(void) {
    testInsertPerformance(1000);
    testUpdatePerformance();
    testDeletePerformance();
}

int main(void) {
//    unitTest();
//    performanceTest();
    testInsertPerformance(0);
    FILE *fileDataInsert = fopen(FILE_INSERT, "r+");
    FILE *fileDataDelete = fopen(FILE_DELETE, "r+");
    for (uint16_t i = 0; i < 100; i++) {
        testDeleteInsertPerformance(500, 400, i, fileDataInsert, fileDataDelete);
    }
    fclose(fileDataInsert);
    fclose(fileDataDelete);

//    FILE *file = fopen(FILE_NAME, "rb+");
//    printMetaTableRecords(file);
//    fclose(file);

    return 0;
}
