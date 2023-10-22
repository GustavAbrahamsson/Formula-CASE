# Notes for FC-CONTROL_BOARD

## Hardware notes
### FC Control board v0.1
#### Known faults
- [ ] Wrong 3.3v regulator pinout ordered. Footprint is correct but non standard part was ordered.
- [ ] 5v from USB goes directly to regulated 5v rail (can lead to excessive current draw from/to USB). Should be separate or have reverse current protection.
- [ ] Servo pin header had wrong pinout. Should be 5v, GND, signal. Not signal, 5v, GND.
- [ ] Negative battery terminal is not connected.
- [ ] Silkscreen needs to be improved.
  - [ ] Add pinout for servo header.
  - [ ] Add pinout for battery header.
  - [ ] Label for Reset and Boot buttons needs to be fixed.
- [ ] Screw hole is under the USB connector. Connector should be moved to the side.
- [ ] Components at the back block mounting of back wing.

#### Improvements
- [ ] Improve Voltage/Current protection for USB and battery.
- [ ] Add connector with more GPIO pins for future expansion.
- [ ] Test opamp circuit for halleffect encoder. Implement on pcb if works well.
- [ ] 3.3v and 5v routing is not well done, can be improved.
## MAC address
Mac address for the board can be gotten by using `env:print-mac` and reading serial output.
```
- FC Control board v0.1: 34:85:18:5C:4E:F0
```

