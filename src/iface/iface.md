# Interface


## Types
#### int32_t
Standard 32bit integer value.
#### float
32bit float value supporting scientific syntax.
#### string
String limited up to 32 characters. Has to be enclosed in quotation marks.
#### bool
Bool value in form of string `true` or `false`.
#### expr_tok
Wildcard token for accepting: `string`, `float`, `int32_t`, `bool`

<!-- GEN BEGIN HERE -->
#### enum property

 - _mode_: Mode of the servomotor
 - _current_: Actual current flowing through the servomotor
 - _vcc_: Actual input voltage
 - _temp_: Actual temperature of the MCU
 - _position_: Actual position of the servomotor
 - _velocity_: Actual velocity of the servomotor

## Commands
### group `mode`

Set mode of servomotor

#### cmd `mode disengaged`

Set servomotor to disengaged mode, no power applied

#### cmd `mode power <power = 0>`

Regulate power applied to the servomotor

- _power_: Power between -1. and 1.
  - type: _float_
  - unit: __
#### cmd `mode current <current = 0.0>`

Regulate current applied to the servomotor

- _current_: Current level to set
  - type: _float_
  - unit: _A_
#### cmd `mode velocity <velocity = 0.0>`

Regulate velocity applied to the servomotor

- _velocity_: Velocity to set
  - type: _float_
  - unit: _rad/s_
#### cmd `mode position <position = 0.0>`

Regulate position applied to the servomotor

- _position_: Position to set
  - type: _float_
  - unit: _rad_
### cmd `prop <name>`

Get properties of servomotor

- _name_: Property to get
  - type: _property_
### group `cfg`

Configuration commands for the servomotor

#### cmd `cfg set <field> <value>`

Set a configuration field

- _field_: Configuration field to set
  - type: _string_
- _value_: Value to set for the configuration field, type depends on the field
  - type: _expr_tok_
#### cmd `cfg get <field>`

Get a configuration field

- _field_: Configuration field to get
  - type: _string_
#### cmd `cfg list5 <level = ""> <offset = 0> <n = 5>`

List subset of configuration fields

- _level_: Configuration level to list, empty for root level
  - type: _string_
- _offset_: Offset for the configuration fields to list
  - type: _int32_t_
- _n_: Number of configuration fields to list
  - type: _int32_t_
#### cmd `cfg commit`

Commit the current configuration changes into memory

#### cmd `cfg clear`

Clear the current configuration from memory

### cmd `info`

Get information about the servomotor
