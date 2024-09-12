
if [ -z $1 ]
then
	name="default"
else
	name=$1
fi
ffmpeg -f image2 -i yuv%d.jpg  -vcodec libx264 -r 10 $name.mp4
