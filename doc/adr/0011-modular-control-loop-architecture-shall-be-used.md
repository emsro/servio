# 11. Modular control loop architecture shall be used

Date: 2023-12-24

## Status

Accepted

Supercedes [7. Chained control loops architecture shall be used](0007-chained-control-loops-architecture-shall-be-used.md)

## Context

The internal architecture of the servomotor affects the characteristics of the system, and mostly should be clearly defined and understood. This is high-level design principle, does not have to directly reflect the implementation.

For the sake of this ADR, we assume that we can measure position, velocity, current, and that we can control the motor with power percentage as an input. (-100...100%)

## Decision

We want to use a modular approach for control loop architecture.

Internal architecture of the servo should be organized into independent control modes, of which each is well defined and understood for the user.

The concrete list of control modes is not defined by this architecture record.

## Consequences

Making the architecture modular should govern the way we thinkg about it - instead one solution for each use case we should focus more on separate architecture designs for each control mode.

By not specifying the list here we give ourselves a chance to easily introduce and get rid of control modes.
(Note that ADR for the specific modules can always be added in the future)
