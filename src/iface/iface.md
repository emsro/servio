Interface
=========

Servio communicates via a character-based protocol. The user is expected to send a command-line-style message over UART delimited with '\n' character. The servo will reply with a JSON-encoded response. The UART is configured as follows: 8-bit word length, 1 stop bit, no parity, 230400 baud.

This section will explain how the system behaves, followed by a description of existing commands.

## Message

The generic pattern for a message is:
```
<root_cmd> <subcmd1> <value1> <arg2=value2> \n
```
Any message starts with a specific command that has to be executed. Commands can be organized into a nested structure, hence multiple keywords can be present to specify a command.
Any command keyword is separated by at least one whitespace character.

After the command, some commands may support arguments. These are separated by whitespace. Any arguments without a keyword are positionally matched to the expected argument set. Following that are arguments in the form of key=value pairs.
If an argument has a default value it does not have to be provided.

Note that commands and args are not explicitly separated. The parser is context-aware - it knows whether to expect a nested command or argument.

Here are some examples:
 - Non-nested command without args: `info`
 - Nested command without args: `cfg clear`
 - Non-nested command with position arg: `prop position`
 - Non-nested command with keyword arg: `prop name=position`
 - Nested command with position arg: `cfg get model`
 - Nested command with keyword arg: `cfg get field=model`
 - Nested command with position and keyword arg: `cfg set minimum_voltage value=0.1`

## Reply

Servio always replies with valid JSON, whose root element is an array with at least one item.
The first item is always either the string "OK" or "NOK".

Example combinations of answers for commands:
 - `cfg clear`: `["OK"]`
 - `cfg get model`: `["OK", "kavango6"]`
 - `info`: `["OK",{"version":"v0.2.0-82-g6fa680832","commit":"6fa68083287dee3e6f71438346d1c4406dc929ec"}]`
 - `invalid_cmd`: `["NOK","parse error","unknown command"]`

The number of items in the array and their semantics depend on each command.

## Types

Arguments of commands have types; these are validated by the parser.
Any command processing can be subject to more validation, but that is not done by the parser.

### int32_t
Standard 32-bit integer value.
### float
32-bit float value supporting scientific syntax.
### string
String limited up to 32 characters. Must be enclosed in quotation marks.
### bool
Boolean value in the form of a string `true` or `false`.
### expr_tok
Wildcard token for accepting: `string`, `float`, `int32_t`, `bool`

<!-- GEN BEGIN HERE -->
### enum property

 - _mode_: Mode of the servomotor
 - _current_: Actual current flowing through the servomotor
 - _vcc_: Actual input voltage
 - _temp_: Actual temperature of the MCU
 - _position_: Actual position of the servomotor
 - _velocity_: Actual velocity of the servomotor

Commands
========
## group: mode

Set mode of the servomotor

### cmd: mode disengaged

Set the servomotor to disengaged mode, no power applied

### cmd: mode power <power = 0>

Regulate the power applied to the servomotor. Sign of value controls direction of the movement. 1.0 is full power, 0.0 is no power, -1.0 is full reverse power.

- _power_: Power between -1.0 and 1.0
  - type: _float_
  - unit: __
### cmd: mode current <current = 0.0>

Regulate the current applied to the servomotor. Sign of value controls direction of the movement. Value is clamped based on current limits configuration.

- _current_: Current level to set
  - type: _float_
  - unit: _A_
### cmd: mode velocity <velocity = 0.0>

Regulate the velocity applied to the servomotor. Value is clamped based on velocity limits configuration.

- _velocity_: Velocity to set
  - type: _float_
  - unit: _rad/s_
### cmd: mode position <position = 0.0>

Regulate the position applied to the servomotor. The position will be clamped to fit within the current position limits. The way it is mapped depends on configuration of position_low_angle and position_high_angle.

- _position_: Position to set
  - type: _float_
  - unit: _rad_
## cmd: prop <name>

Get properties of the servomotor

- _name_: Property to get
  - type: _property_
## group: cfg

Configuration commands for the servomotor. See configuration documentation to get list of existing fields.

### cmd: cfg set <field> <value>

Set a configuration field

- _field_: Configuration field to set
  - type: _string_
- _value_: Value to set for the configuration field, type depends on the field
  - type: _expr_tok_
### cmd: cfg get <field>

Get a configuration field

- _field_: Configuration field to get
  - type: _string_
### cmd: cfg list5 <level = ""> <offset = 0> <n = 5>

List subset of configuration fields

- _level_: Configuration level to list, empty for root level
  - type: _string_
- _offset_: Offset for the configuration fields to list
  - type: _int32_t_
- _n_: Number of configuration fields to list
  - type: _int32_t_
### cmd: cfg commit

Commit the current configuration changes to memory

### cmd: cfg clear

Clear the current configuration from memory

## cmd: info

Get information about the servomotor


<!-- GEN END HERE -->
