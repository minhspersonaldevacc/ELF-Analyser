#include "../../inc/program.h"
#include "../minunit/minunit.h"
#include <stdint.h>

MU_TEST(testProgramHeader) {
  /* One little-endian ELF32 PT_LOAD entry. */
  uint8_t data[ELF32_PROGRAM_SIZE] = {
      [0] = 1,    /* type */
      [5] = 0x10, /* file offset: 0x1000 */
      [9] = 0x20, /* virtual address: 0x2000 */
      [13] = 0x20, /* physical address: 0x2000 */
      [16] = 0x40, /* file size */
      [20] = 0x80, /* memory size */
      [24] = 5,    /* flags: read + execute */
  };
  binaryReader reader = {
      .data = data,
      .size = sizeof(data),
      .order = BYTE_ORDER_LITTLE,
  };
  elfHeader header = {
      .ident = {.elfClass = ELFCLASS32, .order = BYTE_ORDER_LITTLE},
      .programEntrySize = ELF32_PROGRAM_SIZE,
      .programCount = 1,
  };
  elfProgramHeader program;

  mu_check(elfParseProgramHeader(&reader, &header, 0, &program));
  mu_check(program.type == 1);
  mu_check(program.offset == 0x1000);
  mu_check(program.virtualAddr == 0x2000);
  mu_check(program.programAddr == 0x2000);
  mu_check(program.fileSize == 0x40);
  mu_check(program.memSize == 0x80);
  mu_check(program.flags == 5);
}

MU_TEST_SUITE(programSuite) { MU_RUN_TEST(testProgramHeader); }

int main(void) {
  MU_RUN_SUITE(programSuite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
