#ifndef PROGRAM_H
#define PROGRAM_H

#include "header.h"
#include <stddef.h>
#include <stdint.h>

enum { ELF32_PROGRAM_SIZE = 32, ELF64_PROGRAM_SIZE = 56, PN_XNUM = 0xFFFF };

typedef struct {
  uint32_t type;
  uint32_t flags;
  uint64_t offset;

  uint64_t virtualAddr;
  uint64_t programAddr;

  uint64_t fileSize;
  uint64_t memSize;
  uint64_t align;
} elfProgramHeader;

bool elfParseProgramHeader(const binaryReader *reader, elfHeader *header,
                           size_t index, elfProgramHeader *result);

#endif
