@ECHO OFF
ECHO [VTX] GENERATING VIDEO
ffmpeg.exe -f image2 -framerate 60 -s 1280x720 -i snapshot%04d.png -vcodec libx264 -crf 25 video.mp4
ECHO [VTX] DONE
PAUSE