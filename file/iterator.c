#include "iterator.h"
#include "../data/data.h"
#include "fileApi.h"
#include "dataBlocks.h"
#include "../query/query.h"

bool hasNext (struct iterator* iterator, FILE* file) {
    struct headerSection headerSection;
    fseek(file, iterator->blockOffset, SEEK_SET);
    fread(&headerSection, sizeof (struct headerSection), 1, file);
    bool hasNextVariable = false;
    for (uint16_t i = iterator->currentPositionInBlock; i < headerSection.recordsNumber; i++) {
        struct EntityRecord* entityRecord = readRecord(file, i, iterator->blockOffset,iterator->fieldsNumber);
        bool valid = true;
        for (uint16_t j = 0; j < iterator->predicateNumber; j++) {
            bool result = checkPredicate(&iterator->predicate[j], entityRecord, iterator->fieldsNumber, iterator->nameTypeBlock);
            if (!result) {
                valid = false;
                break;
            }
        }
//        freeEntityRecord(entityRecord, iterator->fieldsNumber);
        if (valid) {
            hasNextVariable = true;
            iterator->currentPositionInBlock = (i + 1);
            return hasNextVariable;
        }
    }
    fseek(file, iterator->blockOffset + sizeof (struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
    struct specialDataSection specialDataSection;
    fread(&specialDataSection, sizeof (struct specialDataSection), 1, file);
    if (specialDataSection.nextBlockOffset != 0) {
        iterator->currentPositionInBlock = 0;
        iterator->blockOffset = specialDataSection.nextBlockOffset;
        return hasNext(iterator, file);
    } else {
        return false;
    }
}


struct EntityRecord* next(struct iterator* iterator, FILE* file) {
    return readRecord(file, iterator->currentPositionInBlock - 1, iterator->blockOffset,iterator->fieldsNumber);
}
