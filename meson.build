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

test('reader', reader_test)
