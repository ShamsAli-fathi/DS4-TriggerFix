.PHONY: all test build check-env clean verify
all: build

test:
	sh plugin_src/gamepad_helper/tests/run_host_checks.sh

check-env:
	@test -n "$(OO_PS4_TOOLCHAIN)" || { echo 'Set OO_PS4_TOOLCHAIN (see README.md)'; exit 1; }
	@test -n "$(GOLDHEN_SDK)" || { echo 'Set GOLDHEN_SDK (see README.md)'; exit 1; }
	@test -x "$(OO_PS4_TOOLCHAIN)/bin/linux/create-fself"
	@test -f "$(GOLDHEN_SDK)/Makefile"
	@command -v clang >/dev/null
	@command -v ld.lld >/dev/null
	@command -v llvm-ar >/dev/null

build: check-env test
	$(MAKE) -C "$(GOLDHEN_SDK)" PRINTF=1 DEBUGFLAGS=1
	@mkdir -p bin/plugins/prx_final bin/plugins/elf_final
	$(MAKE) -C plugin_src/gamepad_helper clean
	$(MAKE) -C plugin_src/gamepad_helper DEBUG=0 PRINTF=1

verify:
	cd dist && sha256sum -c sha256.txt

clean:
	$(MAKE) -C plugin_src/gamepad_helper clean
	rm -rf bin
	rm -f common/git_ver.h plugin_src/gamepad_helper/tests/test_trigger_scaling plugin_src/gamepad_helper/tests/test_hooks_config
