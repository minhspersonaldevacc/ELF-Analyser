# Current APIs

Public declarations live in [`inc/`](../inc/). All offsets and lengths are in
bytes. The caller supplies and owns the input buffer; these APIs do not open
files or allocate memory. Keep the buffer valid while reading it.

## Binary reader — `reader.h`

```c
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
```

| Function | Current behavior |
| --- | --- |
| `readELFMagic` | Returns whether the first four bytes are `0x7F, 'E', 'L', 'F'`. Returns `false` for a null buffer or fewer than four bytes. |
| `readerRangeValid` | Checks that the reader and its data are non-null and the requested range fits, without overflowing. A zero-length range is valid through `offset == size`. |
| `readerReadU8` | Reads one unsigned byte; returns `0` for an invalid range. Ignores byte order. |
| `readerReadU16` | Reads a 16-bit unsigned integer using `reader->order`. |
| `readerReadU32` | Reads a 32-bit unsigned integer using `reader->order`. |
| `readerReadU64` | Reads a 64-bit unsigned integer using `reader->order`. |
| `readerReadBytes` | Copies a valid range into the caller's destination and returns `true`. Returns `false` for an invalid range or a null destination with nonzero length. A valid zero-length copy allows a null destination. |

The multi-byte integer readers return `0` for an invalid range or byte order.
Since zero is also a valid value, check `readerRangeValid` and the byte order
separately when errors must be distinguished. For `readerReadBytes`, provide
enough destination space and ensure source and destination do not overlap.

## ELF identification — `ident.h`

```c
typedef struct {
    uint8_t elfClass;
    byteOrder order;
    uint8_t version;
    uint8_t osAbi;
    uint8_t abiVersion;
} elfIdent;

bool elfParseIdent(const binaryReader *reader, elfIdent *ident);
```

Parses the first 16 bytes. Returns `true` only when the magic, class, data
encoding and identification version are supported. Returns `false` for invalid
input or a null output pointer, leaving the output unchanged. OS ABI and ABI
version are copied without validation. The reader's initial byte order is ignored.

Public constants:

| Constants | Values |
| --- | --- |
| `EI_MAG0`, `EI_MAG1`, `EI_MAG2`, `EI_MAG3` | `0`, `1`, `2`, `3` |
| `EI_CLASS`, `EI_DATA`, `EI_VERSION`, `EI_OSABI`, `EI_ABIVERSION`, `EI_PAD` | `4`, `5`, `6`, `7`, `8`, `9` |
| `EI_NIDENT` | `16` |
| `ELFCLASSNONE`, `ELFCLASS32`, `ELFCLASS64` | `0`, `1`, `2` |
| `ELFDATANONE`, `ELFDATA2LSB`, `ELFDATA2MSB` | `0`, `1`, `2` |
| `EV_NONE`, `EV_CURRENT` | `0`, `1` |

Only ELF32/ELF64, little/big endian, and `EV_CURRENT` are accepted.

## ELF file header — `header.h`

```c
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
```

Parses identification and the ELF32 or ELF64 file header, using the byte order
from identification. ELF32 addresses and offsets are widened to `uint64_t`.
`entry` is the entry-point address; `programOffset` and `sectionOffset` locate
the corresponding tables. The remaining size, count and index fields mirror
the ELF file header.

Returns `true` on success. Returns `false` for invalid identification, a truncated
header, a null output pointer, a version other than `EV_CURRENT`, or a header
size other than `ELF32_HEADER_SIZE` (`52`) or `ELF64_HEADER_SIZE` (`64`), as
appropriate. Output is unchanged on failure.

This function does not validate machine/type values or table contents and ranges,
and does not resolve extended section counts or indices.

## Program header — `program.h` (in progress)

```c
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
```

Reads one program-header table entry at a zero-based `index`, using table
metadata and byte order from `header`. Supply the full file buffer and a
previously parsed file header. `offset` is the segment's file offset,
`virtualAddr` its virtual address, and `programAddr` corresponds to ELF's physical
address field (`p_paddr`). `fileSize`, `memSize` and `align` represent segment
sizes and alignment.

Returns `false` for null pointers/data, an out-of-range index, extended program
counts (`PN_XNUM`), an entry-size mismatch, an invalid byte order, offset
overflow, or an entry outside the buffer. Output is unchanged on failure.
The `header` parameter is not modified by the current implementation.

Public constants: `ELF32_PROGRAM_SIZE = 32`, `ELF64_PROGRAM_SIZE = 56`,
and `PN_XNUM = 0xFFFF`.

Current limitations:

- The class-selection check treats any nonzero class as ELF32 when checking
  entry size. Standard ELF64 entries are therefore rejected, and invalid
  nonzero classes are not reliably rejected.
- The ELF64 decoding branch reads `offset` and `flags` with incorrect widths
  and positions.
- `align` is not read and remains zero, including for ELF32 entries.
- Segment ranges, alignment constraints and the full table are not validated.

Program-header parsing currently has a basic ELF32 unit test; it is incomplete.

## Example: parse an in-memory ELF header

```c
#include "header.h"

bool inspectHeader(const uint8_t *data, size_t size, elfHeader *header) {
    binaryReader reader = {
        .data = data,
        .size = size,
        .order = BYTE_ORDER_LITTLE,
    };
    return elfParseHeader(&reader, header);
}
```

The parser determines the actual byte order from the ELF identification bytes.
