# MediaPlayer
该项目主要用于学习FFMpeg
## 功能描述
### 播放视频
1. 使用FFMpeg解码
2. 使用sws_scale将FFMpeg解码后的yuv数据转换成rgb
3. 使用QT的QLabel组件，通过QImage显示rgb数据
### 播放音频
1. 使用FFMpeg解码
2. 使用swr_convert将FFMpeg解码后的一帧数据转换成播放器指定的播放参数
3. 使用QT的QAudioOutput播放pcm数据
