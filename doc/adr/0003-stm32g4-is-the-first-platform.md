# 3. STM32G4 is the first platform

Date: 2023-01-15

## Status

Parent of [7. Chained control loops architecture shall be used](0007-chained-control-loops-architecture-shall-be-used.md)

Superceded by [10. STM32H5 is the main platform](0010-stm32h5-is-the-main-platform.md)

## Context

The servio is firmware for servomotors, that firmware has to run on some MCU - platform.
We have to pick one platform to begin with, which should have sufficient capabilities for the project and do not limit us during development.
That is, we explicitly should not optimize price yet.

## Decision

We decided to pick STM32G4 series of MCUs given our positive previous experience with the platform, and the fact that we had multiple units in our inventory.
It should also easily be overpowered for our use case.

## Consequences

The project will be developed on this platform, that is: the development experience on this platform will affect the entire development.

There is good assumption that the project will (in the end) be developed in a way that has certain tailoring to the nuances of this platform.
This can't be really avoided unless we decided to support yet another platform.
However, we should still invest proper effort into minimizing that effect.
