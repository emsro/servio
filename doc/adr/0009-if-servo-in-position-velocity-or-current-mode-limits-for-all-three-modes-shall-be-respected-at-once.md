# 9. If servo in position, velocity, or current mode, limits for all three modes shall be respected at once

Date: 2023-01-17

## Status

Accepted

## Context

The control modes for the servomotor represents the form of control.
Those modes represent certain variables under control, for which it is practical for the user to manually configure limitation.

Current might be limited to prevent the servo exert too much strenght.
Velocity might be limited to prevent dangerous velocities.
Position might be limited to prevent the servo to move out of safe range.

## Decision

Limits for all three variables should be configurable by the sure, that is:
 - maximum and minimum current
 - maximum and minimum velocity
 - maximum and minimum position
 
Those limits should be configured in the units of those variables:
 - A
 - rad/s
 - rad

And those limits should be respect in all three modes of control: position, velocity, or current.

That is: Current control should also respect position and velocity limits.

## Consequences

This will complicate the control loop design, mostly because only thing that can enforce velocity limits is velocity control loop, which might not be active in case only current control is necessary.

