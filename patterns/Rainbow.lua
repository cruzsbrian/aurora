add_param("speed", 0, 1, 0.05, 0.15)
add_param("cycles", 1, 10, 1, 1)

start = 0

print(N_LEDS)

function update()
    for i=1,N_LEDS do
        h = ((i - start) * cycles / N_LEDS) % 1
        setHSV(i, h, 1, 0.25)
    end

    start = start + speed
end