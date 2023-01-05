add_param("intensity", 0, 2, 0.1, 0.6)
add_param("max_hue", 0, 0.05, 0.001, 0.01)
add_param("speed", 0, 0.5, 0.01, 0.20)
add_param("thresh", 0, 0.1, 0.01, 0.03)


base = {}
a = math.random()
total = 0
for i=1,N_LEDS do
    base[i] = a
    total = total + a

    if (a <= 0.2) then
        a = a + math.random() * 0.1
    elseif (a >= 1) then
        a = a - math.random() * 0.1
    else
        a = a + math.random() * 0.2 - 0.1
    end
end

avg = total / N_LEDS
for i=1,N_LEDS do
    base[i] = 0.5 * base[i] / avg
    print(base[i])
end


function update()
    for i=1,N_LEDS do
        if (math.random() < base[i] * speed) then
            activity = base[i] * intensity
            brightness = math.random() * activity^2
            if (brightness > 1) then
                brightness = 1
            end

            if (brightness >= thresh) then
                setHSV(i, (base[i] + activity)^2 * max_hue, 1, brightness)
            else
                setHSV(i, 0, 0, 0)
            end
        end
    end
end