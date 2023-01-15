# 8. CMake is the one only build system

Date: 2023-01-15

## Status

Accepted

## Context

The build system inevitably affects the project.
It is the tool that we use to combine the code with compiler to build the firmware, host tests, or other artefacts.

## Decision

The CMake is and will be the only build system used by the project.
There is not intent to support any other build system.

## Consequences

CMake should be properly supported these days, but we can still expected that it is not compatible with some of the native embedded IDEs.
We, however, do not expect that the sacrifice would be a problematic in the future.
