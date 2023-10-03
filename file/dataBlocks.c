//
// Created by iwaa0303 on 03/10/23.
//

#include "dataBlocks.h"

struct specialDataSection *initSpecialDataSection() {
    struct specialDataSection *specialDataSection = malloc(sizeof(struct specialDataSection));
    return specialDataSection;
}

struct headerSection *initHeaderSection() {
    struct headerSection *headerSection = malloc(sizeof(struct headerSection));
    return headerSection;
}

struct recordId *initRecordId() {
    struct recordId *recordId = malloc((sizeof(struct recordId)));
    return recordId;
}