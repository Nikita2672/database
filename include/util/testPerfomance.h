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
#define FILE_INSERT_DATA "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\insert_performance_data.txt"
#define FILE_UPDATE_DATA "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\update_performance_data.txt"
#define FILE_DELETE_DATA "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\delete_performance_data.txt"
#define FILE_DELETE "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\delete_insert_performance.txt"
#define FILE_INSERT "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\insert_delete_performance.txt"
#else
#define FILE_INSERT_DATA "/home/iwaa0303/CLionProjects/database/src/util/insert_performance_data.txt"
#define FILE_UPDATE_DATA "/home/iwaa0303/CLionProjects/database/src/util/update_performance_data.txt"
#define FILE_DELETE_DATA "/home/iwaa0303/CLionProjects/database/src/util/delete_performance_data.txt"
#define FILE_DELETE "/home/iwaa0303/CLionProjects/database/src/util/delete_insert_performance.txt"
#define FILE_INSERT "/home/iwaa0303/CLionProjects/database/src/util/insert_delete_performance.txt"
#endif
void testInsertPerformance(int64_t amountData);

void testUpdatePerformance(void );

void testDeletePerformance(void );

double testDeleteInsertPerformance(int64_t insertAmount, int32_t deleteAmount, uint16_t i, FILE* fileDataInsert, FILE* fileDataDelete);

NameTypeBlock *buildNameTypeBlock(void );

EntityRecord *buildEntityRecord(void );

#endif //LLP_TESTPERFOMANCE_H
