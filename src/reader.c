#include "../inc/reader.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool readELFMagic(const uint8_t buffer[], size_t size) {
  if (buffer == NULL || size < 4)
    return false;

  if (buffer[0] == 0x7F && buffer[1] == 'E' && buffer[2] == 'L' &&
      buffer[3] == 'F')
    return true;

  return false;
}

bool readerRangeValid(const binaryReader *reader, size_t offset,
                      size_t length) {

  if (reader == NULL || reader->data == NULL)
    return false;
  if (offset > reader->size)
    return false;
  if (length > reader->size - offset)
    return false;

  return true;
}

uint8_t readerReadU8(const binaryReader *reader, size_t offset) {
  if (!readerRangeValid(reader, offset, 1))
    return 0;

  return reader->data[offset];
}

uint16_t readerReadU16(const binaryReader *reader, size_t offset) {
  if (!readerRangeValid(reader, offset, 2))
    return 0;

  if (reader->order != BYTE_ORDER_LITTLE && reader->order != BYTE_ORDER_BIG)
    return 0;

  uint16_t value = 0;

  for (int i = 0; i < 2; ++i) {
    uint16_t byte = reader->data[offset + i];
    if (reader->order == BYTE_ORDER_LITTLE)
      value |= byte << i * 8;
    else if (reader->order == BYTE_ORDER_BIG)
      value = (value << 8) | byte;
  }

  return value;
}

uint32_t readerReadU32(const binaryReader *reader, size_t offset) {
  if (!readerRangeValid(reader, offset, 4))
    return 0;

  if (reader->order != BYTE_ORDER_LITTLE && reader->order != BYTE_ORDER_BIG)
    return 0;

  uint32_t value = 0;

  for (int i = 0; i < 4; ++i) {
    uint32_t byte = reader->data[offset + i];
    if (reader->order == BYTE_ORDER_LITTLE)
      value |= byte << i * 8;
    else if (reader->order == BYTE_ORDER_BIG)
      value = (value << 8) | byte;
  }

  return value;
}

uint64_t readerReadU64(const binaryReader *reader, size_t offset) {
  if (!readerRangeValid(reader, offset, 8))
    return 0;

  if (reader->order != BYTE_ORDER_LITTLE && reader->order != BYTE_ORDER_BIG)
    return 0;

  uint64_t value = 0;

  for (int i = 0; i < 8; ++i) {
    uint64_t byte = reader->data[offset + i];
    if (reader->order == BYTE_ORDER_LITTLE)
      value |= byte << i * 8;
    else if (reader->order == BYTE_ORDER_BIG)
      value = (value << 8) | byte;
  }

  return value;
}

bool readerReadBytes(const binaryReader *reader, size_t offset,
                     void *destination, size_t length) {
  if (!readerRangeValid(reader, offset, length))
    return false;

  if (length == 0)
    return true;
  if (destination == NULL)
    return false;

  memcpy(destination, reader->data + offset, length);

  return true;
}
