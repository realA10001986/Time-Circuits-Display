
DIY additions for the Time Circuits Display

## Speedometer

| [![Watch the video](https://img.youtube.com/vi/opAZugb_W1Q/0.jpg)](https://youtu.be/opAZugb_W1Q) |
|:--:|
| Click to watch the video |

The speedo display shown in this video (which was shot before the CircuitSetup Speedo was available) is based on a fairly well-designed stand-alone replica I purchased on ebay. I removed the electronics inside and wired the LED segments to an Adafruit i2c backpack (from the Adafruit 878 product) and connected it to the TCD. Yes, it is really that simple. (The TFC switch was made by me, see [here](https://tfc.out-a-ti.me), it uses the [Fake Power Switch](#fake-power-switch) feature of the TCD.)

What you need is a box, the LED segment displays and a HT16K33-based PCB that allows accessing the LED displays via i2c (address 0x70). There are various readily available LED segment displays with suitable i2c break-outs from Adafruit and Seeed (Grove) that can be used as a basis. Adafruit [878](https://www.adafruit.com/product/878)/[5599](https://www.adafruit.com/product/5599), [1270](https://www.adafruit.com/product/1270) and [1911](https://www.adafruit.com/product/1911), as well as Grove 0.54" 14-segment [2-digit](https://www.seeedstudio.com/Grove-0-54-Red-Dual-Alphanumeric-Display-p-4031.html) or [4-digit](https://www.seeedstudio.com/Grove-0-54-Red-Quad-Alphanumeric-Display-p-4032.html) alphanumeric displays are supported. (The product numbers vary with color, the numbers here are the red ones.)

Since the I2C bus is already quite long from the control board to the last display in the chain, I recommend soldering another XH 4pin plug onto the control board (there are two additional i2c break-outs available), and to connect the speedometer there. For wiring information, please see [here](#appendix-b-i2c-peripheral-wiring).

#### Software setup

For DIY speedos, there are two special options in the Speedo Display Type drop-down: *Ada 1911 (left tube)* and *Ada 878 (left tube)*. These two can be used if you connect only one 2-digit-tube to the respective Adafruit i2c backpack, as I did in case of my speedo replica as well as my [Wall Clock](#my-custom-made-time-circuits-wall-clock).

## GPS receiver

The CircuitSetup original [speedo](https://circuitsetup.us/product/delorean-time-machine-speedometer-pcb/?v=fa868488740a) has a built-in GPS receiver, but the firmware also supports alternatives such as the Adafruit Mini GPS PA1010D (product id [4415](https://www.adafruit.com/product/4415)) or the Pimoroni P1010D GPS Breakout ([PIM525](https://shop.pimoroni.com/products/pa1010d-gps-breakout?variant=32257258881107)) or any other MT(K)3333-based GPS receiver, connected through i2c (slave address 0x10). The GPS receiver can be used as a source of authoritative time (like NTP) and speed of movement.

GPS receivers receive signals from satellites, but in order to do so, they need to be "tuned in" (aka get a "fix"). This "tuning" process can take a long time; after first power up, it can take 30 minutes or more for a receiver to be able to determine its position. In order to speed up this process, modern GPS receivers have special "assisting" features. One key element is knowledge of current time, as this helps identifying satellite signals quicker. So, in other words, initially, you need to tell the receiver what it is supposed to tell you. However, as soon as the receiver has received satellite signals for 15-20 minutes, it saves the data it collected to its battery-backed memory and will find a fix within seconds after power-up in the future.

For using GPS effectively as a long-term source of accurate time, it is therefore essential, that 
- the TimeCircuit's RTC (real time clock) is initially [set to correct local time](#how-to-set-the-real-time-clock-rtc), 
- the correct time zone is defined in the Config Portal,
- the GPS receiver has a battery
- and has been receiving data for 15-20 mins at least once a month.

If/as long as the GPS receiver has a fix and receives data from satellites, the dot in the present time's year field is lit.

In order to use the GPS receiver as a source of time, no special configuration is required. If it is detected during boot, it will be used.

For wiring information, see [here](#appendix-b-i2c-peripheral-wiring).
