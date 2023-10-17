#include "iterator.h"
#include "../data/data.h"
#include "fileApi.h"
#include "dataBlocks.h"
#include "../query/query.h"

bool hasNext (struct iterator* iterator, const char* fileName) {
    FILE *file = fopen(fileName, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return false;
    }
    struct headerSection headerSection;
    fseek(file, iterator->blockOffset, SEEK_CUR);
    fread(&headerSection, sizeof (struct headerSection), 1, file);
    bool hasNext = false;
    for (uint16_t i = iterator->currentPositionInBlock; i < headerSection.recordsNumber; i++) {
        struct EntityRecord* entityRecord = readRecord(file, i, iterator->blockOffset,iterator->fieldsNumber);
        bool valid = true;
        for (uint16_t j = 0; j < iterator->predicateNumber; j++) {
            bool result = checkPredicate(&iterator->predicate[j], entityRecord, iterator->fieldsNumber);
            if (!result) {
                valid = false;
                break;
            }
        }
        // дописать malloc entityRecord
        if (valid) {
            hasNext = true;
            iterator->currentPositionInBlock = i;
            return hasNext;
        }
    }
    // дописать логику перехода на следующий блок при наличии в противном случае вернуть false
}


struct EntityRecord* next(struct iterator* iterator, char * fileName) {

}
