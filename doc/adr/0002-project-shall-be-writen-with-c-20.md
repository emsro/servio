# 2. Project shall be writen with C++20

Date: 2023-01-15

## Status

Accepted

## Context

Project should've clearl specification as to what language version is necessary.

## Decision

We decided to use C++20, as it is language we are currently used to and use in our libraries, that should be used with this project. (emlabcpp)

## Consequences

It allows us usage of emlabcpp without modifications to it for backwards compability.
We can use the modern facilities provided by the newer version of the language.

We will suffer problems for use cases in which platform with old compiler has to be supported - C++20 is still quite new.
