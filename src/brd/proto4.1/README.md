# Periphery usage

clock - tim2:counter
leds - tim2:ch3, gpio:pa0(red), gpio:pa1(blue), gpi:pa???(green)

adc_pooler - gpdma1:ch0, tim6:trigger, adc:ch15+gpio:pa3(current), adc:ch14+gpio:pa2(position), adc:ch7+gpio:pa7(vcc)
quad_encd - tim3:quad_encoder, gpio:pb4(ch1), gpio:pb5(ch2), gpio:pa2(clk)
spi - spi1, gpio:pb4(miso), gpio:pb5(mosi), gpi:pa2(sck)

debug_comms - uart2, gpdma1:ch2, gpio:pa11(rx), gpio:pa12(tx)
comms - uart1, gpdma1:ch1, gpio:pa10(rx), gpio:pa9(tx)

hbridge - gpio:pa4(vref), tim1:ch1+gpio:pb6(m1), tim1:ch2+gpio:pb7(m2)

eeprom - i2c2, gpio:pb10(scl), gpio:pb13(sda), gpio:pa15(wp)

debugger - gpio:pa13(swdio), gpio:pa14(swclk), gpio:pb3(traceswo)
