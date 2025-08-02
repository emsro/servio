
<!-- GEN BEGIN HERE -->
Commands
========
## cmd: set <goal>

Set goal current for the servomotor

- _goal_: Goal current
  - type: _float_
## group: cfg

Configuration commands for the current governor.

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
### cmd: cfg list5 <offset = 0>

List subset of configuration fields

- _offset_: Offset for the configuration fields to list
  - type: _int32_t_

<!-- GEN END HERE -->
