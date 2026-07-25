CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -Werror -Isrc -Ithird_party -Itests -g
SRCS    := $(wildcard src/rommpl/*.c)
TEST_SRCS := $(wildcard tests/test_*.c)
TEST_BINS := $(patsubst tests/%.c,build/%,$(TEST_SRCS))

build:
	mkdir -p build

build/%: tests/%.c $(SRCS) | build
	$(CC) $(CFLAGS) -o $@ $< $(SRCS)

.PHONY: test
test: $(TEST_BINS)
	@fail=0; for t in $(TEST_BINS); do echo "== $$t =="; ./$$t || fail=1; done; \
	 if [ $$fail -ne 0 ]; then echo "TESTS FAILED"; exit 1; fi; echo "ALL PASS"

.PHONY: itest
itest: build/itest_live
	./build/itest_live

build/itest_live: tests/itest_live.c $(SRCS) | build
	$(CC) $(CFLAGS) -o $@ $< $(SRCS)

.PHONY: clean
clean:
	rm -rf build
