

# Config fields

<!-- GEN BEGIN HERE -->

## 1 model
Model of the servomotor, used for debugging purposes
 - _type_: `string`
 - _default_: `"no model"`

## 2 id
ID of the servomotor
 - _type_: `uint32_t`

## 3 group_id
Group ID of the servomotor
 - _type_: `uint32_t`

## 4 encoder_mode
Encoder mode of the servomotor
 - _type_: `cfg::encoder_mode`
 - _default_: `cfg::encoder_mode::analog`

## 11 position_low_angle
Expected lowest position of the servomotor
 - _type_: `float`
 - _unit_: rad

## 12 position_high_angle
Expected highest position of the servomotor
 - _type_: `float`
 - _unit_: rad
 - _default_: `2*pi`

## 14 current_conv_scale
Current conversion scale factor for formula: C = ADC * scale + offset
 - _type_: `float`
 - _default_: `1.0`

## 15 current_conv_offset
Current conversion offset for formula: C = ADC * scale + offset
 - _type_: `float`

## 16 temp_conv_scale
Temperature conversion scale factor for formula: T = ADC * scale + offset
 - _type_: `float`
 - _default_: `1.0`

## 17 temp_conv_offset
Temperature conversion offset for formula: T = ADC * scale + offset
 - _type_: `float`

## 18 voltage_conv_scale
Voltage conversion scale factor for formula: V = ADC * scale
 - _type_: `float`
 - _default_: `1.0`

## 19 invert_hbridge
Invert direction of the H-bridge
 - _type_: `bool`
 - _default_: `false`

## 62 minimum_voltage
Minimum voltage for the servomotor to operate
 - _type_: `float`
 - _unit_: voltage
 - _default_: `6.0`

## 65 maximum_temperature
Maximum temperature for the servomotor to operate
 - _type_: `float`
 - _unit_: celsius
 - _default_: `80.0`

## 66 moving_detection_step
Deadband for the moving detection
 - _type_: `float`
 - _unit_: rad
 - _default_: `0.05F`

## 80 quad_encoder_range
Number of pulses per revolution of the quadrature encoder
 - _type_: `uint32_t`
 - _default_: `2048`
         <!-- GEN END HERE -->
