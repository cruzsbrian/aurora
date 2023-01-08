-- author("Ray Sun")
-- This is an implementation of Fire2012 by Mark Kriegsman


add_param("brightness", 0.1, 1, 0.05, 1)
add_param("cooling", 0, 3000, 10, 100)     -- Cooling rate
add_param("max_base", 10, 200, 10, 80)     -- Highest LED which can spark

-- Archways (inclusive):
-- This is with the 6 broken LEDs at ~ index 600 or so, so in the future,
-- add 6 to the closer values
--      closer: 889 - 911 - 931
--      farther: 286 - 304 - 322
led_idx_farther = 304;      -- Index of ~ center of archway at end of Hell
led_idx_closer  = 911;      -- Index of ~ center of archway at start of Hell

-- Reference frame:
--     'farther' is you looking down Hell from the controller's location
--     'left' is always to your left when looking from 'closer' to 'farther'

heat_fl = {}
heat_fr = {}
heat_cr = {}
heat_cl = {}
arr_size = math.ceil(N_LEDS/4) + 10      -- Bigger than needed
for i=1,arr_size do
    heat_fl[i] = 0
    heat_fr[i] = 0
    heat_cr[i] = 0
    heat_cl[i] = 0
end

function update()
    -- Step 1.  Cool down every cell a little
    for i = 1,arr_size do
        heat_fl[i] = heat_fl[i] - (math.random() * cooling / arr_size)
        heat_fr[i] = heat_fr[i] - (math.random() * cooling / arr_size)
        heat_cr[i] = heat_cr[i] - (math.random() * cooling / arr_size)
        heat_cl[i] = heat_cl[i] - (math.random() * cooling / arr_size)
    end

    -- Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for k = arr_size, 3, -1 do
        heat_fl[k] = (heat_fl[k - 1] + heat_fl[k - 2] + heat_fl[k - 2]) / 3
        heat_fr[k] = (heat_fr[k - 1] + heat_fr[k - 2] + heat_fr[k - 2]) / 3
        heat_cr[k] = (heat_cr[k - 1] + heat_cr[k - 2] + heat_cr[k - 2]) / 3
        heat_cl[k] = (heat_cl[k - 1] + heat_cl[k - 2] + heat_cl[k - 2]) / 3
    end

    -- Step 3.  Randomly ignite new 'sparks' near the bottom
    y = math.random(1, max_base)
    heat_fl[y] = heat_fl[y] + math.random(160,255)
    y = math.random(1, max_base)
    heat_fr[y] = heat_fr[y] + math.random(160,255)
    y = math.random(1, max_base)
    heat_cr[y] = heat_cr[y] + math.random(160,255)
    y = math.random(1, max_base)
    heat_cl[y] = heat_cl[y] + math.random(160,255)

    for i = 1,arr_size do
        if heat_fl[i] > 255 then
            heat_fl[i] = 255
        end
        if heat_fr[i] > 255 then
            heat_fr[i] = 255
        end
        if heat_cr[i] > 255 then
            heat_cr[i] = 255
        end
        if heat_cl[i] > 255 then
            heat_cl[i] = 255
        end
    end

    -- Step 4.  Convert heat to LED colors
    -- Update only one quadrant of the installation each loop, starting from 
    -- physical LED index zero (~ left middle, going farther away)

    -- Map into three colors: white, yellow, red
    -- Lower than led_idx_closer is far end of left side of Hell
    for j = 1,led_idx_farther-1 do
        if heat_fl[led_idx_farther-j] > 128 then -- hottest
            setRGB(j, brightness, brightness, brightness*heat_fl[led_idx_farther - j]/255)
        elseif heat_fl[led_idx_farther-j] > 64 then -- middle
            setRGB(j, brightness, brightness*heat_fl[led_idx_farther-j]/255, 0)
        else 
            setRGB(j, brightness*heat_fl[led_idx_farther-j]/255, 0, 0)
        end
    end

    -- If greater than led_idx_farther but less than halfway to 
    -- led_idx_closer, is far right side
    for j = led_idx_farther+1, math.floor((led_idx_farther + led_idx_closer)/2) do
        if heat_fr[j-led_idx_farther] > 128 then -- hottest
            setRGB(j, brightness, brightness, brightness*heat_fr[j-led_idx_farther]/255)
        elseif heat_fr[j-led_idx_farther] > 64 then -- middle
            setRGB(j, brightness, brightness*heat_fr[j-led_idx_farther]/255, 0)
        else 
            setRGB(j, brightness*heat_fr[j-led_idx_farther]/255, 0, 0)
        end
    end
    -- If greater than led_idx_farther and more than halfway to closer,
    -- is close right side

    for j = math.floor((led_idx_farther + led_idx_closer)/2)+1, led_idx_closer-1 do
        if heat_cr[led_idx_closer-j] > 128 then -- hottest
            setRGB(j, brightness, brightness, brightness*heat_cr[led_idx_closer-j]/255)
        elseif heat_cr[led_idx_closer-j] > 64 then -- middle
            setRGB(j, brightness, brightness*heat_cr[led_idx_closer-j]/255, 0)
        else 
            setRGB(j, brightness*heat_cr[led_idx_closer-j]/255, 0, 0)
        end
    end
    -- If greater than led_idx_closer, is close left side
    for j = led_idx_closer+1, N_LEDS do
        if heat_cl[j-led_idx_closer] > 128 then -- hottest
            setRGB(j, brightness, brightness, brightness*heat_cl[j-led_idx_closer]/255)
        elseif heat_cl[j-led_idx_closer] > 64 then -- middle
            setRGB(j, brightness, brightness*heat_cl[j-led_idx_closer]/255, 0)
        else 
            setRGB(j, brightness*heat_cl[j-led_idx_closer]/255, 0, 0)
        end
    end
end
