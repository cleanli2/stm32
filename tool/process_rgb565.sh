function yuv2jpg()
{
ffmpeg.exe -f rawvideo -pixel_format rgb565be -video_size 640x480 -i $1 $2 -y
}


function jobs()
{
	#cp files
	i=$1
	((j=$i/100))
	((i=$j*100))
	((en=$2/100 + 1))
	while [ -e V$j/yuv$i.bin ] && [ $j -lt $en ]
	do
		echo cp -r $3/V$j $4
		cp -r $3/V$j $4
		((i=$i+100))
		let "j++"
	done

	#convert to jpeg
	jpgdn=1
	while [ -e $4/j$jpgdn ]
	do
		let "jpgdn++"
	done
	mkdir $4/j$jpgdn
	i=$1
	((j=$i/100))
	((en=$2+1))
	while [ -e V$j/yuv$i.bin ] && [ $i -lt $en ]
	do
		yuv2jpg V$j/yuv$i.bin $4/j$jpgdn/j$i.jpg
		let "i++"
		((j=$i/100))
	done
	ffmpeg -start_number $1 -f image2 -i $4/j$jpgdn/j%d.jpg  -vcodec libx264 -r 5 $4/video$1.mp4
}



stafn=`cat stafn.txt`
endfn=`cat endfn.txt`
dirn=`cat dtidn.txt`
ns=$(expr $stafn + 0)
ne=$(expr $endfn + 0)
echo start=$ns
echo end=$ne
dn=1
while [ -e /d/stm32_cam_data/$dirn$dn ]
do
	let "dn++"
done
echo mkdir /d/stm32_cam_data/$dirn$dn
mkdir /d/stm32_cam_data/$dirn$dn
if [ $ne -gt $ns ]
then
	jobs $ns $ne $PWD /d/stm32_cam_data/$dirn$dn
else
	jobs $ns 9999 $PWD /d/stm32_cam_data/$dirn$dn
	jobs 0 $ne $PWD /d/stm32_cam_data/$dirn$dn
fi
cp $PWD/*fn.txt /d/stm32_cam_data/$dirn$dn
cp $PWD/stmlog.txt /d/stm32_cam_data/$dirn$dn
cp $PWD/process.sh /d/stm32_cam_data/$dirn$dn
