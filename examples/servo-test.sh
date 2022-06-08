pin=12

#pwmc=192
#pwmr=2000
#max=230
#mid=120
#min=20

#pwmc=512
#pwmr=256
#max=90
#mid=46
#min=20

#pwmc=4000
#pwmr=240
#max=30
#mid=16
#min=5

#pwmc=400
#pwmr=240
#max=115
#mid=60
#min=26

#pwmc=50
#pwmr=4000
#max=900
#mid=500
#min=200

#pwmc=100
#pwmr=10000
#max=450
#mid=250
#min=90
#step=5

#pwmc=50
#pwmr=10000
#max=880
#mid=500
#min=200
#step=10

pwmc=60
pwmr=10000
max=733
mid=416
min=167
step=10

(( nsteps = ($max - $min) / $step ))

#nsteps=32
#(( step = ($max - $min) / $nsteps ))
#if [[ $step -lt 1 ]]; then step=1; fi

echo "pwmc = $pwmc"
echo "pwmr = $pwmr"
echo "max = $max"
echo "min = $min"
echo "mid = $mid"
echo "nsteps = $nsteps"
echo "step = $step"

function init() {
    echo "Init 1"
    gpio mode $pin pwm
    gpio pwm-ms

    gpio pwmc 192
    gpio pwmr 2000
    gpio pwm $pin 120
    sleep 1

    echo "Init 2"
    gpio pwmc $pwmc
    gpio pwmr $pwmr
    gpio pwm $pin $mid
    sleep 1
}

function reset() {
    echo "Reset"
    gpio mode $pin pwm
    gpio pwm-ms
    gpio pwmc $pwmc
    gpio pwmr $pwmr
    gpio pwm $pin $mid
    sleep 0.5
    gpio mode $pin out
    exit 0
}

function rangeUp() {
    for (( i=$min; i <= $max; i = $i + $step )); do
        echo "Set $i"
        gpio pwm $pin $i
        sleep 0.1
    done
}

function rangeDn() {
    for (( i=$max; i >= $min; i = $i - $step )); do
        echo "Set $i"
        gpio pwm $pin $i
        sleep 0.1
    done
}

trap reset INT TERM

init

rangeUp
sleep 1
rangeDn
sleep 1

# 100%
echo "Max"
gpio pwm $pin $max
sleep 3

# 0%
echo "Min"
gpio pwm $pin $min
sleep 3

# 50% (middle)
echo "Middle"
gpio pwm $pin $mid
sleep 3

echo "Done"
reset
