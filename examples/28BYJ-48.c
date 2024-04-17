#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <wiringPi.h>

#define MOTOR_PIN_1 0 // 连接到树莓派的GPIO引脚，需要根据你的实际连接修改
#define MOTOR_PIN_2 1
#define MOTOR_PIN_3 2
#define MOTOR_PIN_4 3

#define STEPS_PER_REVOLUTION 4096 // 28BYJ-48在8拍情况下：4096拍/外轴每转

void setup();
void setOneClockWiseBeat(int beat);
void setOneAnticlockWiseBeat(int beat);
void rotateMotorBySteps(int steps, bool clockwise, int speed);
void rotateMotorByAngle(int angle, bool clockwise, int speed);
void rotateMotorByTime(int seconds, bool clockwise, int speed);

// 步进电机的8拍顺时针序列
int motorClockWiseSteps[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}};

// 步进电机的8拍逆时针序列
int motorAnticlockwiseSteps[8][4] = {
    {1, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {1, 0, 0, 0}};

void setup()
{
    wiringPiSetup();
    pinMode(MOTOR_PIN_1, OUTPUT);
    pinMode(MOTOR_PIN_2, OUTPUT);
    pinMode(MOTOR_PIN_3, OUTPUT);
    pinMode(MOTOR_PIN_4, OUTPUT);
}

// 发送一次脉冲信号，beat表示拍号
void setOneClockWiseBeat(int beat)
{
    digitalWrite(MOTOR_PIN_1, motorClockWiseSteps[beat][0]);
    digitalWrite(MOTOR_PIN_2, motorClockWiseSteps[beat][1]);
    digitalWrite(MOTOR_PIN_3, motorClockWiseSteps[beat][2]);
    digitalWrite(MOTOR_PIN_4, motorClockWiseSteps[beat][3]);
}

void setOneAnticlockWiseBeat(int beat)
{
    digitalWrite(MOTOR_PIN_1, motorAnticlockwiseSteps[beat][0]);
    digitalWrite(MOTOR_PIN_2, motorAnticlockwiseSteps[beat][1]);
    digitalWrite(MOTOR_PIN_3, motorAnticlockwiseSteps[beat][2]);
    digitalWrite(MOTOR_PIN_4, motorAnticlockwiseSteps[beat][3]);
}

void rotateMotorBySteps(int steps, bool clockwise, int speed)
{
    // speed转delay
    if (speed == 0)
    {
        return;
    }
    // 每拍持续时间
    int microseconds = 100.0 / speed * 1000;
    if (clockwise)
    {
        // 一直转
        if (steps == -1)
        {
            for (;;)
            {
                for (int beat = 0; beat < 8; beat++)
                {
                    setOneClockWiseBeat(beat);
                    // 调整延时以改变步进速度
                    delayMicroseconds(microseconds);
                }
            }
        }
        else
        {
            for (int i = 0; i < steps; i++)
            {
                setOneClockWiseBeat(i % 8);
                delayMicroseconds(microseconds);
            }
        }
    }
    else
    {
        // 一直转
        if (steps == -1)
        {
            for (;;)
            {
                for (int beat = 0; beat < 8; beat++)
                {
                    setOneAnticlockWiseBeat(beat);
                    delayMicroseconds(microseconds);
                }
            }
        }
        else
        {
            for (int i = 0; i < steps; i++)
            {
                setOneAnticlockWiseBeat(i % 8);
                delayMicroseconds(microseconds);
            }
        }
    }
}

void rotateMotorByAngle(int angle, bool clockwise, int speed)
{
    int steps = angle * STEPS_PER_REVOLUTION / 360;
    rotateMotorBySteps(steps, clockwise, speed);
}

// 精度还可以
void rotateMotorByTime(int seconds, bool clockwise, int speed)
{
    // 每step运行时间
    int microseconds = 100.0 / speed * 1000 + 60; // 加一个修正值us
    // seconds时间能运行多少steps
    int eachStepTime = microseconds;
    int steps = seconds * 1000 * 1000 / eachStepTime;
    rotateMotorBySteps(steps, clockwise, speed);
}

int generate_random_number(int min, int max)
{
    srand(time(0));
    return min + rand() % (max - min + 1);
}

bool randClockwise()
{
    srand(time(0));
    return rand() % 2;
}

int main(void)
{
    setup();

    int angle;
    int speed;
    bool clockwise;

    // 测试随机角度，随机方向，随机速度
    for (;;)
    {
        angle = generate_random_number(0, 90);
        clockwise = randClockwise();
        speed = generate_random_number(50, 100);
        rotateMotorByAngle(angle, clockwise, speed);
        if (clockwise)
        {
            printf("angle: %d°\tclockwise: %d->\tspeed: %d%%\n", angle, clockwise, speed);
        }
        else
        {
            printf("angle: %d°\tclockwise: %d<-\tspeed: %d%%\n", angle, clockwise, speed);
        }

        delay(100);
    }

    // //测试变速转动
    // for(;;){
    //     int b = 3;
    //     for(int x = 0;x<=100;x++){
    //         degrees = x*b;
    //         clockwise = false;
    //         speed = -(1/25.0)*(x-50)*(x-50)+100+20;
    //         rotateMotorByAngle(b,clockwise,speed);
    //         printf("degress:%d\tclockwise:%d\tspeed:%d\n",degrees,clockwise,speed);
    //     }
    // }

    // // 测试设定转动时间
    // struct timeval start,end;
    // gettimeofday(&start, NULL );

    // rotateMotorByTime(5,false,100);

    // gettimeofday(&end, NULL );
    // long timeuse =1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
    // printf("time=%f\n",timeuse /1000000.0);

    return 0;
}
