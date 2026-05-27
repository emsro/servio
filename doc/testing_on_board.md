# Running ftest on a Physical Board

## Overview

```
STM32H503 board
   proto4_fw_ftest.elf running
   ↕ COBS/UART (Black Magic Probe virtual COM)
Host: asrtio serial runner
   → _test/yellow.proto4/fw/proto4_fw_ftest/res.json
```

---

## Step 1 — Build firmware and host runner

```bash
make build_h5     # firmware: _build/stm32h5/src/brd/proto4/proto4_fw_ftest.elf
make build_host   # asrtio:   _build/host/_deps/asrt-build/asrtio/asrtio
```

**Note:** `asrtio` is not yet built as part of `host_debug` — `CMakeLists.txt`
currently sets `ASRT_LIBRARIES asrtl;asrtr;asrtlpp;asrtrpp`, which excludes
`asrtio`. To fix:
1. Add `asrtio` to the `ASRT_LIBRARIES` option in the CPMAddPackage call
2. Add `libuv` as a host dependency (asrtio links against it)

---

## Step 2 — Flash via Black Magic Probe

The BMP exposes two virtual serial ports on macOS. The first (lower number) is
the GDB server; the second is the UART passthrough.

```bash
# find BMP ports
ls /dev/cu.usbmodem*
# e.g.: /dev/cu.usbmodemXXXX1 (GDB)  /dev/cu.usbmodemXXXX3 (UART)

arm-none-eabi-gdb -nx --batch \
  -ex "target extended-remote /dev/cu.usbmodemXXXX1" \
  -ex "monitor swdp_scan" \
  -ex "attach 1" \
  -ex "load" \
  -ex "kill" \
  _build/stm32h5/src/brd/proto4/proto4_fw_ftest.elf
```

---

## Step 3 — Run the test suite

```bash
_build/host/_deps/asrt-build/asrtio/asrtio serial \
  --port /dev/cu.usbmodemXXXX3 \
  --baud 115200 \
  --output _test/yellow.proto4/fw/proto4_fw_ftest
```

Optional flags:
- `--timeout <ms>` — per-test timeout (default: 5000 ms)
- `--params <file.json>` — parameter overrides for parameterised tests
- `-v` / `-vv` — verbosity (info / debug)

---

## Step 4 — Inspect results

```
_test/yellow.proto4/fw/proto4_fw_ftest/
  res.json               ← aggregate: OK/FAIL/SKIP counts + per-test status
  <test_name>.json       ← per-test metrics collected via store_metric
  spdlog.log             ← runner log
```

`res.json` exit codes per test:
- `retcode: 0` → PASS
- `retcode: 2` → FAIL (diag entry contains file/line of failed `expect`)

---

## Tests registered in `proto4_fw_ftest`

| Name | File | What it tests |
|------|------|---------------|
| `clock_test` | `interface_tests.hpp` | Clock driver |
| `comms_echo` | `interface_tests.hpp` | UART round-trip |
| `comms_timeout` | `interface_tests.hpp` | UART timeout handling |
| `cobs_uart_rx` | `impl_tests.hpp` | COBS framing receive |
| `cobs_uart_err` | `impl_tests.hpp` | COBS error recovery |
| `period_iface` | `interface_tests.hpp` | Period measurement |
| `pwm_motor` | `interface_tests.hpp` | PWM output |
| `hbridge_test` | `impl_tests.hpp` | H-bridge driver |
| `vcc_test` | `interface_tests.hpp` | VCC ADC |
| `temp_test` | `interface_tests.hpp` | Temperature ADC |
| `pos_test` | `interface_tests.hpp` | Position sensor |
| `curr_test` | `interface_tests.hpp` | Current sensor |
| `storage_iface` | `interface_tests.hpp` | Flash storage |
| `current_ctl_test` | `intg/ctl_test.hpp` | Current control loop accuracy |
| `sign_test` | `intg/ctl_test.hpp` | Current/velocity sign correlation |
| `measure_current` | `intg/meas_test.hpp` | Current measurement accuracy |
| `measure_position` | `intg/meas_test.hpp` | Position measurement accuracy |
| `measure_vel` | `intg/meas_test.hpp` | Velocity measurement accuracy |

---

## Open items

- [ ] Add `asrtio` + `libuv` to the `host_debug` build (CMakeLists.txt)
- [ ] Add `make flash_ftest` and `make run_ftest` targets to the Makefile
- [ ] Decide on a fixed serial port path or make it configurable via env var


## Overview

```
STM32H503 board
   proto4_fw_ftest.elf running
   ↕ COBS/UART (ST-Link virtual COM)
Host: asrtio serial runner
   → _test/yellow.proto4/fw/proto4_fw_ftest/res.json
```

---

## Step 1 — Build the firmware

```bash
make build_h5
# output: _build/stm32h5/src/brd/proto4/proto4_fw_ftest.elf
```

---

## Step 2 — Flash the board

Requires OpenOCD and an ST-Link connected to the board.

```bash
openocd \
  -f src/plt/stm32h5/openocd.cfg \
  -c "program _build/stm32h5/src/brd/proto4/proto4_fw_ftest.elf verify reset exit"
```

---

## Step 3 — Build the `asrtio` host runner

`asrtio` is the CLI test runner from the asrt library. It is fetched via CPM
during the stm32h5 build but **not built** (cross-compile target). It needs
a separate native build of the asrt sources.

TODO: determine the right way to build asrtio on macOS — options:
- Add a `host_asrtio` CMake preset that builds asrt with `BUILD_ASRTIO=ON`
- Build asrt standalone with cmake in `_build/stm32h5/_deps/asrt-src`
- Install asrtio from a release binary if asrt provides one

---

## Step 4 — Find the serial port

The ST-Link virtual COM port appears on macOS as:

```bash
ls /dev/cu.usbmodem*
# typically: /dev/cu.usbmodemXXXX
```

---

## Step 5 — Run the test suite

```bash
asrtio serial \
  --port /dev/cu.usbmodemXXXX \
  --baud 115200 \
  --output _test/yellow.proto4/fw/proto4_fw_ftest
```

Optional flags:
- `--timeout <ms>` — per-test timeout (default: 5000 ms)
- `--params <file.json>` — parameter overrides for parameterized tests
- `-v` / `-vv` — verbosity (info / debug)

---

## Step 6 — Inspect results

```
_test/yellow.proto4/fw/proto4_fw_ftest/
  res.json               ← aggregate: OK/FAIL/SKIP counts + per-test status
  <test_name>.json       ← per-test metrics collected via store_metric
  spdlog.log             ← runner log
```

`res.json` exit codes:
- `retcode: 0` → PASS
- `retcode: 2` → FAIL (diag entry contains file/line of failed expect)

---

## Tests registered in `proto4_fw_ftest`

| Name | File | What it tests |
|------|------|---------------|
| `clock_test` | `interface_tests.hpp` | Clock driver |
| `comms_echo` | `interface_tests.hpp` | UART round-trip |
| `comms_timeout` | `interface_tests.hpp` | UART timeout handling |
| `cobs_uart_rx` | `impl_tests.hpp` | COBS framing receive |
| `cobs_uart_err` | `impl_tests.hpp` | COBS error recovery |
| `period_iface` | `interface_tests.hpp` | Period measurement |
| `pwm_motor` | `interface_tests.hpp` | PWM output |
| `hbridge_test` | `impl_tests.hpp` | H-bridge driver |
| `vcc_test` | `interface_tests.hpp` | VCC ADC |
| `temp_test` | `interface_tests.hpp` | Temperature ADC |
| `pos_test` | `interface_tests.hpp` | Position sensor |
| `curr_test` | `interface_tests.hpp` | Current sensor |
| `storage_iface` | `interface_tests.hpp` | Flash storage |
| `current_ctl_test` | `intg/ctl_test.hpp` | Current control loop accuracy |
| `sign_test` | `intg/ctl_test.hpp` | Current/velocity sign correlation |
| `measure_current` | `intg/meas_test.hpp` | Current measurement accuracy |
| `measure_position` | `intg/meas_test.hpp` | Position measurement accuracy |
| `measure_vel` | `intg/meas_test.hpp` | Velocity measurement accuracy |

---

## Open items

- [ ] Resolve how to build/install `asrtio` natively on macOS
- [ ] Add `make flash_ftest` and `make run_ftest` targets to the Makefile
- [ ] Decide on a fixed serial port path or make it configurable via env var
- [ ] Add `_test/` results to CI or document as manual-only
