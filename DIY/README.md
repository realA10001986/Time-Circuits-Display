
# DIY additions for the Time Circuits Display

## Light sensor

The following sensor types/models are supported: TSL2591, TSL2561, BH1750, VEML7700/VEML6030, LTR303/LTR329, with their respective default address. The VEML7700 can only be connected if no CircuitSetup Speedo or third-party GPS receiver is connected at the same time; the VEML6030 needs its address to be set to  0x48 if a CircuitSetup Speedo or third party GPS receiver is present at the same time. All these sensor types are readily available on breakout boards from Adafruit or Seeed (Grove). Only one light sensor can be used at the same time.

For wiring information, see [here](#appendix-b-i2c-peripheral-wiring).

>*Note: You cannot connect the sensor chip directly to the TCD control board; most sensors need at least a voltage converter/level-shifter.*  It is recommended to use Adafruit or Seeed breakouts ([TSL2591](https://www.adafruit.com/product/1980), [TSL2651](https://www.seeedstudio.com/Grove-Digital-Light-Sensor-TSL2561.html), [BH1750](https://www.adafruit.com/product/4681), [VEML7700](https://www.adafruit.com/product/4162), [LTR303](https://www.adafruit.com/product/5610)), which all allow connecting named sensors to the 5V the TCD board operates on. 

## Speedometer

| [![Watch the video](https://img.youtube.com/vi/opAZugb_W1Q/0.jpg)](https://youtu.be/opAZugb_W1Q) |
|:--:|
| Click to watch the video |

The speedo display shown in this video is based on a fairly well-designed stand-alone replica I purchased on ebay. I removed the electronics inside and wired the LED segments to an Adafruit i2c backpack (from the Adafruit 878 product) and connected it to the TCD. Yes, it is really that simple. 

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

## Rotary Encoder

A rotary encoder is, simply put, a turnable knob. On the TCD, rotary encoders can be used for speed and/or audio volume.

The firmware currently supports the [Adafruit 4991](https://www.adafruit.com/product/4991), [DFRobot Gravity 360](https://www.dfrobot.com/product-2575.html) and [DuPPA I2CEncoder 2.1](https://www.duppa.net/shop/i2cencoder-v2-1/) (or [here](https://www.tindie.com/products/saimon/i2cencoder-v21-connect-rotary-encoder-on-i2c-bus/)) i2c rotary encoders; a CircuitSetup original prop is in the works. For the Adafruit and the DuPPa, I recommend buying the PCBs without an actual encoder and soldering on a Bourns PEC11R-42xxy-S0024.

For wiring information, see [here](#appendix-b-i2c-peripheral-wiring). Up to two rotary encoders can be connected, one for speed, one for volume.

### Rotary Encoder for Speed 

The rotary encoder, if configured for speed, allows manually selecting a speed to be displayed on the Speedo display, as well as to be sent to [BTTFN](#bttf-network-bttfn) clients in place of actual (GPS) speed.

| [![Watch the video](https://img.youtube.com/vi/Y6uu1SU6YJA/0.jpg)](https://youtu.be/Y6uu1SU6YJA) |
|:--:|
| Click to watch the video |

### Rotary Encoder for Audio Volume

The rotary encoder for volume replaces the volume knob on back of the TCD's keypad. The advantages of the rotary encoder are that it is more precise, especially at lower volume levels, and it can be relocated. 

### Hardware Configuration

In order to use an encoder for speed or volume, it needs to be configured as follows:

  <table>
  <tr><td></td><td>Ada4991</td><td>DFRobot</td><td>DuPPA</td></tr>
  <tr><td>Speed</td><td>Default [0x36]</td><td>SW1=0,SW2=0 [0x54]</td><td>A0 closed [0x01]</td></tr>
  <tr><td>Volume</td><td>A0 closed [0x37]</td><td>SW1=0,SW2=1 [0x55]</td><td>A0,A1 closed [0x03]</td></tr>
  </table>

The numbers in brackets are the resulting i2c address. (For DuPPA: RGB-encoders not supported.)

Here is how they look configured for speed (the purple spots are solder joints):

![RotEncSpd](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/ae4ee45b-5cbf-45e1-9092-1043367e9af5)

Here is the configuration for volume:

![RotEncVol](https://github.com/realA10001986/Time-Circuits-Display/assets/76924199/a13a02ed-ab5b-42f6-9160-96070e1d5a17)

## Temperature/humidity sensor

The firmware supports connecting a temperature/humidity sensor for "room condition mode"; in this mode, *destination* and *last departed* times are replaced by temperature and humidity (if applicable), respectively. To toggle between normal and room condition mode, enter "111" and press ENTER. 

![rcmode](https://user-images.githubusercontent.com/76924199/208133653-f0fb0a38-51e4-4436-9506-d841ef1bfa6c.jpg)

#### Sensor hardware

The following sensor types are supported: MCP9808 (address 0x18), BMx280 (0x77), SI7021, SHT40/SHT45 (0x44), TMP117 (0x49), AHT20/AM2315C, HTU31D (0x41), MS8607. All of those are readily available on breakout boards from Adafruit or Seeed (Grove). The BMP280 (unlike BME280), MCP9808 and TMP117 work as pure temperature sensors, the others for temperature and humidity. For wiring information, see [here](#appendix-b-i2c-peripheral-wiring).

>Note: You cannot connect the sensor chip directly to the TCD control board; most sensors need at least a voltage converter/level-shifter.* It is recommended to use Adafruit or Seeed breakouts ([MCP9808](https://www.adafruit.com/product/1782), [BME280](https://www.adafruit.com/product/2652), [SI7021](https://www.adafruit.com/product/3251), [SHT40](https://www.adafruit.com/product/4885), [SHT45](https://www.adafruit.com/product/5665), [TMP117](https://www.adafruit.com/product/4821), [AHT20](https://www.adafruit.com/product/4566), [HTU31D](https://www.adafruit.com/product/4832), [MS8607](https://www.adafruit.com/product/4716)), which all allow connecting named sensors to the 5V the TCD board operates on.


