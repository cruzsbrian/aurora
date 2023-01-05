add_param("hue", 0, 1, 0.01, 0)
add_param("saturation", 0, 1, 0.01, 1)
add_param("value", 0, 1, 0.01, 0.25)


function update()
    for i=1,N_LEDS do
        setHSV(i, hue, saturation, value)
    end
end