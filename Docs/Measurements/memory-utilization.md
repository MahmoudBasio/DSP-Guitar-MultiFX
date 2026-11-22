# Firmware memory utilization — experimental build

**Status:** Historical values reported in the associated research manuscript. **Not independently recompiled or revalidated for the current `main` revision.**

## Reported values

| Effect enabled | Flash | RAM1 | RAM2 |
|---|---:|---:|---:|
| Delay | 130.0 KB | 187.6 KB | 89.30 KB |
| Reverb | 130.0 KB | 187.6 KB | 89.30 KB |
| Chorus | 130.0 KB | 187.6 KB | 89.30 KB |

These are the **complete compiled firmware footprint** values from the original paper's Table 2, not incremental per-effect memory requirements. The identical values are consistent with statically allocated audio objects and buffers regardless of which effect is enabled.

The original manuscript quotes Teensy 4.1 capacities of 8 MB flash, 512 KB RAM1 and 512 KB RAM2, corresponding to approximately **1.6% flash, 36.6% RAM1 and 17.4% RAM2** using the reported rounded figures.

## Reproducibility and limitations

- **Source:** Research manuscript, *The Design and Implementation of an Open-Source Real-time Embedded DSP Guitar Multi-Effects Platform*, Table 2.
- **Firmware revision / commit:** Not recorded alongside these figures; do **not** attribute the measurements to the current modular firmware without rebuilding.
- **Build toolchain, compiler optimization, library versions, and full build log:** Not preserved with this record.
- **Run-time AudioMemory pool use is different from compiled Flash/RAM footprint.** These measurements do not establish peak audio block allocation or memory safety.
- To reproduce: check out the exact test firmware, record the compiler/linker memory report and build flags, log the commit SHA and Teensy/Arduino/PlatformIO versions, then repeat for the desired firmware configurations. Compare compiled RAM1/RAM2 totals and separately log `AudioMemoryUsageMax()` where relevant.

For the six-page research manuscript, these identical rows may be condensed into a paragraph, retaining the values and explaining static allocation.
