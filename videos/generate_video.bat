@ECHO OFF
ECHO [VTX] GENERATING VIDEO
.\ffmpeg.exe -f image2 -framerate 60 -i snapshot%04d.png -vcodec -s 1920x1080 -crf 10 libx264 video.mp4
ECHO [VTX] DONE
PAUSE