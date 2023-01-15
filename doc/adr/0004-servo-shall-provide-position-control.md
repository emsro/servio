# 4. Servo shall provide position control

Date: 2023-01-15

## Status

Accepted

## Context

The project represents firmware for servomotors, these however have various forms of modes.
That is, what exactly the input specifies and what is the servo trying to achieve.

## Decision

We agreed to provide position control.
In this mode, the servomotor gets desired position as an input.

The system assumes that the motor output shaft is connected in appropiate manner to the currently used position sensor.
Position control shall provide bi-directional control of the motor so that the shaft is moved in a position, in which the position sensor returns the desired value.

The input is in radians.

## Consequences

Given the decision, position control is one of the modes that has to be designed, tested, and verified that it works.

