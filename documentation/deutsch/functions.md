# WiringPi-Bibliothek Dokumentation
Die WiringPi-Bibliothek ermöglicht den Zugriff auf die GPIO-Pins des Raspberry Pi. In dieser Dokumentation finden Sie Informationen zu den verfügbaren Funktionen und deren Verwendung.
Seit Version 3 werden nun auch wieder Erweiterungen an der Schnittstelle vorgenommen. Bei Neuimplementierungen sollte man auf die aktuellen bzw. neunen Funktionen setzen, da WiringPi 3 auch auf alten Systemen (ab Kernel 5.10) installiert werden kann.  

ACHTUNG: Diese Dokumemtation ist noch in Arbeit und somit unvollständig.  

## Installation

Leider steht die WiringPi Library nicht direkt in Raspberry Pi OS zur Verfügung, darum muss sie manuelle installiert weden.
Entweder man lädt sich ein Debian-Paket herunter oder übersetzt es manuell. 

**Debian-Paket erstellen:**

```bash
sudo apt install git
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
mv debian-template/wiringpi-3.0-1.deb .
```

**Debian-Paket installieren:**

```bash
sudo apt install ./wiringpi-3.0-1.deb
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


### wiringPiSetup V2

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
int wiringPiISR(int pin, int mode, void (*function)(void));
```

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``mode``: Auslösende Flankenmodus
 - INT_EDGE_RISING ... Steigende Flanke  
 - INT_EDGE_FALLING ... Fallende Flanke  
 - INT_EDGE_BOTH ... Steigende und fallende Flanke  

``*function``: Funktionspointer für ISR  
``Rückgabewert``:   
 > 0 ... Erfolgreich  
<!-- > <>0 ... Fehler, zur Zeit nicht implementiert -->

Beispiel siehe wiringPiISRStop.


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


**Beispiel:**

```C
static volatile int edgeCounter;

static void isr(void) { 
  edgeCounter++;
}

int main (void) {
    wiringPiSetupPinType(WPI_PIN_BCM);
    edgeCounter = 0;
    wiringPiISR (17, INT_EDGE_RISING, &isr);
    Sleep(1000);
    printf("%d rinsing edges\n", )
    wiringPiISRStop(17) ;
}
```


### waitForInterrupt

Wartet auf einen Aufruf der Interrupt Service Routine (ISR) mit Timeout.

>>>
```C
int  waitForInterrupt (int pin, int mS)
```

``pin``: Der gewünschte Pin (BCM-, WiringPi- oder Pin-Nummer).  
``mS``: Timeout in Milisekunden.  
``Rückgabewert``: Fehler  
> 0 ... Erfolgreich  
> -1 ... GPIO Device Chip nicht erfolgreich geöffnet  
> -2 ... ISR wurde nicht registriert (wiringPiISR muss aufgerufen werden)


## Hardware PWM (Pulsweitenmodulation)

Verfügbare GPIOs:  https://pinout.xyz/pinout/pwm

### pwmWrite

Verändert den PWM Wert des Pins. Mögliche Werte sind 0-<PWM Range> 

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
