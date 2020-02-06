@ECHO OFF
ECHO [VTX] GENERATING VIDEO
ffmpeg -r 60 -f image2 -s 1280x720 -pattern_type glob -i *.jpg -vcodec libx264 -crf 15 -pix_fmt yuv420p video.mp4
ECHO [VTX] DONE
PAUSE