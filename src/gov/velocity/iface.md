
<!-- GEN BEGIN HERE -->
Commands
========
## cmd: set <goal>

Set velocity of the servomotor

- _goal_: Goal velocity
  - type: _float_
## group: cfg

Configuration commands for the position governor.

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
### cmd: cfg list <index = 0>

List X-th configuration field

- _index_: Index for the configuration field to list, returns nothing in case of out of range index
  - type: _int32_t_

<!-- GEN END HERE -->
