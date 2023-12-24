# 10. STM32H5 is the main platform

Date: 2023-12-24

## Status

Accepted

Supercedes [3. STM32G4 is the first platform](0003-stm32g4-is-the-first-platform.md)

## Context

Servio is firmware for servomotors, that firmware has to run on some MCU - platform.
We want to pick one platform at a moment, that as sufficient capabilities and reasonable price.

When we wanted to iterated new PCB, we found out that previously used platform STM32G4 is out of stock.

## Decision

We picked STM32H5 line as it is similar to STM32G4 we had previous experience with. It has same crucial capabilities, is sold at lower price and frequency of MCU is higher.

Out of the line, we start with STM32H503.

## Consequences

1) We got a newer MCU that in theory should be feasible for the project for more time
2) The MCU is in stock and has lower price - higher availability of HW for the project.
3) The codebase has to be re-factored to support the new MCU, ideally this should be low amount o work, but given that this is first time platform is changed, this should won't be cheap time-wise.
