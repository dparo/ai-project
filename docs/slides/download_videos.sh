#!/bin/bash

# Downloads Requested video for presentation

mkdir -p ./videos
youtube-dl https://www.youtube.com/watch?v=S530Vwa33G0
ffmpeg -ss 00:01:13.0 -i "./Like in a dream - 3D fractal trip-S530Vwa33G0.webm" -t 00:00:22.0 -c copy ./videos/fractal.webm
rm "./Like in a dream - 3D fractal trip-S530Vwa33G0.webm"
