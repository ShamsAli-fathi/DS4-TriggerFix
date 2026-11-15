# GoldHEN GamePad Helper — L2/R2 scaling

PS4 plugin with independent analog scaling for both DualShock 4 triggers.
For either trigger, set its measured maximum to compensate for reduced travel:

```text
output = min(255, round(input × 255 / measured_maximum))
```

With maximum 102: **0 → 0**, **51 → 128**, **102 → 255**. Applies to both `scePadRead`
and `scePadReadState`. Each enabled scaler changes only its own analog trigger byte by default. The supplied
configuration disables the helper's optional deadzone and remapping features.

**Compiled, host-tested, and tested on PS4 hardware in some games.** Games that bypass
these hooks may be unaffected. No controller firmware or calibration is changed.

## Install

The compiled plugin, configuration example, build report and checksums are in
[`dist/`](dist/). Follow [INSTALL_PS4.md](dist/INSTALL_PS4.md) to load it for one
game through GoldHEN's plugin configuration. Do not enable PayLoader AutoRun.

Configure `/data/GoldHEN/gamepad.ini` under your game's actual Title ID:

| Setting | L2 | R2 |
| --- | --- | --- |
| Enable (0 or 1) | `enableL2Scaling` | `enableR2Scaling` |
| Measured maximum (1–255) | `L2InputMax` | `R2InputMax` |
| Digital threshold (0–255) | `L2DigitalThreshold` | `R2DigitalThreshold` |

Both scalers are disabled by default. Built-in maxima are L2=255 and R2=102.
Threshold 0 preserves each trigger's original digital bit; 1–255 synthesizes
that bit from its scaled analog value before existing button remapping.
Out-of-range numbers retain the previous setting. Use plain decimal integers:
the inherited parser uses `atoi`. `[default]` loads before Title ID overrides.

The example enables R2 at 102 and leaves your healthy L2 unchanged. To use L2
scaling, enable it and set its measured maximum. Existing R2-only configurations
continue to work. Restart the game after changing settings.

To roll back, close the game, remove/comment its plugin entry from
`/data/GoldHEN/plugins.ini`, restore your backups and restart the game.

## Build on Ubuntu / WSL

Requires Make, a host C compiler, Clang, LLD, llvm-ar, the complete
[OpenOrbis toolchain release](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain/releases/tag/v0.5.4)
and the [GoldHEN Plugin SDK source](https://github.com/GoldHEN/GoldHEN_Plugins_SDK).
Reuse existing installations. Toolchains and dependencies are not bundled.

If compiler tools are missing:

```bash
sudo apt update
sudo apt install build-essential clang lld llvm
```

Extract OpenOrbis to a directory of your choice. If the SDK is missing:

```bash
mkdir -p deps
git clone https://github.com/GoldHEN/GoldHEN_Plugins_SDK.git deps/GoldHEN_Plugins_SDK
git -C deps/GoldHEN_Plugins_SDK checkout bcea3c7ef01dac6d7f9f49ebf9e90fe66d86f5f7
```

Set actual absolute paths without spaces, then build:

```bash
export OO_PS4_TOOLCHAIN=/absolute/path/to/OpenOrbis/PS4Toolchain
export GOLDHEN_SDK=/absolute/path/to/GoldHEN_Plugins_SDK
make build
```

This runs the host checks, rebuilds the SDK at `GOLDHEN_SDK`, then compiles,
links and packages this plugin through its upstream Makefile. Output:
`bin/plugins/prx_final/gamepad_helper.prx`. The included release in `dist/`
is preserved. Git metadata falls back to `uncommitted` before the first commit.

- `make test`: scaling, both hooks, input preservation and INI parser checks.
- `make verify`: verify the included release checksums.
- `make clean`: remove generated files, preserving `dist/`.

Host tests use SDK stubs only for simulation; production builds use the real SDK.

## Contents and license

`plugin_src/gamepad_helper/` contains source, headers, tests and its Makefile.
`common/` contains the required shared header. `dist/` contains the release.
Generated objects and local dependency checkouts are ignored by Git.

Based on [GoldHEN GamePad Helper](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/tree/main/plugin_src/gamepad_helper)
by jocover and the GoldHEN contributors, with the user-supplied R2 modification
and independent L2 scaling added to this fork.
See [BUILD_REPORT.md](dist/BUILD_REPORT.md) for source revisions and validation.
Upstream [GPL-3.0 license](LICENSE) and source attribution are retained.
