#include "blocks.h"
#include "stdio.h"

void writeEmptyTablesBlock(const char *name) {
    FILE* file = fopen(name, "wb");
    if (file == NULL) {
        printf("Error opening file\n");
    }
    struct defineTablesBlock *data = malloc(sizeof (struct defineTablesBlock));
    if (data == NULL) {
        printf("error allocation memory");
    }
    data->countTables = 0;
    data->emptySpaceOffset = sizeof(struct defineTablesBlock) + 1;
    fwrite(data, sizeof(struct defineTablesBlock), 1, file);
    free(data);
    fclose(file);
}

struct defineTablesBlock* readTablesBlock(const char *name) {
    FILE* file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
    }
    struct defineTablesBlock* defineTablesBlock = malloc(sizeof (struct defineTablesBlock));
    fread(defineTablesBlock, sizeof (struct defineTablesBlock), 1, file);
    fclose(file);
    return defineTablesBlock;
}

u_int32_t readTablesCount(const char *name) {
    FILE* file = fopen(name, "rb");
    if (file == NULL) printf("Error opening file\n");
    u_int32_t tablesCount;
    fread(&tablesCount, sizeof (u_int32_t), 1, file);
    fclose(file);
    return tablesCount;
}

void writeTableCount(const char *name, u_int32_t tablesCount) {
    FILE* file = fopen(name, "wb");
    if (file == NULL) printf("Error opening file\n");
    fseek(file, 0, SEEK_SET);
    fwrite(&tablesCount, sizeof (u_int32_t), 1, file);
    fclose(file);
}

u_int64_t readEmptySpaceOffset(const char *name) {
    FILE* file = fopen(name, "rb");
    if (file == NULL) printf("Error opening file\n");
    u_int64_t tableCount;
    fseek(file, (sizeof (struct defineTablesBlock) - sizeof (u_int64_t)), SEEK_SET);
    fread(&tableCount, sizeof (u_int64_t), 1, file);
    fclose(file);
    return tableCount;
}








