# 7. Chained control loops architecture shall be used

Date: 2023-01-15

## Status

Accepted

Child of [3. STM32G4 is the first platform](0003-stm32g4-is-the-first-platform.md)

Child of [5. Servo shall provide velocity control](0005-servo-shall-provide-velocity-control.md)

Child of [6. Servo shall provide current control](0006-servo-shall-provide-current-control.md)

## Context

The internal architecture of the servomotor affects the characteristics of the system, and mostly should be clearly defined and understood.
This is high-level design principle, does not have to directly reflect the implementation.
(That is, any implementation that is practically equivalent to this is considered valid)

For the sake of this ADR, we assume that we can measure position,velocity,current, and that we can control the motor with power percentage as an input.
(-100...100%)

## Decision

Based on previous decisions, we have three main control modes: position, velocity, and current control.
All three represent a form of control that, in the end, affects the power value based on current values of sensory input.

There are multiple ways these free control modes can be supported by one system, in our case, we decided to have one control loop and chain those loop:

```
       ┌────────────┐
  rad ─┤POSITION    │
       └─────┬──────┘
             │
       ┌─────┴──────┐
rad/s ─┤VELOCITY    │
       └─────┬──────┘
             │
       ┌─────┴──────┐
   A  ─┤CURRENT     │
       └─────┬──────┘
             │
            PWR
```

That is, position/velocity control loops does not directly control the motor, but rather different control loop that in consequence controls the motor:
 - velocity control regulates current to the motor via control loop to achieve desired velocity
 - position control regulates velocity via velocity loop to achieve desired position

For this concept, we shall use the terminology of upstream loop and downstream loop: Upstream loop controls the downstream loop to create the desired effect, directly or undirectly.

The usage of control loops also implies that we have three separate loops that have to be tuned up.

## Consequences

The positive outcome of this architecture should be following property:

If downstream control loop is properly tuned up, tuning the upstream loop should be simpler.

This is key motivation for this architecture, step by step tuning of the current loop, followed by velocity loop, followed by position loop, should result in more stable system, than having independent loops.


The downside is, that if problem manifets in wokr with upstream loop, it might not be immediatly obvious where the problem is: is the upstream loop tuned incorrectly, or is one of the downstream loops incorrectly tunned? In a way that did not manifested in downstream tests?
