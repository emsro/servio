# Motor related math

Write down of math related to DC motors that we accumulated, the purpose is to have one source of all information and consistent usage of symbols.

Sources:
 - [https://en.wikipedia.org/wiki/Motor_constants](https://en.wikipedia.org/wiki/Motor_constants)

# Dictionary

| | unit | |
| ----- | ---- | --- |
| $I$ | $A$ |  motor current
| $R$ | $\Omega$ | motor electrical resistance
| $V$ | $V$ | motor voltage
| $P$ | $W=\frac{Nm}{s}$ | motor power
| $\omega$ | $\frac{rad}{s}$ | motor velocity
| $\omega_n$ | $\frac{rad}{s}$ | no-load velocity
| $\alpha$ | $\frac{rad}{s^2}$ | motor angular acceleration
| $\tau$ | $Nm$ | motor [torque](https://en.wikipedia.org/wiki/Torque) - moment of force
| $\tau_s$| $Nm$| stalled motor torque
| $J$ | $kg\cdot m^2 $ | moment of inertia
| $V_{emf}$ | $V$ | Back counter electromotive force voltage - [EMF](https://en.wikipedia.org/wiki/Counter_electromotive_force)
| $K_v$ | $\frac{rad}{s V}$ | motor velocity constant
| $K_T$ | $\frac{N m }{A}$ | motor torque constant
| $K_M$ | $\frac{Nm}{\sqrt{W}}$ | motor constant

# Equations

||||
| --- | --- | --- |
| Ohm's law | $V = IR$||
| Torque to current | $\tau = I_a K_T$ | $I_a$ is armature current
| Motor constant | $K_m = \frac{\tau}{\sqrt{P}} = \frac{K_T I}{\sqrt{P}} = \frac{K_T I}{\sqrt{I^2 R}} = \frac{K_T}{\sqrt{R}}$ | $P$ is resistive power loss
| Back EMF | $V_{emf} = K_v \omega$ |
| Motor velocity constant | $K_v = \frac{\omega_n}{V_{peak}} $ | Note the velocity without load
| Torque to force | $\tau = r F$ | $r$ is radius and $F$ is force
| Rot. equivalent of 2. Newton's law | $\tau = J \alpha$
| Instantaneous power | $P = \tau \omega$


```mermaid
xychart-beta
    title "Motor torque/velocity curve"
    x-axis "Torque" 0 --> 1
    y-axis "Velocity" 0 --> 1
    line [0, 1]
         [1, 0]
```
