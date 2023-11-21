#ifndef LLP_TESTPERFOMANCE_H
#define LLP_TESTPERFOMANCE_H
#include "../../include/file/fileApi.h"
#include "stdio.h"
#include "../../include/file/tableBlocks.h"
#include <string.h>
#include "../../include/query/query.h"
#include "../../include/util/util.h"
#include "../../include/platformic/cutfile.h"


#ifdef _WIN32
#define FILE_NAME "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\file.bin"
#define FILE_NAME_1 "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\testInsert.bin"
#define FILE_NAME_2 "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\data.bin"
#else
#define FILE_NAME "/home/iwaa0303/CLionProjects/lab1/test/file.bin"
#define FILE_NAME_1 "/home/iwaa0303/CLionProjects/lab1/testInsert.bin"
#define FILE_NAME_2 "/home/iwaa0303/CLionProjects/lab1/test/data.bin"
#endif
void testInsertPerformance(uint32_t amountData);

void testUpdatePerformance(void );

void testDeletePerformance(void );

#endif //LLP_TESTPERFOMANCE_H
