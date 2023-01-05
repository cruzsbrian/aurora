-- author("Brian Cruz")

add_param("spawn_rate", -5, -2.2, 0.1, -3.3)
add_param("radius", 1, 10, 1, 4)
add_param("intertia", 0, 0.95, 0.05, 0.5)
add_param("decay", 0, 10, 0.5, 2)

function update()
    for i=1,N_LEDS do
        if (math.random() < 0.5) then
            r_sum = 0
            g_sum = 0
            b_sum = 0

            -- Sum the colors of all the pixels within the radius to find average
            for j=-radius,radius do
                led_idx = (i + j + N_LEDS) % N_LEDS
                r, g, b = getRGB(led_idx)
                r_sum = r_sum + r
                g_sum = g_sum + g
                b_sum = b_sum + b
            end

            -- Average, and divide by 1 more than the number of pixels averaged
            -- so the color fades out gradually.
            r_avg = r_sum / (radius * 2 + 1 + decay)
            g_avg = g_sum / (radius * 2 + 1 + decay)
            b_avg = b_sum / (radius * 2 + 1 + decay)

            r, g, b = getRGB(i)

            new_r = r * intertia + r_avg * (1 - intertia)
            new_g = g * intertia + g_avg * (1 - intertia)
            new_b = b * intertia + b_avg * (1 - intertia)

            -- setRGB(i, r_avg, g_avg, b_avg)
            setRGB(i, new_r, new_g, new_b)
        end

        -- Randomly spawn new splashes.
        if (math.random() < 10^spawn_rate) then
            -- leds[i] = HSV(math.random(), 1, 1)
            setHSV(i, math.random(), 1, 1)
        end
    end
end