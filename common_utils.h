#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <stddef.h>

int ensureBufferCapacity(char **buffer, size_t *bufferSize, size_t requiredIndex);
int appendToDynamicBuffer(char **buffer, size_t *bufferSize, size_t *bufferIndex, char c);
int splitUserLine(char *line, char **fields, int maxFields);
void trimNewline(char *s);

#endif
