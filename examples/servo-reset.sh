gpio  mode 1 pwm
gpio pwm-ms
gpio pwmc 192
gpio pwmr 2000
sleep 1

# 100%
gpio pwm 1 250
sleep 1

# 0%
gpio pwm 1 50
sleep 1

gpio  mode 1 pwm
gpio pwm-ms
gpio pwmc 192
gpio pwmr 2000

