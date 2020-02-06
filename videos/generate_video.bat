@ECHO OFF
ECHO [VTX] GENERATING VIDEO
.\ffmpeg.exe -f image2 -framerate 60 -s 3840x2160 -i snapshot%04d.png -vcodec libx265 -crf 15 video.mp4
ECHO [VTX] DONE
PAUSE