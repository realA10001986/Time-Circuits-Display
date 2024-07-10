# Time Circuits Wall Clock

Here are a few photos of my custom Time Circuits Wall Clock.

Final result:

![IMG_9612](https://user-images.githubusercontent.com/76924199/204099156-5617a33b-9a9d-4155-98ab-da33364bfaf1.jpg)

![IMG_9613](https://user-images.githubusercontent.com/76924199/204099461-6499d8a4-4c8f-446c-b0a8-7ef13b558c67.jpg)

The clock replaced a failing QLOCKTWO so I made it the exact same size, 40x40cm (15.75x15.75in). Total depth (wall to front surface) is 4.5cm (1.77in), the outer frame's depth is 2cm (0.8in) in total.

The body is made from wood; the front is a 6.5mm (0.25in) plywood panel, the rest is made from birch, resulting in very low weight.

![processed-40f4864a-b98b-482a-9e27-5b484d93d226_LMPGnUEC](https://user-images.githubusercontent.com/76924199/204100106-654882a2-c7f1-4d9e-90fc-3248b95ae4be.jpg)
![processed-84ba7b18-83d1-4b46-9e31-6df70d2fc600_1shrtVM7](https://user-images.githubusercontent.com/76924199/204100108-aeffe268-9d24-4bb2-9c40-7f77790ac6b1.jpg)
![processed-4875de71-211f-466e-ad6c-ee0993443041_foT9rLNA](https://user-images.githubusercontent.com/76924199/204100109-c0c827e6-13a7-4ba6-b7fe-b912e9db19b6.jpg)

![IMG_9607](https://user-images.githubusercontent.com/76924199/204100112-bcaf038d-556c-4e3d-8af1-829a8794cbef.jpg)

![IMG_9606](https://user-images.githubusercontent.com/76924199/204100117-0130b7cb-fb35-4d62-80e9-6c9a6b4a7c7d.jpg)

The wood work was done by a professional. The final steps were to paint the entire body white, and to put up a photo printed on adhesive film on the front.

As for the 3D-printed parts: The three inner enclosures were glued together using industrial duct tape, with an offset of about 0.5cm (0.2in) to limit the clock's depth. On the back, I cut out some space for the control board; be careful with the cutter, the material breaks very easily along the print layers. The whole thing as well as the face plates were spray painted using Rust-Oleum 271481 Universal All Surface Spray Paint, Forged Hammered Antique Pewter.

![processed-bc73fc63-353f-4a4d-8bca-9e554d9c6c18_i36KguoN](https://user-images.githubusercontent.com/76924199/204100473-c9e49008-bbae-47dd-92e0-0d3a165799e0.jpg)

The clock has an additional speedo display top left, a temperature and a light sensor. The speedo display is an [Adafruit 14-segment LED Alphanumeric Backpack](https://www.adafruit.com/product/1911) (1911) with only one tube (two segments) soldered on; the temperature sensor is an [Adafruit MCP9808](https://www.adafruit.com/product/1782) (1782), and the light sensor is an [Adafruit VEML7700](https://www.adafruit.com/product/4162) (4162).

![IMG_9624](https://user-images.githubusercontent.com/76924199/204110092-ad91ce4b-b1c7-4c77-a561-90c64fdc7aaf.jpg)

![IMG_9622](https://user-images.githubusercontent.com/76924199/204110099-ea7f4b59-1512-4949-a1ac-5d692a9f9d67.jpg)

There are three i2c breakouts on the control board, and they are used as follows:

- the three displays are connected in parallel to a "hub" with a cable to the control board's first i2c socket;
- the temperature and light sensors are daisy-chained, and the cable does not exceed 20cm (8in) from the control board's second i2c socket to the last sensor;
- the speedo display is on the third i2c socket on the control board, cable length is about 20cm (8in).

The Enter key is made of a small PCB with a computer keyboard switch soldered on, and a key cap on top. The white LED is housed in a Signal Construct SMK1089 LED holder.

The control board's other LEDs were removed, as was the volume pot; it would be inaccessible anyway.

_Text & images: (C) Thomas Winischhofer ("A10001986"). See LICENSE._
