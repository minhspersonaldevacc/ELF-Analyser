MESON ?= meson

.PHONY: test test-asan test-asan-ubsan test-ubsan

test:
	@if test -f build/build.ninja; then \
	  $(MESON) configure build -Db_sanitize=none; \
	else \
	  $(MESON) setup build -Db_sanitize=none; \
	fi
	$(MESON) test -C build --print-errorlogs

# ASan enables LeakSanitizer by default on supported Linux systems.
test-asan:
	@if test -f build-asan/build.ninja; then \
	  $(MESON) configure build-asan -Db_sanitize=address; \
	else \
	  $(MESON) setup build-asan -Db_sanitize=address; \
	fi
	ASAN_OPTIONS=detect_leaks=1 $(MESON) test -C build-asan --print-errorlogs

# Use this when LeakSanitizer cannot run in the current environment (for
# example, when ptrace is restricted). ASan and UBSan remain enabled.
test-asan-ubsan:
	@if test -f build-asan-ubsan/build.ninja; then \
	  $(MESON) configure build-asan-ubsan -Db_sanitize=address,undefined; \
	else \
	  $(MESON) setup build-asan-ubsan -Db_sanitize=address,undefined; \
	fi
	ASAN_OPTIONS=detect_leaks=0 $(MESON) test -C build-asan-ubsan --print-errorlogs

test-ubsan:
	@if test -f build-ubsan/build.ninja; then \
	  $(MESON) configure build-ubsan -Db_sanitize=undefined; \
	else \
	  $(MESON) setup build-ubsan -Db_sanitize=undefined; \
	fi
	$(MESON) test -C build-ubsan --print-errorlogs
