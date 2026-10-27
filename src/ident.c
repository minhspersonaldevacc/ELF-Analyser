#include "../inc/ident.h"
#include "../inc/reader.h"
#include <stdint.h>

bool elfParseIdent(const binaryReader *reader, elfIdent *ident) {
  if (ident == NULL)
    return false;

  if (!readerRangeValid(reader, 0, EI_NIDENT))
    return false;

  if (!readELFMagic(reader->data, reader->size))
    return false;

  uint8_t elfClass = readerReadU8(reader, EI_CLASS);
  // check if class is valid
  if (elfClass != ELFCLASS32 && elfClass != ELFCLASS64)
    return false;

  uint8_t elfData = readerReadU8(reader, EI_DATA);
  uint8_t elfVersion = readerReadU8(reader, EI_VERSION);
  uint8_t osAbi = readerReadU8(reader, EI_OSABI);
  uint8_t abiVersion = readerReadU8(reader, EI_ABIVERSION);

  byteOrder order;

  if (elfData == ELFDATA2LSB)
    order = BYTE_ORDER_LITTLE;
  else if (elfData == ELFDATA2MSB)
    order = BYTE_ORDER_BIG;
  else
    return false;

  if (elfVersion != EV_CURRENT)
    return false;

  elfIdent result = {
      .elfClass = elfClass,
      .order = order,
      .version = elfVersion,
      .osAbi = osAbi,
      .abiVersion = abiVersion,
  };

  *ident = result;

  return true;
}
