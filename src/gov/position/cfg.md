

<!-- GEN BEGIN HERE -->

## 1 pos_loop_p

 - _type_: `float`
 - _unit_: A/rad
 - _default_: `1.0F`

## 2 pos_loop_i

 - _type_: `float`
 - _unit_: A/(rad s)
 - _default_: `0.0001F`

## 3 pos_loop_d

 - _type_: `float`
 - _unit_: A s/rad
 - _default_: `0.0F`

## 4 curr_loop_p

 - _type_: `float`
 - _unit_: 1/A
 - _default_: `1.0F`

## 5 curr_loop_i

 - _type_: `float`
 - _unit_: 1/(A s)
 - _default_: `0.0001F`

## 6 curr_loop_d

 - _type_: `float`
 - _unit_: s/A
 - _default_: `0.0F`

## 7 static_friction_scale
Scaling of current in case of non-moving servo
 - _type_: `float`
 - _default_: `3.0`

## 8 static_friction_decay
Rate per second at which the static friction compensation rises while static and decays while moving
 - _type_: `float`
 - _unit_: 1/s
 - _default_: `1.0`

## 9 pos_lim_min
Soft limit for lowest position
 - _type_: `float`
 - _unit_: rad
 - _default_: `0.0F`

## 10 pos_lim_max
Soft limit for highest position
 - _type_: `float`
 - _unit_: rad
 - _default_: `2*pi`

## 11 pos_to_curr_lim_scale
Scale factor for position to current limit conversion
 - _type_: `float`
 - _unit_: A/rad
 - _default_: `10.0F`

## 12 curr_lim_min
Soft limit for lowest current
 - _type_: `float`
 - _unit_: A
 - _default_: `-2.0F`

## 13 curr_lim_max
Soft limit for highest current
 - _type_: `float`
 - _unit_: A
 - _default_: `2.0F`
<!-- GEN END HERE -->
