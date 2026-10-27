#ifndef READER_H
#define READER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum { BYTE_ORDER_LITTLE, BYTE_ORDER_BIG } byteOrder;

typedef struct {
  const uint8_t *data;
  size_t size;
  byteOrder order;
} binaryReader;

bool readELFMagic(const uint8_t buffer[], size_t size);
bool readerRangeValid(const binaryReader *reader, size_t offset, size_t length);
uint8_t readerReadU8(const binaryReader *reader, size_t offset);
uint16_t readerReadU16(const binaryReader *reader, size_t offset);
uint32_t readerReadU32(const binaryReader *reader, size_t offset);
uint64_t readerReadU64(const binaryReader *reader, size_t offset);

bool readerReadBytes(const binaryReader *reader, size_t offset,
                     void *destination, size_t length);

#endif // !READER_H
