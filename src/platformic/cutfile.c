#include <stdio.h>
#include "../../include/platformic/cutfile.h"

#ifdef _WIN32
#include <io.h>
#else

#include <unistd.h>

#endif

void cutFile(FILE *file, uint64_t offset) {
#ifdef _WIN32
    int fd = _fileno(file);
    if (fd != -1) {
        if (_chsize_s(fd, offset + 1) == 0) {
            // Обрезка файла прошла успешно
        } else {
            perror("Error truncating file");
        }
    } else {
        perror("Error getting file descriptor");
    }
#else
    int fd = fileno(file);
    if (fd != -1) {
        if (ftruncate(fd, offset + 1) == -1) {
            perror("Error truncating file");
        }
    } else {
        perror("Error getting file descriptor");
    }
#endif
}
