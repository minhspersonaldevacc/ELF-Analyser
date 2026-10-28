#include "../inc/header.h"
#include "ident.h"
#include "reader.h"
#include <stddef.h>

bool elfParseHeader(const binaryReader *reader, elfHeader *header) {
  if (header == NULL)
    return false;
  elfIdent ident;

  if (!elfParseIdent(reader, &ident))
    return false;

  size_t requiredSize = 0;

  if (ident.elfClass == ELFCLASS32)
    requiredSize = ELF32_HEADER_SIZE;
  else if (ident.elfClass == ELFCLASS64)
    requiredSize = ELF64_HEADER_SIZE;

  if (!readerRangeValid(reader, 0, requiredSize))
    return false;

  binaryReader binary = *reader;
  binary.order = ident.order;

  elfHeader result = {0};

  size_t offset = 16;
  result.ident = ident;
  result.type = readerReadU16(&binary, offset);
  result.machine = readerReadU16(&binary, offset += 2);
  result.version = readerReadU32(&binary, offset += 2);

  if (ident.elfClass == ELFCLASS32) {
    result.entry = readerReadU32(&binary, offset += 4);
    result.programOffset = readerReadU32(&binary, offset += 4);
    result.sectionOffset = readerReadU32(&binary, offset += 4);
    result.flags = readerReadU32(&binary, offset += 4);
  }

  else if (ident.elfClass == ELFCLASS64) {
    result.entry = readerReadU64(&binary, offset += 4);
    result.programOffset = readerReadU64(&binary, offset += 8);
    result.sectionOffset = readerReadU64(&binary, offset += 8);
    result.flags = readerReadU32(&binary, offset += 8);
  }

  result.headerSize = readerReadU16(&binary, offset += 4);
  result.programEntrySize = readerReadU16(&binary, offset += 2);
  result.programCount = readerReadU16(&binary, offset += 2);
  result.sectionEntrySize = readerReadU16(&binary, offset += 2);
  result.sectionCount = readerReadU16(&binary, offset += 2);
  result.sectionNameIndex = readerReadU16(&binary, offset += 2);

  if (result.version != EV_CURRENT)
    return false;
  if (result.headerSize != requiredSize)
    return false;
  *header = result;

  return true;
}
