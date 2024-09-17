function yuv2jpg()
{
ffmpeg.exe -f rawvideo -pixel_format yuyv422 -video_size 640x480 -i $1 $2 -y
}


function jobs()
{
	#convert to jpeg
	jpgdn=1
	while [ -e j$jpgdn ]
	do
		let "jpgdn++"
	done
	mkdir j$jpgdn
	i=$1
	((j=$i/100))
	((en=$2+1))
	while [ -e V$j/yuv$i.bin ] && [ $i -lt $en ]
	do
		if [ ! -e j$jpgdn/j$i.jpg ]
		then
		yuv2jpg V$j/yuv$i.bin j$jpgdn/j$i.jpg
		let "i++"
		((j=$i/100))
		fi
	done
	ffmpeg -start_number $1 -f image2 -i j$jpgdn/j%d.jpg  -vcodec libx264 -r 5 video$1.mp4
}



stafn=`cat stafn.txt`
endfn=`cat endfn.txt`
dirn=`cat dtidn.txt`
ns=$(expr $stafn + 0)
ne=$(expr $endfn + 0)
dn=1
if [ $ne -gt $ns ]
then
	jobs $ns $ne
else
	jobs $ns 9999
	jobs 0 $ne
fi
