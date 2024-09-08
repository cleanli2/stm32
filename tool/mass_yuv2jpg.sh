
function yuv2jpg()
{
    ffmpeg.exe -f rawvideo -pixel_format yuyv422 -video_size 640x480 -i $1 $2 -y
}

i=0
while true
do
	((j=$i/100))
	echo $j
	if [ -f V$j/yuv$i.bin ]
	then
		echo yuv$i.bin exist
		./yuv2jpg.sh V$j/yuv$i.bin yuv$i.jpg
	else
		echo yuv$i.bin not exist
		exit
	fi
	let "i++"
done
