# power up motor with gpio

```
gpio -g mode <pin> pwm
gpio -g pwm <pin> <0-1023>
```

e.g.

```
gpio -g mode 12 pwm
gpio -g mode 19 pwm
gpio -g pwm 12 1023
gpio -g pwm 19 0
```

whereas 0 is strongest.
