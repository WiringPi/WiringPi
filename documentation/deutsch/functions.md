# Dokumentation WiringPi-Bibliothek

Die WiringPi-Bibliothek ermöglicht den Zugriff auf die GPIO-Pins des Raspberry Pi. In dieser Dokumentation finden Sie Informationen zu den verfügbaren Funktionen und deren Verwendung. 
Seit Version 3 werden nun auch wieder Erweiterungen an der Schnittstelle vorgenommen. Bei Neuimplementierungen sollte man auf die aktuellen bzw. neunen Funktionen setzen.  
Das alte [GPIO Sysfs Interface for Userspace](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt) wird nun nicht mehr unterstützt. 

**ACHTUNG:** Diese Dokumemtation ist noch in Arbeit und somit unvollständig.  
Die Inhalte dieser Dokumentation wurden mit Sorgfalt und nach bestem Wissen und Gewissen erstellt. Die Autoren übernehmen jedoch keine Gewähr für die Richtigkeit, Vollständigkeit und Aktualität der bereitgestellten Informationen. Die Nutzung der Inhalte der Dokumentation erfolgt auf eigene Gefahr.  
Für Schäden materieller oder immaterieller Art, die durch die Nutzung oder Nichtnutzung der dargebotenen Informationen oder durch die Nutzung fehlerhafter und unvollständiger Informationen verursacht wurden, wird grundsätzlich keine Haftung übernommen.

## Installation

Leider steht die WiringPi Library nicht direkt in Raspberry Pi OS zur Verfügung, darum muss sie manuelle installiert weden.
Entweder man lädt sich ein Debian-Paket herunter oder übersetzt es manuell. 

**Debian-Paket erstellen:**

```bash
sudo apt install git
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
mv debian-template/wiringpi-3.1x.deb .
```

**Debian-Paket installieren:**

```bash
sudo apt install ./wiringpi-3.1x.deb
```

**Debian-Paket deinstallieren:**

```bash
sudo apt purge wiringpi
```


## Pin-Nummerierung und Raspbery Pi Modelle

GPIOs: https://pinout.xyz/pinout/wiringpi

**Raspberry Pi Modelle mit 40-Pin GPIO J8 Header:**

 | BCM | WPI |   Name  | Physisch  | Name    | WPI | BCM |
 |-----|-----|---------|:---------:|---------|-----|-----|
 |     |     |     3V3 |  1  I  2  | 5V      |     |     |
 |   2 |   8 |   SDA.1 |  3  I  4  | 5V      |     |     |
 |   3 |   9 |   SCL.1 |  5  I  6  | GND     |     |     |
 |   4 |   7 | GPIO. 7 |  7  I  8  | TxD     | 15  | 14  |
 |     |     |     GND |  9  I  10 | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 | 11  I  12 | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 | 13  I  14 | GND     |     |     |
 |  22 |   3 | GPIO. 3 | 15  I  16 | GPIO. 4 | 4   | 23  |
 |     |     |     3V3 | 17  I  18 | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | 19  I  20 | GND     |     |     |
 |   9 |  13 |    MISO | 21  I  22 | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | 23  I  24 | CE0     | 10  | 8   |
 |     |     |     GND | 25  I  26 | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 | 27  I  28 | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 | 29  I  30 | GND     |     |     |
 |   6 |  22 | GPIO.22 | 31  I  32 | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 | 33  I  34 | GND     |     |     |
 |  19 |  24 | GPIO.24 | 35  I  36 | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 | 37  I  38 | GPIO.28 | 28  | 20  |
 |     |     |     GND | 39  I  40 | GPIO.29 | 29  | 21  |


**Raspberry Pi 1B Rev. 2 mit 26-Pin GPIO P1 Header:**

 | BCM | WPI |   Name  | Physisch |  Name    | WPI | BCM |
 |-----|-----|---------|:--------:|----------|-----|-----|
 |     |     |     3V3 |  1 I 2   |  5V      |     |     |
 |   2 |   8 |   SDA.1 |  3 I 4   |  5V      |     |     |
 |   3 |   9 |   SCL.1 |  5 I 6   |  GND     |     |     |
 |   4 |   7 | GPIO. 7 |  7 I 8   |  TxD     | 15  | 14  |
 |     |     |     GND |  9 I 10  |  RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 | 11 I 12  |  GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 | 13 I 14  |  GND     |     |     |
 |  22 |   3 | GPIO. 3 | 15 I 16  |  GPIO. 4 | 4   | 23  |
 |     |     |     3V3 | 17 I 18  |  GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | 19 I 20  |  GND     |     |     |
 |   9 |  13 |    MISO | 21 I 22  |  GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | 23 I 24  |  CE0     | 10  | 8   |
 |     |     |     GND | 25 I 26  |  CE1     | 11  | 7   |



**Raspberry Pi 1B Rev. 1 mit 26-Pin GPIO P1 Header:**

 | BCM | WPI |   Name  | Physisch |  Name   | WPI | BCM |
 |-----|-----|---------|:--------:|---------|-----|-----|
 |     |     |     3V3 |  1 I 2   | 5V      |     |     |
 |   0 |   8 |   SDA.0 |  3 I 4   | 5V      |     |     |
 |   1 |   9 |   SCL.0 |  5 I 6   | GND     |     |     |
 |   4 |   7 | GPIO. 7 |  7 I 8   | TxD     | 15  | 14  |
 |     |     |     GND |  9 I 10  | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 | 11 I 12  | GPIO. 1 | 1   | 18  |
 |  21 |   2 | GPIO. 2 | 13 I 14  | GND     |     |     |
 |  22 |   3 | GPIO. 3 | 15 I 16  | GPIO. 4 | 4   | 23  |
 |     |     |     3V3 | 17 I 18  | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | 19 I 20  | GND     |     |     |
 |   9 |  13 |    MISO | 21 I 22  | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | 23 I 24  | CE0     | 10  | 8   |
 |     |     |     GND | 25 I 26  | CE1     | 11  | 7   |


**Hinweise**  
Beachten Sie die abweichende Pin-Nummern und den I2C0 bei Raspberry Pi 1B Rev. 1!


## Initialisierung

Am Beginn muss eine Initialisierung der WiringPi Library erfolgen.
Dazu muss eine der folgenden Funktionen aufgerufen werden:

Veraltete Funktionen (nicht mehr verwenden):  

``wiringPiSetup`` verwendet WiringPi-Nummerierung (WPI) der GPIO's und greift direkt auf die GPIO-Register zu.  
``wiringPiSetupGpio`` verwendet BCM-Nummerierung der GPIO's und greift direkt auf die GPIO-Register zu.  
``wiringPiSetupPhys`` verwendet physikalische Pin-Nummerierung der GPIO's und greift direkt auf die GPIO-Register zu.  
``wiringPiSetupSys`` verwendet BCM-Nummerierung und ruft ab Version 3.4 die neue Funktion ``wiringPiSetupGpioDevice`` auf, um die Kompatibilität auch mit neuen Kerneln zu gewährleisten.
In Version 2 wurde noch das virtuelle Dateisystem /sys/class/gpio verwendet. Der Export der GPIO's musste allerdings extern vor der Initialsierung erfolgen! Die Funktion ist veraltet und soll nicht
mehr benutzt werden!

**Ab Version 3.4:**  
``wiringPiSetupPinType`` entscheidet ob nun WiringPi, BCM oder physische Pin-Nummerierung verwendet wird, anhand des Parameters pinType. Es führt also die ersten 3 Setup-Funktionen auf eine zusammen.  
``wiringPiSetupGpioDevice`` ist der Nachfolger der ``wiringPiSetupSys`` Funktion und verwendet nun "GPIO Character Device Userspace API" in Version 1 (ab Kernel 5.10 verfügbar). Nähere Informationen findet man bei https://docs.kernel.org/driver-api/gpio/driver.html. Anhand des Parameters pinType wird wieder entschieden, welche Pin-Nummerierung verwendet wird.  
Bei dieser Variante wird nicht direkt auf den GPIO-Speicher (DMA) zugegriffen sondern über eine Kernel Schnittstelle, die mit Benutzerrechten verfügbar ist. Nachteil ist der eingeschrenkte Funktionsumfang und die niedrige Performance. 

<!-- Achtung Code der die neunen Funktionen benutzt ist nicht mehr mit der älterer Library Versionen 2 kompatibel! -->


### wiringPiSetup V2 (veraltet)

Inialisierung WiringPi in klassischer Art.  
>>>
```C
int wiringPiSetupGpio(void)
```  

``Rückgabewert``:  Fehlerstatus  
>  0 ... kein Fehler  

**Beispiel:**

```C
wiringPiSetupGpio();
```

### wiringPiSetup V3

Inialisierung WiringPi.  
>>>
```C
int wiringPiSetupPinType(enum WPIPinType pinType)
```  

``pinType``: Art der Pin-Nummerierung  
 - WPI_PIN_BCM ... BCM-Nummerierung  
 - WPI_PIN_WPI ... WiringPi-Nummerierung  
 - WPI_PIN_PHYS ... physikalische Nummerierung  

``Rückgabewert``:  Fehlerstatus  
> 0 ... kein Fehler  
> -1 ... Fehler ungültiger Parameter

**Beispiel:**

```C
wiringPiSetupPinType(WPI_PIN_BCM);
```

## Basisfunktionen

### pinMode

Ändert den Modus eines GPIO-Pins.
>>>
```C
void pinMode(int pin, int mode)
```  

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``mode``: Modus.  
 - INPUT ... Eingang  
 - OUTPUT ... Ausgang  
 - PWM_OUTPUT ... PWM Ausgang (Frequenz und Puls-Pause Verhältnis konfigurierbar)  
 - PWM_MS_OUTPUT ...  PWM Ausgang mit Modus MS (Mark/Space) (Ab Version 3)
 - PWM_BAL_OUTPUT ... PWM Ausgang mit Modus BAL (Balanced)  (Ab Version 3)
 - GPIO_CLOCK ... Frequenz Ausgang 
 - PM_OFF ... Freigabe

**Beispiel:**

```C
pinMode(17, OUTPUT);
```

**Support:**  
PM_OFF setzt den GPIO zurück (Input) und gibt ihn frei. PWM wird beendet.  
Raspberry Pi 5 unterstützt den PWM BAL (Balanced) Modus nicht. Bei PWM_OUTPUT wird der MS Modus aktiviert.
GPIO_CLOCK wird bei Raspberry Pi 5 (RP1) aktuell noch nicht unterstützt.

**PWM Ausgang**  
PWM_OUTPUT aktiviert den angegeben PWM Ausgang mit den Einstellungen: 
 - Modus: BAL-Balanced (Pi0-4), MS-Mark/Space (Pi 5)
 - Range: 1024  
 - Divider: 32  

Um sicher zu stellen, dass der Ausgang ohne aktiver Frequenz startet, sollte man vor der Aktivierung ``pwmWrite(PWM_GPIO, 0);`` ausführen. 
Danach können die entsprechenden Clock und Range Werte angepasst werden, ohne das bereits ungewollt eine Frequenz ausgegeben wird.


### pinMode


### digitalWrite

Setzt den Wert eines GPIO-Pins.
>>>
```C
void digitalWrite(int pin, int value)
```

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``value``: Der logische Wert.  
 - HIGH ... Wert 1 (elektisch ~3,3 V)  
 - LOW ... Wert 0 (elektisch ~0 V / GND)

**Beispiel:**

```C
pinMode(17, OUTPUT);
DigitalWrite(17, HIGH);
```


### pullUpDnControl

Ändert den internen Pull-Up/Down Widerstand.  
>>>
```C
void pullUpDnControl (int pin, int pud)
```  

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``pud``: Der Widerstand.  
 - PUD_OFF ... Kein Widerstand   
 - PUD_UP ... Pull-Up Widerstand (~50 kOhm)
 - PUD_DOWN ... Pull-Down Widerstand (~50 kOhm)

**Beispiel:**

```C
pullUpDnControl(17, PUD_DOWN);
```


### digitalRead

Liest den Wert eines GPIO-Pins.
>>>
```C
int digitalRead(int pin)
```

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``Rückgabewert``: Der logische gelesene Wert.  
> HIGH ... Wert 1  
> LOW ... Wert 0  

**Beispiel:**

```C
pinMode(17, INPUT);
pullUpDnControl(17, PUD_DOWN);
int value = digitalRead(17);
if (value==HIGH) 
{
    // your code
}
```


## Interrupts

### wiringPiISR

Registriert eine Interrupt Service Routine (ISR) bzw. Funktion die bei Flankenwechsel ausgeführt wird.

>>>
```C
int wiringPiISR(int pin, int edgeMode, void (*function)(struct WPIWfiStatus wfiStatus), unsigned long bouncetime);
```

``pin``: Der gewünschte Pin (BCM\-, WiringPi\- oder Pin\-Nummer).  

``edgeMode``: Auslösende Flankenmodus  
 - INT_EDGE_RISING ... Steigende Flanke  
 - INT_EDGE_FALLING ... Fallende Flanke  
 - INT_EDGE_BOTH ... Steigende und fallende Flanke  

``*function``: Funktionspointer für ISR mit Rückgabeparameter struct WPIWfiStatus:   
```C  
struct WPIWfiStatus {
    int status;             // -1: error, 0: timeout, 1: valid values for edge and timeStamp_us
    unsigned int gpioPin;   // gpio as BCM pin
    int edge;               // One of INT_EDGE_FALLING or INT_EDGE_RISING	
    long long int timeStamp_us;     // time stamp in microseconds, when interrupt happened
};
```  
 
``bouncetime``: Entprellzeit in Microsekunden, 0 ms schaltet das Entprellen ab   

``Rückgabewert``:   
 > 0 ... Erfolgreich  
<!-- > <>0 ... Fehler, zur Zeit nicht implementiert -->

Beispiel siehe waitForInterrupt.  


### wiringPiISRStop

Deregistriert die Interrupt Service Routine (ISR) auf einem Pin.

>>>
```C
int wiringPiISRStop (int pin)
```

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``Rückgabewert``:  
> 0 ... Erfolgreich  
<!-- > <>0 ... Fehler, zur Zeit nicht implementiert -->


### waitForInterrupt

Wartet auf einen Aufruf der Interrupt Service Routine (ISR) mit Timeout und Entprellzeit in Mikrosekunden.  
Blockiert das Programm bis zum Eintreffen der auslösenden Flanke oder bis zum Ablauf des Timeouts.  
>>>
```C
struct WPIWfiStatus wfiStatus waitForInterrupt (int pin, int edgeMode, int mS, unsigned long bouncetime)
```

``pin``: Der gewünschte Pin (BCM\-, WiringPi\- oder Pin\-Nummer).  

``edgeMode``: Auslösende Flankenmodus  
 - INT_EDGE_RISING ... Steigende Flanke  
 - INT_EDGE_FALLING ... Fallende Flanke  
 - INT_EDGE_BOTH ... Steigende und fallende Flanke  
 
``mS``: Timeout in Milisekunden.  
 - \-1 warten ohne timeout  
 - 0 wartet nicht  
 - 1...n wartet maximal n mS Millisekunden  
 
``bouncetime``: Entprellzeit in Microsekunden, 0 schaltet Entprellen ab.  

``Rückgabewert``:  
```C  
struct WPIWfiStatus {
    int status;             // -1: error, 0: timeout, 1: valid values for edge and timeStamp_us
    unsigned int gpioPin;   // gpio as BCM pin
    int edge;               // One of INT_EDGE_FALLING or INT_EDGE_RISING	
    long long int timeStamp_us;     // time stamp in microseconds, when interrupt happened
};
```  

**Beispiel:**

```C  
/*
 * isr_debounce.c:
 *	Wait for Interrupt test program  WiringPi >=3.16 - ISR method
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

#include <time.h>

#define BOUNCETIME 3000 // microseconds
#define BOUNCETIME_WFI  300
#define TIMEOUT    10000
//*************************************
// BCM pins
// IRQpin : setup as input with internal pullup. Connected with push button to GND with 1K resistor in series.
// OUTpin : connected to a LED with 470 Ohm resistor in series to GND. Toggles LED with every push button pressed.
//*************************************
#define IRQpin     16
#define OUTpin     12 

int toggle = 0;

/*
 * myInterrupt:
 *********************************************************************************
 */

static void wfi (struct WPIWfiStatus wfiStatus) { 
//  struct timeval now;
  long long int timenow, diff;
  struct timespec curr;
  char *edgeType;
  
  if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1) {
        printf("clock_gettime error");
        return;
  }
  
  timenow = curr.tv_sec * 1000000LL + curr.tv_nsec/1000L; // convert to microseconds
  diff = timenow - wfiStatus.timeStamp_us;
  if (wfiStatus.edge == INT_EDGE_RISING)
      edgeType = "rising";
  else if (wfiStatus.edge == INT_EDGE_FALLING)
      edgeType = "falling";
  else
      edgeType = "none";
  printf("gpio BCM = %d, IRQ edge = %s, timestamp = %lld microseconds, timenow = %lld, diff = %lld\n", wfiStatus.gpioPin, edgeType, wfiStatus.timeStamp_us, timenow, diff);
  if (toggle == 0) {
    digitalWrite (OUTpin, HIGH) ;
    toggle = 1;
  }
  else {
    digitalWrite (OUTpin, LOW) ; 
    toggle = 0;
  }
}



int main (void)
{
  int major, minor;
  
  wiringPiVersion(&major, &minor);

  printf("\nISR debounce test (WiringPi %d.%d)\n\n", major, minor);
  
  wiringPiSetupGpio() ;

  pinMode(IRQpin, INPUT);
  
  // pull up/down mode (PUD_OFF, PUD_UP, PUD_DOWN) => down
  pullUpDnControl(IRQpin, PUD_UP);

  pinMode(OUTpin, OUTPUT);
  digitalWrite (OUTpin, LOW) ;

  printf("Testing waitForInterrupt on both edges IRQ @ GPIO%d, timeout is %d\n", IRQpin, TIMEOUT);  
  struct WPIWfiStatus wfiStatus = waitForInterrupt(IRQpin, INT_EDGE_BOTH, TIMEOUT, BOUNCETIME_WFI );
  if (wfiStatus.status < 0) {
    printf("waitForInterrupt returned error\n");
    pinMode(OUTpin, INPUT);
    return 0;    
  }
  else if (wfiStatus.status == 0) {
    printf("waitForInterrupt timed out\n\n");
  }   
  else {
    if (wfiStatus.edge == INT_EDGE_FALLING)
        printf("waitForInterrupt: GPIO pin %d falling edge fired at %lld microseconds\n\n", wfiStatus.gpioPin, wfiStatus.timeStamp_us); 
    else
        printf("waitForInterrupt: GPIO pin %d rising edge fired at %lld microseconds\n\n", wfiStatus.gpioPin, wfiStatus.timeStamp_us);        
  }
  
  printf("Testing IRQ @ GPIO%d with trigger @ GPIO%d on both edges and bouncetime %d microseconds. Toggle LED @ OUTpin on IRQ.\n\n", IRQpin, IRQpin, BOUNCETIME, OUTpin);
  printf("To stop program hit return key\n\n");
  
  wiringPiISR (IRQpin, INT_EDGE_BOTH, &wfi, BOUNCETIME) ; 
 
  getc(stdin);
  
  wiringPiISRStop (IRQpin) ;  
  pinMode(OUTpin, INPUT);

  return 0 ;  
}
```

Output auf dem Terminal:  

```  
pi@RaspberryPi:~/wiringpi-test-v3.16 $ gcc -o isr_debounce isr_debounce.c -l wiringPi
pi@RaspberryPi:~/wiringpi-test-v3.16 $ ./isr_debounce

ISR debounce test (WiringPi 3.16)

Testing waitForInterrupt on both edges IRQ @ GPIO16, timeout is 10000
waitForInterrupt: GPIO pin 16 falling edge fired at 256522528012 microseconds

Testing IRQ @ GPIO16 with trigger @ GPIO16 on both edges and bouncetime 3000 microseconds. Toggle LED @ OUTpin on IRQ.

To stop program hit return key

gpio BCM = 16, IRQ edge = rising, timestamp = 256522668010 microseconds, timenow = 256522668017, diff = 7
gpio BCM = 16, IRQ edge = falling, timestamp = 256536364014 microseconds, timenow = 256536364021, diff = 7
gpio BCM = 16, IRQ edge = rising, timestamp = 256536952011 microseconds, timenow = 256536952018, diff = 7
gpio BCM = 16, IRQ edge = falling, timestamp = 256537856010 microseconds, timenow = 256537856016, diff = 6
gpio BCM = 16, IRQ edge = rising, timestamp = 256538744011 microseconds, timenow = 256538744018, diff = 7
gpio BCM = 16, IRQ edge = falling, timestamp = 256539664010 microseconds, timenow = 256539664017, diff = 7
gpio BCM = 16, IRQ edge = rising, timestamp = 256540516010 microseconds, timenow = 256540516016, diff = 6
gpio BCM = 16, IRQ edge = falling, timestamp = 256541560013 microseconds, timenow = 256541560022, diff = 9
gpio BCM = 16, IRQ edge = rising, timestamp = 256542360010 microseconds, timenow = 256542360016, diff = 6
gpio BCM = 16, IRQ edge = falling, timestamp = 256543320012 microseconds, timenow = 256543320020, diff = 8
gpio BCM = 16, IRQ edge = rising, timestamp = 256544092021 microseconds, timenow = 256544092029, diff = 8
^C
pi@RaspberryPi:~/wiringpi-test-v3.16 $
```  


## Hardware PWM (Pulsweitenmodulation)

Verfügbare GPIOs:  https://pinout.xyz/pinout/pwm

### pwmWrite

Verändert den PWM Wert des Pins. Mögliche Werte sind 0-{PWM Range} 

>>>
```C
pwmWrite(int pin, int value)
```

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``value``: PWM Wert

### pwmSetRange

Setzt den Bereich für den PWM-Wert für alle PWM Pins bzw. PWM Kanäle.  
Für die Berechnung der PWM Frequenz (M/S Mode) gilt 19200/divisor/range.
Bei Befehl ``pinMode(pin,PWM_OUTPUT)`` wird automatisch der Wert 1024 für den Teiler gesetzt. 

>>>
```C
pwmSetRange (unsigned int range)
```

``range``: PWM Range

### pwmSetMode

Setzt den PWM Modus für alle PWM Pins bzw. PWM Kanäle.   

>>>
```C
pwmSetMode(int mode);
```

``mode``: PWM Modus
 - PWM_MODE_MS ... Mark/Space Modus (PWM Frequenz fix)  
 - PWM_MODE_BAL ... Balanced Modus (PWM Frequenz variabel)

**Support:**  
Raspberry Pi 5 unterstützt den Balanced Modus nicht!


### pwmSetClock

Setzt den Teiler für den PWM Basistakt. Der Basistakt ist für alle Raspberry Pis auf 1900 kHz normiert.  
Für die Berechnung der PWM Frequenz (M/S Mode) gilt 19200/divisor/range.
Bei Befehl ``pinMode(pin,PWM_OUTPUT)`` wird automatisch der Wert 32 für den Teiler gesetzt. 

>>>
```C
pwmSetClock(int divisor)
```

``divisor``: Teiler  (Raspberry Pi 4: 1-1456, alle anderen 1-4095) 
- 0 ... Deaktivert den PWM Takt bei Raspberry Pi 5, bei anderen Pi's wird divisor 1 benutzt    

**Support:**  
Der Raspberry Pi 4 hat aufgrund seines höheren internen Basistakt nur einen Einstellbereich von 1-1456.  
Ansonsten gilt 0-4095 für einen gültigen Teiler.


**Beispiel:**

```C
int main (void) {
    wiringPiSetupGpio() ;

    pwmSetRange(1024);
    pwmSetClock(35);
    pwmWrite(18, 512);
    pinMode(18, PWM_MS_OUTPUT);
    double freq = 19200.0/(double)pwmc/(double)pwmr;
    printf("PWM 50%% @ %g kHz", freq);
    delay(250);
    pinMode(18, PM_OFF);
}
```


## I2C - Bus


``wiringPiI2CRawWrite`` und ``wiringPiI2CRawRead`` sind die neuen Funktionen der Version 3 mit denen nun direkt I2C-Daten gesendet und gelesen weden können.  
Die anderen Schreib- und Lese-Funktionen verwenden das SMBus-Protokoll, das üblicherweise bei I2C-Chips Verwendung findet.


### wiringPiI2CSetup

Öffnet den default I2C-Bus am Raspberry Pi und adressiert das angegebene Gerät / Slave.

>>>
```C
wiringPiI2CSetup(const int devId)
```

``devId``: I2C-Gerät / Slave Adresse.  
``Rückgabewert``:  Datei Handle zum I2C-Bus   
> -1 ... Fehler bzw. EXIT (Programm Beendigung)

**Beispiel**
>>>
```C
int fd = wiringPiI2CSetup(0x20);
```


### wiringPiI2CSetupInterface

Öffnet den angebenen I2C-Bus und adressiert das angegebene I2C-Gerät / Slave.

>>>
```C
wiringPiI2CSetupInterface(const char *device, int devId)
```

``devId``: I2C-Gerät / Slave Adresse.  
``Rückgabewert``:  Datei Handle zum I2C-Bus   
> -1 ... Fehler bzw. EXIT (Programm Beendigung)

**Beispiel**
>>>
```C
int fd = wiringPiI2CSetupInterface("/dev/i2c-1", 0x20);
```


### wiringPiI2CWrite / wiringPiI2CWriteReg8 / wiringPiI2CWriteReg16 / wiringPiI2CWriteBlockData

...

### wiringPiI2CRawWrite


Schreiben von Daten zu einem I2C-Slave.

>>>
```C
int wiringPiI2CRawWrite(int fd, const uint8_t *values, uint8_t size)
```

``fd``: Datei Handle.  
``values``: Quellpuffer.  
``size``: Anzahl der Bytes die com Quellpuffer geschrieben werden sollen.  
``Rückgabewert``:  Anzahl der Bytes die geschrieben wurden.

**Beispiel**
>>>
```C
int fd = wiringPiI2CSetup(I2C_ADDR);
if (fd>0) {
    uint8_t i2cvalue = 0x55;
    int result = wiringPiI2CRawWrite(fd, &i2cvalue, 1);
    if (1 == result) {
        // 1 byte from i2cvalue send to I2C_ADDR slave  
    } else {
        // error
    }
} else {
    // error
}
```

### wiringPiI2CRead / wiringPiI2CReadReg8 / wiringPiI2CReadReg16 / wiringPiI2CReadBlockData

...

### wiringPiI2CRawRead

Lesen von Daten von einem I2C-Slave.

>>>
```C
int wiringPiI2CRawRead(int fd, uint8_t *values, uint8_t size)
```

``fd``: Datei Handle.  
``values``: Zielpuffer.  
``size``: Anzahl der Bytes die in den Zielpuffer gelesen werden sollen.  
``Rückgabewert``:  Anzahl der Bytes die gelesen wurden.

**Beispiel**
>>>
```C
int fd = wiringPiI2CSetup(I2C_ADDR);
if (fd>0) {
    uint8_t i2cvalue;
    int result = wiringPiI2CRawRead(fd, &i2cvalue, 1);
    if (1 == result) {
        // 1 byte received from I2C_ADDR and stored to i2cvalue  
    } else {
        // error
    }
} else {
    // error
}
```


## SPI - Bus

...
