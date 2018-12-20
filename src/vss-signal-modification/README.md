GC8 VSS signal modification
===========================

## Links
- http://rjes.com/html/vss.html
- https://www.iwireservices.com/single-post/2018/03/21/Subaru-Vehicle-Speed-Sensor-Types
- http://www.enduringsolutions.com/ECU978pinout.pdf
- GC8インプレッサWRX&STi ver. (オーナーズバイブルシリーズ)
- http://forums.haltech.com/viewtopic.php?t=7395

## Explanations

Analysis of the ECU's code and track driving showed that the speed limiter was
triggered when the car reached 186km/h. Further analysis of the code showed
that no other parameter seemed to be used when triggering the limiter, and that
the speed was directly read from the speed sensor when used by the ECU. While
the speed is used at some other moments by the ECU (see
`../ecu-dump-and-reverse/reverse-notes.txt`), disconnecting the speed sensor
didn't prevent the engine from running (a bit roughly, but running).

The different options for getting rid of the speed limit were, in the order they
were thought of:
- Modify the ECU's firmware (requires a daughterboard on top of the motherboard)
- Disconnect the speed sensor when going on the track
- Fool the speed sensor's signal

While the two first ideas presented a lot of downsides (no speed information
for neither the engine nor me and rough engine runs when disconnecting the
sensor) or difficulties (creating a daughterboard to reflash the firmware), the
last one was pretty easy to implement and could still allow speed information
to be sent to either the engine or my infotainement.
Reading different sources (like the GC8's owner bible, see Links section)
and `ESL's ECU978pinout PDF`, I found the wire responsible for carrying the
signal to the ECU (middle connector - B135, terminal 3). This signal is an
analog signal that switches between 0 and 5v following the speed of the car.
The speed is determined by the number of edges in a given amount of time (see
`../ecu-dump-and-reverse/reverse-notes.txt#fun_6863`). Cutting this wire, and
adding a Teensy 3.2 in between (with a double level-shifter, as the Teensy
usually works with 3.3v) did the trick in succesfully monitoring the signal.
Having the teensy's LED blink at each edge was the simplest way to get a direct
visual representation of the speed, to determine the number of edges in
function of the speed (see "Understanding the signal" section below). The
second step would be to modify the signal when a speed of 180km/h or more is
reached only. This prevents strange behavior of the engine from happening and
allows accurate reading of the speed on my infotainement system at "low" speed,
my ultimate goal being to see how fast the car can really go.

### Understanding the signal

I first thought that the speed was understood by the ECU as the number of
positive edges the signal made over a fixed amount of edges. Driving with the
Teensy blinking revealed that the ECU is in fact counting the edges, whether
they are positive or negative: this is due to the fact that the sensor is a
wheel with X teeth (4 it seems here) and that the output of the signal is wether
we are on a tooth at a given moment (if we stop the car when then sensor reads
a tooth, positive value, the LED will stay on on the teensy as the value will
stay at 1, or 5v here).

Determining the number of edges read in one kilometer could was required. I
first tried a known formula that didn't give any correct result, so I drove 100
meters and monitored the number of edges to get a rough value. The result is
500edges/100m, so 5000edges/km

Here is the (invalid) computation:
```
> E/Km = 1000 (meters in 1km) / ((pi * diameter of the wheel) / number of teeth on sensor)
diameter of the wheel is D
The tires are 205/55R16 91V, which mean we have 16" (0.4064m) wheels, and two
times 0.55 * 205mm.
> D(inches) = 8.070 * 0.55 * 2 + 16 = 24,877 inches = 0.6318758m
> E/Km = 1000 / ((pi * D) / 4)
>      = 1000 / ((3.14159 * 0.6319) / 4)
>      ~= 2014 edges/km (incorrect)
```

### Modifying the signal

This was far easier than I thought it would be. I made some test with my
infotainement (which is connected to the ECU via SSM1, and thus displays the
speed as read by the ECU) and the real cluster giving the real speed. I was
able to make it print 140km/h while not driving by just sending a signal that
edge'd once every 5 milliseconds approximately. I am able to keep the real
speed in the teensy at all time, and thus am able to send the real speed when
it is less than 180, or just my `delay(5)` loop when it is over, to avoid
triggering the limiter.

TODO: track tests
