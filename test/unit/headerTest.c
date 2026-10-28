#include "../../inc/header.h"
#include "../minunit/minunit.h"
#include "inc/reader.h"
#include <stdint.h>

MU_TEST(testHeader) {
  uint8_t data[64] = {0x7F,      'E',      'L',         'F',
                      2,         1,        1,           [16] = 2,
                      [18] = 62, [20] = 1, [24] = 0x40, [52] = 64};
  binaryReader reader = {
      .data = data,
      .size = sizeof(data),
      .order = BYTE_ORDER_LITTLE,
  };

  elfHeader header;

  mu_check(elfParseHeader(&reader, &header));
  mu_assert_int_eq(2, header.type);
  mu_assert_int_eq(62, header.machine);
  mu_assert_int_eq(64, header.headerSize);
  mu_check(header.entry == 0x40);
}

MU_TEST_SUITE(headerSuite) { MU_RUN_TEST(testHeader); }

int main(void) {
  MU_RUN_SUITE(headerSuite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
