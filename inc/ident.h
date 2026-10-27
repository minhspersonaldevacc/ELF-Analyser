#ifndef IDENT_H
#define IDENT_H
#include "reader.h"
#include <stdint.h>

enum {
  EI_MAG0 = 0,
  EI_MAG1 = 1,
  EI_MAG2 = 2,
  EI_MAG3 = 3,
  EI_CLASS = 4,
  EI_DATA = 5,
  EI_VERSION = 6,
  EI_OSABI = 7,
  EI_ABIVERSION = 8,
  EI_PAD = 9,
  EI_NIDENT = 16
};

enum { ELFCLASSNONE = 0, ELFCLASS32 = 1, ELFCLASS64 = 2 };

enum { ELFDATANONE = 0, ELFDATA2LSB = 1, ELFDATA2MSB = 2 };

enum { EV_NONE = 0, EV_CURRENT = 1 };

typedef struct {
  uint8_t elfClass;
  byteOrder order;
  uint8_t version;
  uint8_t osAbi;
  uint8_t abiVersion;
} elfIdent;

bool elfParseIdent(const binaryReader *reader, elfIdent *ident);

#endif
