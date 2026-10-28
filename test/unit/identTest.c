#include "../../inc/ident.h"
#include "../minunit/minunit.h"
#include "inc/reader.h"

MU_TEST(testValidELF64LittleEndian) {
  const uint8_t buffer[16] = {0x7F, 'E', 'L', 'F', 2, 1, 1, 0,
                              0,    0,   0,   0,   0, 0, 0, 0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  elfIdent ident;

  mu_assert(elfParseIdent(&reader, &ident), "identification should work here");

  mu_assert_int_eq(ELFCLASS64, ident.elfClass);
  mu_assert_int_eq(BYTE_ORDER_LITTLE, ident.order);
  mu_assert_int_eq(0, ident.osAbi);
  mu_assert_int_eq(0, ident.abiVersion);
}

int main(void) {
  MU_RUN_TEST(testValidELF64LittleEndian);
  MU_REPORT();
  return MU_EXIT_CODE;
}
