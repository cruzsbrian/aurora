# Aurora
Aurora is an addressable LED controller with a graphical interface, audio input, and patterns programmed on an SD card.
It controls up to 8 WS2811-style LED strips in parallel.
A microphone and audio jack can be used for music visualization.
LED patterns are programmed in Lua and can be modified without rebooting by replacing the SD card.

[Interactive PCB BOM viewer](https://htmlpreview.github.io/?https://github.com/cruzsbrian/aurora/blob/main/aurora_hw/bom/ibom.html)

| <img src="/img/menu.jpg" height=100%> | <img src="/img/pattern.jpg" height=100%> |
| ----------------------------- | ----------------------------------- |

## Configuring LED Strips

`aurora_sw/src/lights.h` contains configuration for the number of strips and their lengths and directions.

`N_STRIPS` is the total number of strips (starting from the top connector).  
`LEDS_PER_STRIP` the maximum number of LEDs on a single strip.  
`N_LEDS` is the total number of LEDs (the sum of all the strip lengths, less than or equal to `N_STRIPS * LEDS_PER_STRIP`).  
`n_leds[N_STRIPS]` is an array of the strip lengths.  
`led_dirs[N_STRIPS]` is an array of the strip directions.
1 indicates the strip should start from its first LED, -1 indicates it should start from its last LED.

## Writing Patterns

When an SD card is inserted, Aurora interprets each Lua file in its root directory as a pattern.
The title of a pattern is its filename, minus the `.lua` extension.

Every pattern must define an `update()` function to display one frame of its animation, which will be called at 50 Hz.

Patterns can set LED colors with the `setRGB(i, r, g, b)` and `setHSV(i, h, s, v)` functions, where `i` is the index of the LED (starting at 1 to follow the Lua convention) and the other parameters specify the color (all values ranging from 0 to 1).
Patterns can also inspect the current color of an LED with `getRGB(i)`, which returns the red, green, and blue values from 0 to 1.

Calling `print(arg1, arg2, ...)` from Lua prints to the serial console.

A pattern can specify any number of "parameters" for itself, which appear as sliders in the UI, with `add_param(name, min, max, step, default)`, where `name` is a string, and `max`, `min`, `step`, `default` are floating point numbers.
The parameter will then be available as a global variable in the Lua environment for that pattern (so `name` must be a valid Lua variable name).

Here is an example pattern which displays a rainbow repeated a configurable number of times (set by `cycles`) moving at a configurable speed:
```Lua
add_param("speed", 0, 1, 0.05, 0.15)
add_param("cycles", 1, 10, 1, 1)

start = 0

function update()
    for i=1,N_LEDS do
        -- Calculate hue based on LED position, rainbow start position, and number of cycles.
        h = ((i - start) * cycles / N_LEDS) % 1
        
        -- Set LED to calculated hue, full saturation, 25% brightness.
        setHSV(i, h, 1, 0.25)
    end

    start = start + speed
end
```

# Compile / Upload:
## Site Customization: : 
Although you can run the Aurora firmware as-is, you will probably want to go in and customize the LED settings to match the specific layout of your installation. This customization is done in the aurora_sw/src/lights.h file as follows. Examples are included for a few different possible configurations. 

Important Parameters:
- *LED_BRIGHTNESS:* How bright should "full" brightness be. Higher values will provide more light and color depth, but will also increase current use and may introduce color aberration on longer strips when many LEDs are lit. A default value of 100 will work in most environments. 
- *N_STRIPS:* How many different led strips there are. In this context a strip is defined as a group of LEDs which all share one signal line. 
- *LEDS_PER_STRIP:* The maximum number of LEDs on any one strip.
- *N_LEDS:* The total number of LEDs, this should be the sum of "n_leds".
- *n_leds:* How many LEDs there are in each strip. For custom installations it may be nessary to count, or to upload debug patterns to precisely determine this number. Incorrect values with result in lost pixels. 
- *led_dirs:* Which direction each strip runs relative to the direction of signal. You can pick either clockwise or counter-clockwise as the logic direction, but it must remain consistent to prevent animation jumps. Clockwise is a good default. 
## Windows Setup: 
Write-up Coming Soon!

## Linux Setup: 
The Linux install instructions assume that you have Version 1.x.x of the Arduino IDE installed, along with teensyduino which is used to upload code to the Teensy. Instructions for installing teensyduino (and links to arduino installers) can be found here: https://www.pjrc.com/teensy/teensyduino.html. It is definitely worth checking that your install works by uploading one of the teensy example scripts before proceeding. 

Setting up the build environment is pretty straightforward, but it does involve mucking around in the arduino libraries folder. If you have any questions definitely feel free to reach out. 

Using the arduino IDE Install: 
(Sketch -> Include Library -> Manage Libraries...)
 - SD
 - TFT_eSPI
 - lvgl
 - arduinoFFT (for debugging, optional)
 
 Navigate to your a libraries folder (likely /home/[name]/Arduino/libraries) and clone the [arduinoLua](https://github.com/blackketter/LuaArduino) repo. 
 
 Now copy the lv_conf.h file from /aurora_sw/src/ to the top level of your Arduino libraries folder. This configures the GUI generator.
 
 In your arduino libraries folder, replace TFT_eSPI/User_Setup.h with the User_Setup.h file provided in aurora_sw/src/. This configures the pins and dimensions of the display itself. 
 
 Rename /aurora_sw/src/main.cpp to /aurora_sw/src/main.ino so that it will be detected by the Arduino IDE. 
 
 You can now open src.ino with the Arduino IDE. 
 
 Before uploading you will need to set the optimizer (tools -> optimize) to "Smallest Code".
 
 You are now ready to upload!
 
 # Hardware Setup / Assembly: 
 
 ## Tips:
 - The MAX9814 is the board's most fiddly component, so if you are going to use the microphone, it pays to solder that first.
 - The fft_debug script can be used to check if the audio components are
   correctly installed. To use it install the script, and then play tones from
   your phone. The tones played and the tones reported on the serial monitor 
   should match to within 1hz or so over 200hz - 4000hz. 
