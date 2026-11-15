# Install L2/R2 scaling for one PS4 game

This is a custom build. Actual PS4/controller testing is still required.

1. Close the target game. Back up `/data/GoldHEN/plugins/gamepad_helper.prx`,
   `/data/GoldHEN/plugins.ini`, and `/data/GoldHEN/gamepad.ini` if present.
2. Copy the supplied `gamepad_helper.prx` to `/data/GoldHEN/plugins/gamepad_helper.prx`
   using your existing PS4 file transfer method.
3. Enable GoldHEN's plugin loading option. Do not enable PayLoader AutoRun.
4. Merge this into `/data/GoldHEN/plugins.ini`, replacing CUSA12345 with the
   actual Title ID of your installed game:

   ```ini
   [CUSA12345]
   /data/GoldHEN/plugins/gamepad_helper.prx
   ```

   Use a single entry under this title only. Remove any duplicate entry for this
   plugin under `[default]`. Preserve other plugins and existing settings.
   The bare path syntax is used by the official repository's release workflow;
   its loader also accepts `=true` and uses `=false` to disable an entry.
5. Merge `gamepad.ini.example` into `/data/GoldHEN/gamepad.ini`, substituting the
   same Title ID. The example enables R2 scaling only for that title with
   `R2InputMax=102` and `R2DigitalThreshold=0`. It explicitly disables the
   helper's default stick deadzone and optional remapping for that title, and
   keeps normal vibration. L2 scaling is disabled because your L2 already reaches
   255. To scale a damaged L2, set `enableL2Scaling=1`, set `L2InputMax` to its
   measured maximum (1–255), and initially use `L2DigitalThreshold=0`. If you already use
   helper features, retain your desired settings instead.
6. Start the game. Verify gradual analog response: input 0 → 0, 51 → 128,
   102 → 255; higher input saturates at 255. L2 should still reach 255.
   Also check sticks, buttons, touchpad, motion and vibration in your game.

Each enabled trigger uses `min(255, round(input * 255 / InputMax))` on connected
samples from `scePadRead` and `scePadReadState`, with its independent maximum.
Both can be enabled together. Threshold 0 preserves that trigger's digital bit.
Only if a game needs a synthesized digital press, try `R2DigitalThreshold=30`
(range 1–255, in scaled units). Use `L2DigitalThreshold` for L2; each setting changes only its own trigger bit.
`L2InputMax` and `R2InputMax` accept 1–255; out-of-range values retain the previous setting.
Use plain decimal integers: the inherited parser uses `atoi`, not strict
numeric validation. `[default]` loads first and the game section overrides it.
Restart the game after configuration changes. A missing INI leaves both scalers off.
Games using other input paths may bypass these hooks. This does not change
controller firmware, calibration memory, or home-screen input.

# Rollback

Close the game, comment out the plugin path using `;` (or remove it) in that
Title ID section of `/data/GoldHEN/plugins.ini`. Restore the backed-up PRX and
INI files if desired, then restart the game. Setting `enableR2Scaling=0` also
disables R2 scaling. Set `enableL2Scaling=0` to disable L2 scaling as well;
the helper and its other configured features remain active.

# Official references

- [GoldHEN plugin repository](https://github.com/GoldHEN/GoldHEN_Plugins_Repository)
- [Loader and configuration handling](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/blob/main/plugin_src/plugin_loader/source/main.c)
- [Official release configuration generation](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/tree/main/.github/workflows)
