# Build report — GamePad Helper 1.01

Independent L2 and R2 analog scaling, compiled with real PS4 tools on 2026-11-15
under Ubuntu WSL. The maintainer has also confirmed PS4 hardware testing in some games.

## Artifact

- File: `gamepad_helper.prx`
- Size: 91,568 bytes
- SHA-256: `77241c9c54d9fa3d404dc81bfc84ee13743edb7659a85419cd35953141b8a8c7`
- Valid SELF magic, embedded x86-64 ELF, PS4 PRX type `0xfe18`.
- Compiled disassembly confirms both read hooks call both trigger scalers.
- All six L2/R2 configuration key strings are present in the packaged PRX.

## Source and dependencies

Based on GoldHEN plugin repository commit
`998fe03fe0275b59310511187ba8550ed475a003` and the user-supplied R2 scaling
modification, extended here with independent L2 settings and hook processing.
Original R2 configuration keys and defaults are preserved; L2 scaling defaults
to disabled with maximum 255 and digital threshold 0. Plugin version is 1.01.

- Official GoldHEN Plugin SDK: `bcea3c7ef01dac6d7f9f49ebf9e90fe66d86f5f7`.
- Official OpenOrbis v0.5.4, Ubuntu Clang/LLD/LLVM ar 18.1.3.
- Existing SDK/toolchain installations were reused; no downloads were needed.
- Production builds use real SDK headers and libraries; host stubs are test-only.

## Commands and results

Set `OO_PS4_TOOLCHAIN` and `GOLDHEN_SDK`, then run `make build` from the repo root.
It runs host checks, compiles the SDK with `PRINTF=1 DEBUGFLAGS=1`, then invokes
`make -C plugin_src/gamepad_helper clean` and
`make -C plugin_src/gamepad_helper DEBUG=0 PRINTF=1`.

Clang compilation, LLVM archive creation, LLD linking and OpenOrbis
`create-fself --lib=.../gamepad_helper.prx --paid 0x3800000000000011` all succeeded.
The resulting PRX was copied into `dist/`. Rebuild output goes to
`bin/plugins/prx_final/`; ELF/OELF files go to `bin/plugins/elf_final/`.

Host tests passed for both individual scalers, both enabled together, neither
enabled, exact rounding for all 256 input values across maxima 1–255,
clamping, monotonicity, optional digital bits, unrelated sample bytes,
disconnected samples, read errors, real INI parsing and range validation,
per-title overrides and compatibility with existing button remapping.
R2-only configuration behavior remains covered. L2/R2 offsets (8/9) and pad
sample size (120) are asserted. Run `make test` to repeat these checks.

Nonfatal warnings remain from the inherited parser/deadzone source and WSL
filesystem clock skew. Required objects were freshly built, and compilation,
linking and PRX packaging all ran successfully. Unrelated source was preserved.

## Limits

PS4 hardware testing has been performed in some games; compatibility with all games
has not been established.
Games using other input paths may bypass these hooks. Restart the game to reload
settings. The inherited INI parser uses `atoi`; use plain decimal integers.
Out-of-range maxima (1–255) and thresholds (0–255) retain prior settings.
Threshold 0 preserves the original digital bit for each trigger.
No PayLoader AutoRun or controller firmware/calibration changes were made.
Installation and rollback instructions are in INSTALL_PS4.md.
