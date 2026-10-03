# Host chorus regression checks

From the firmware directory, with a C++17 compiler:

```sh
g++ -std=c++17 -Itests/host/stubs -Iinclude tests/host/chorus_test.cpp src/effects/chorus.cpp -o chorus_test
./chorus_test
```

The test compiles the production chorus implementation against a minimal audio
transport stub. It checks exact disabled pass-through, received-block release,
dry/wet filter isolation, wet-source selection, and delay limits over parameter
combinations including values outside the UI range. It does not emulate Teensy
interrupts, the codec, filter coefficients, or real-time scheduling.
