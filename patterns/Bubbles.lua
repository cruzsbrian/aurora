-- author("Brian Cruz")

min_hue_gap = 0.1
max_bubbles = 100

add_param("n_bubbles", 2, max_bubbles, 1, 30)
add_param("speed", 0, 0.5, 0.02, 0.1)
add_param("power", 0.1, 4, 0.1, 1.5)

hues = {}
hues[1] = math.random()
for i=2,max_bubbles do
    hues[i] = (hues[i-1] + min_hue_gap + math.random() * (1 - min_hue_gap * 2)) % 1
end

start = 0.0

function update()

    bubble_len = N_LEDS / n_bubbles
    fade_radius = bubble_len / 2

    for i=1,N_LEDS do
        bubble_pos = (i - start) % N_LEDS
        bubble_idx = math.floor(bubble_pos / bubble_len) + 1
        bubble_start = (bubble_idx - 1) * bubble_len
        bubble_end = bubble_start + bubble_len

        fade_frac = 1
        if (bubble_pos - bubble_start <= fade_radius) then
            fade_frac = ((bubble_pos - bubble_start) / fade_radius)^power
        elseif (bubble_end - bubble_pos <= fade_radius) then
            fade_frac = ((bubble_end - bubble_pos) / fade_radius)^power
        end

        -- print(bubble_idx, bubble_start, bubble_pos, bubble_end, fade_frac)

        setHSV(i, hues[bubble_idx], 1, fade_frac)
    end

    start = (start + speed) % N_LEDS
end