# CameraMod

![Front side of the camera](https://github.com/kongaskristjan/CameraMod/blob/master/hardware/front.jpg)

I bought a XIAOMI Mijia 160° dash cam for my car, but soon it became clear that there was an issue.
The camera should start recording once car starts supplying USB power,
but sometimes (perhaps with ~10% chance) the camera would fail to do so.
I've also failed to reproduce this indoors (it's probably temperature dependent),
so warranty repair was unlikely to help. So I decided to solve this problem myself.

Specifically, two factors made this tractable:
* An LED indicates whether the device is recording (white) or not (red)
* A failure to record can be solved with a simple restart (power off for a few seconds)

The LED is simply read with a covered photoresistor with a blue light filter and an analog to digital converter (built in the Arduino).
The restarts are handled with a relay controlled using a transistor.

The final electrical scheme was complicated by two main factors:
* Arduino power had to be swappable. While debugging, it's connected and powered from the computer's USB.
While working in the car, it's powered by the same 5V that goes to the camera. These two power sources
must not be connected, otherwise I risk damaging my computer. This lead to further cutting and soldering of USB cables.
* The 5V USB supply became 4.5V when the camera was on, which rendered the Arduino unstable.
Thus, Arduino power had to be regulated.

![Schema](https://github.com/kongaskristjan/CameraMod/blob/master/hardware/schema.svg)
