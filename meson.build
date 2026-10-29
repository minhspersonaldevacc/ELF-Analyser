project(
  'elfAnalyser',
  'c',
  meson_version : '>= 0.60.0',
  version : '0.1',
  default_options : [
    'c_std=c17',
    'warning_level=3',
  ],
)

reader = static_library(
  'reader',
  'src/reader.c',
  include_directories : include_directories('inc'),
)

reader_test = executable(
  'reader-test',
  'test/unit/readerTest.c',
  c_args : ['-D_POSIX_C_SOURCE=200809L'],
  link_with : reader,
)

ident = static_library(
  'ident',
  'src/ident.c',
  include_directories : include_directories('inc'),
)

ident_test = executable(
  'ident-test',
  'test/unit/identTest.c',
  c_args : ['-D_POSIX_C_SOURCE=200809L'],
  link_with : [ident, reader],
)

header = static_library(
  'header',
  'src/header.c',
  include_directories : include_directories('inc'),
)

header_test = executable(
  'header-test',
  'test/unit/headerTest.c',
  c_args : ['-D_POSIX_C_SOURCE=200809L'],
  link_with : [header, ident, reader],
)

program = static_library(
  'program',
  'src/program.c',
  include_directories : include_directories('inc'),
)

program_test = executable(
  'program-test',
  'test/unit/programTest.c',
  c_args : ['-D_POSIX_C_SOURCE=200809L'],
  link_with : [program, reader],
)

test('reader', reader_test)
test('ident', ident_test)
test('header', header_test)
test('program', program_test)
