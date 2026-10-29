#include "../inc/program.h"
#include "header.h"
#include "ident.h"
#include "reader.h"
#include <stddef.h>
#include <stdint.h>

bool elfParseProgramHeader(const binaryReader *reader, elfHeader *header,
                           size_t index, elfProgramHeader *result) {

  if (reader == NULL || header == NULL || result == NULL)
    return false;
  if (reader->data == NULL)
    return false;
  if (header->programCount == PN_XNUM || index >= header->programCount)
    return false;

  size_t entrySize;

  if (header->ident.elfClass)
    entrySize = ELF32_PROGRAM_SIZE;
  else if (header->ident.elfClass)
    entrySize = ELF64_HEADER_SIZE;
  else
    return false;

  if (header->programEntrySize != entrySize)
    return false;

  if (header->ident.order != BYTE_ORDER_LITTLE &&
      header->ident.order != BYTE_ORDER_BIG)
    return false;

  if (header->programOffset > reader->size)
    return false;

  size_t tableOffset = (size_t)header->programOffset;

  if (index > (SIZE_MAX - tableOffset) / entrySize)
    return false;

  size_t offset = tableOffset + index * entrySize;
  if (!readerRangeValid(reader, offset, entrySize))
    return false;

  binaryReader binary = *reader;
  binary.order = header->ident.order;

  elfProgramHeader program = {0};

  program.type = readerReadU32(&binary, offset);

  if (header->ident.elfClass == ELFCLASS32) {
    program.offset = readerReadU32(&binary, offset += 4);
    program.virtualAddr = readerReadU32(&binary, offset += 4);
    program.programAddr = readerReadU32(&binary, offset += 4);
    program.fileSize = readerReadU32(&binary, offset += 4);
    program.memSize = readerReadU32(&binary, offset += 4);
    program.flags = readerReadU32(&binary, offset += 4);
  } else if (header->ident.elfClass == ELFCLASS64) {
    program.offset = readerReadU32(&binary, offset += 4);
    program.flags = readerReadU64(&binary, offset += 4);
    program.virtualAddr = readerReadU64(&binary, offset += 8);
    program.programAddr = readerReadU64(&binary, offset += 8);
    program.fileSize = readerReadU64(&binary, offset += 8);
    program.memSize = readerReadU64(&binary, offset += 8);
  }

  *result = program;

  return true;
}
