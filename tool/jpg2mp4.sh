ffmpeg -f image2 -i yuv%d.jpg  -vcodec libx264 -r 10 $1.mp4
