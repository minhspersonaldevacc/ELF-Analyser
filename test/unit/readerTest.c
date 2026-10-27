#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../../inc/reader.h"
#include "../minunit/minunit.h"

MU_TEST(testValidELFMagic) {
  const uint8_t buffer[] = {0x7F, 'E', 'L', 'F'};

  mu_check(readELFMagic(buffer, sizeof(buffer)));
}

MU_TEST(testInvalidELFMagic) {
  const uint8_t invalidFirst[] = {0x00, 'E', 'L', 'F'};
  const uint8_t invalidMiddle[] = {0x7F, 'E', 'X', 'F'};
  const uint8_t invalidLast[] = {0x7F, 'E', 'L', 'X'};

  mu_check(!readELFMagic(invalidFirst, sizeof(invalidFirst)));
  mu_check(!readELFMagic(invalidMiddle, sizeof(invalidMiddle)));
  mu_check(!readELFMagic(invalidLast, sizeof(invalidLast)));
}

MU_TEST(testTruncatedELFMagic) {
  const uint8_t buffer[] = {0x7F, 'E', 'L', 'F'};

  for (size_t size = 0; size < 4; size++)
    mu_check(!readELFMagic(buffer, size));
}

MU_TEST(testNullELFMagic) {
  mu_check(!readELFMagic(NULL, 0));
  mu_check(!readELFMagic(NULL, 4));
}

MU_TEST(testELFMagicExtraBytes) {
  const uint8_t buffer[] = {0x7F, 'E', 'L', 'F', 0x02, 0x01, 0x00};

  mu_check(readELFMagic(buffer, sizeof(buffer)));
}

MU_TEST(testValidRanges) {
  const uint8_t buffer[8] = {0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerRangeValid(&reader, 0, 8));
  mu_check(readerRangeValid(&reader, 0, 1));
  mu_check(readerRangeValid(&reader, 3, 4));
  mu_check(readerRangeValid(&reader, 6, 2));
  mu_check(readerRangeValid(&reader, 7, 1));
}

MU_TEST(testInvalidRanges) {
  const uint8_t buffer[8] = {0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(!readerRangeValid(&reader, 7, 2));
  mu_check(!readerRangeValid(&reader, 8, 1));
  mu_check(!readerRangeValid(&reader, 9, 1));
  mu_check(!readerRangeValid(&reader, 0, 9));
}

MU_TEST(testZeroLengthRanges) {
  const uint8_t buffer[8] = {0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerRangeValid(&reader, 0, 0));
  mu_check(readerRangeValid(&reader, 4, 0));
  mu_check(readerRangeValid(&reader, 8, 0));
  mu_check(!readerRangeValid(&reader, 9, 0));
}

MU_TEST(testRangeNullPointers) {
  const uint8_t buffer[4] = {0};

  binaryReader reader = {.data = NULL, .size = 4, .order = BYTE_ORDER_LITTLE};

  mu_check(!readerRangeValid(NULL, 0, 1));
  mu_check(!readerRangeValid(&reader, 0, 1));

  reader.data = buffer;
  mu_check(readerRangeValid(&reader, 0, 4));
}

MU_TEST(testRangeOverflow) {
  const uint8_t buffer[8] = {0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(!readerRangeValid(&reader, SIZE_MAX, 1));
  mu_check(!readerRangeValid(&reader, 1, SIZE_MAX));
  mu_check(!readerRangeValid(&reader, SIZE_MAX, SIZE_MAX));
}

MU_TEST(testReadU8) {
  const uint8_t buffer[] = {0x12, 0x34, 0x56, 0x78};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_assert_int_eq(0x12, readerReadU8(&reader, 0));
  mu_assert_int_eq(0x34, readerReadU8(&reader, 1));
  mu_assert_int_eq(0x56, readerReadU8(&reader, 2));
  mu_assert_int_eq(0x78, readerReadU8(&reader, 3));
}

MU_TEST(testReadU8ZeroAndMax) {
  const uint8_t buffer[] = {0x00, 0xFF};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_BIG};

  mu_assert_int_eq(0, readerReadU8(&reader, 0));
  mu_assert_int_eq(255, readerReadU8(&reader, 1));
}

MU_TEST(testReadU8Invalid) {
  const uint8_t buffer[] = {0x12};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_assert_int_eq(0, readerReadU8(&reader, 1));
  mu_assert_int_eq(0, readerReadU8(&reader, SIZE_MAX));
  mu_assert_int_eq(0, readerReadU8(NULL, 0));
}

MU_TEST(testReadU16LittleEndian) {
  const uint8_t buffer[] = {0x34, 0x12, 0x78, 0x56};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_assert_int_eq(0x1234, readerReadU16(&reader, 0));
  mu_assert_int_eq(0x5678, readerReadU16(&reader, 2));
}

MU_TEST(testReadU16BigEndian) {
  const uint8_t buffer[] = {0x12, 0x34, 0x56, 0x78};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_BIG};

  mu_assert_int_eq(0x1234, readerReadU16(&reader, 0));
  mu_assert_int_eq(0x5678, readerReadU16(&reader, 2));
}

MU_TEST(testReadU16Max) {
  const uint8_t buffer[] = {0xFF, 0xFF};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadU16(&reader, 0) == UINT16_MAX);

  reader.order = BYTE_ORDER_BIG;
  mu_check(readerReadU16(&reader, 0) == UINT16_MAX);
}

MU_TEST(testReadU16Invalid) {
  const uint8_t buffer[] = {0x34, 0x12};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_assert_int_eq(0, readerReadU16(&reader, 1));
  mu_assert_int_eq(0, readerReadU16(NULL, 0));

  reader.order = (byteOrder)99;
  mu_assert_int_eq(0, readerReadU16(&reader, 0));
}

MU_TEST(testReadU32LittleEndian) {
  const uint8_t buffer[] = {0x78, 0x56, 0x34, 0x12};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadU32(&reader, 0) == UINT32_C(0x12345678));
}

MU_TEST(testReadU32BigEndian) {
  const uint8_t buffer[] = {0x12, 0x34, 0x56, 0x78};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_BIG};

  mu_check(readerReadU32(&reader, 0) == UINT32_C(0x12345678));
}

MU_TEST(testReadU32Max) {
  const uint8_t buffer[] = {0xFF, 0xFF, 0xFF, 0xFF};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadU32(&reader, 0) == UINT32_MAX);

  reader.order = BYTE_ORDER_BIG;
  mu_check(readerReadU32(&reader, 0) == UINT32_MAX);
}

MU_TEST(testReadU32Invalid) {
  const uint8_t buffer[] = {0x78, 0x56, 0x34, 0x12};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadU32(&reader, 1) == 0);
  mu_check(readerReadU32(NULL, 0) == 0);

  reader.order = (byteOrder)99;
  mu_check(readerReadU32(&reader, 0) == 0);
}

MU_TEST(testReadU64LittleEndian) {
  const uint8_t buffer[] = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadU64(&reader, 0) == UINT64_C(0x0102030405060708));
}

MU_TEST(testReadU64BigEndian) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_BIG};

  mu_check(readerReadU64(&reader, 0) == UINT64_C(0x0102030405060708));
}

MU_TEST(testReadU64Max) {
  const uint8_t buffer[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadU64(&reader, 0) == UINT64_MAX);

  reader.order = BYTE_ORDER_BIG;
  mu_check(readerReadU64(&reader, 0) == UINT64_MAX);
}

MU_TEST(testReadU64Invalid) {
  const uint8_t buffer[8] = {0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadU64(&reader, 1) == 0);
  mu_check(readerReadU64(NULL, 0) == 0);

  reader.order = (byteOrder)99;
  mu_check(readerReadU64(&reader, 0) == 0);
}

MU_TEST(testReadBytes) {
  const uint8_t buffer[] = {0x10, 0x20, 0x30, 0x40, 0x50};

  const uint8_t expected[] = {0x20, 0x30, 0x40};
  uint8_t destination[3] = {0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadBytes(&reader, 1, destination, 3));
  mu_check(memcmp(destination, expected, 3) == 0);
}

MU_TEST(testReadBytesFullBuffer) {
  const uint8_t buffer[] = {0x11, 0x22, 0x33, 0x44};
  uint8_t destination[4] = {0};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadBytes(&reader, 0, destination, 4));
  mu_check(memcmp(destination, buffer, 4) == 0);
}

MU_TEST(testReadBytesInvalidRange) {
  const uint8_t buffer[] = {0x10, 0x20};
  uint8_t destination[] = {0xAA, 0xBB, 0xCC};

  const uint8_t expected[] = {0xAA, 0xBB, 0xCC};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(!readerReadBytes(&reader, 1, destination, 3));
  mu_check(memcmp(destination, expected, 3) == 0);
}

MU_TEST(testReadBytesNullDestination) {
  const uint8_t buffer[] = {0x10, 0x20};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(!readerReadBytes(&reader, 0, NULL, 2));
}

MU_TEST(testReadBytesZeroLength) {
  const uint8_t buffer[] = {0x10, 0x20};

  binaryReader reader = {
      .data = buffer, .size = sizeof(buffer), .order = BYTE_ORDER_LITTLE};

  mu_check(readerReadBytes(&reader, 0, NULL, 0));
  mu_check(readerReadBytes(&reader, 2, NULL, 0));
  mu_check(!readerReadBytes(&reader, 3, NULL, 0));
}

MU_TEST_SUITE(readerSuite) {
  MU_RUN_TEST(testValidELFMagic);
  MU_RUN_TEST(testInvalidELFMagic);
  MU_RUN_TEST(testTruncatedELFMagic);
  MU_RUN_TEST(testNullELFMagic);
  MU_RUN_TEST(testELFMagicExtraBytes);

  MU_RUN_TEST(testValidRanges);
  MU_RUN_TEST(testInvalidRanges);
  MU_RUN_TEST(testZeroLengthRanges);
  MU_RUN_TEST(testRangeNullPointers);
  MU_RUN_TEST(testRangeOverflow);

  MU_RUN_TEST(testReadU8);
  MU_RUN_TEST(testReadU8ZeroAndMax);
  MU_RUN_TEST(testReadU8Invalid);

  MU_RUN_TEST(testReadU16LittleEndian);
  MU_RUN_TEST(testReadU16BigEndian);
  MU_RUN_TEST(testReadU16Max);
  MU_RUN_TEST(testReadU16Invalid);

  MU_RUN_TEST(testReadU32LittleEndian);
  MU_RUN_TEST(testReadU32BigEndian);
  MU_RUN_TEST(testReadU32Max);
  MU_RUN_TEST(testReadU32Invalid);

  MU_RUN_TEST(testReadU64LittleEndian);
  MU_RUN_TEST(testReadU64BigEndian);
  MU_RUN_TEST(testReadU64Max);
  MU_RUN_TEST(testReadU64Invalid);

  MU_RUN_TEST(testReadBytes);
  MU_RUN_TEST(testReadBytesFullBuffer);
  MU_RUN_TEST(testReadBytesInvalidRange);
  MU_RUN_TEST(testReadBytesNullDestination);
  MU_RUN_TEST(testReadBytesZeroLength);
}

int main(void) {
  MU_RUN_SUITE(readerSuite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
