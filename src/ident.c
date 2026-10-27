#include "../inc/ident.h"
#include "../inc/reader.h"

bool elfParseIdent(const binaryReader *reader, elfIdent *ident) {
  if (ident == NULL)
    return false;

  if (!readerRangeValid(reader, 0, EI_NIDENT))
    return false;
  if (!readELFMagic(reader->data, reader->size))
    return false;

  return true;
}
