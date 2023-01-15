# 6. Servo shall provide current control

Date: 2023-01-15

## Status

Accepted

Parent of [7. Chained control loops architecture shall be used](0007-chained-control-loops-architecture-shall-be-used.md)

## Context

The project represents firmware for servomotors, these however have various forms of modes.
That is, what exactly the input specifies and what is the servo trying to achieve.

## Decision

We agreed to provide current control.
In this mode, the servomotor gets desired current as an input.

The system shall measure current currently flowing throu the motor.
Current control shall control the motor in a way that currently flowing current equals the desired input.

The input is in ampers.

## Consequences

Given the decision, current control is one of the modes that has to be designed, tested, and verified that it works.
