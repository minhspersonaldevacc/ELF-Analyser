#ifndef HEADER_H
#define HEADER_H
#include "ident.h"
#include <stdint.h>

enum { ELF32_HEADER_SIZE = 52, ELF64_HEADER_SIZE = 64 };

typedef struct {
  elfIdent ident;
  uint16_t type;
  uint16_t machine;
  uint32_t version;

  uint64_t entry;
  uint64_t programOffset;
  uint64_t sectionOffset;

  uint32_t flags;

  uint16_t headerSize;
  uint16_t programEntrySize;
  uint16_t programCount;
  uint16_t sectionEntrySize;
  uint16_t sectionCount;
  uint16_t sectionNameIndex;
} elfHeader;

bool elfParseHeader(const binaryReader *reader, elfHeader *header);

#endif
