#include "AudioOutput.h"
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QDebug>
#include <QFile>

AudioOutput::AudioOutput()
{
    mAudioOutput = NULL;
    mStreamOut = NULL;
}
AudioOutput::~AudioOutput(){
    if(mAudioOutput) delete mAudioOutput;
    if(mStreamOut) delete mStreamOut;
}

int AudioOutput::init(){

    //设置采样格式.
    QAudioFormat audioFormat;
    //设置采样率.
    audioFormat.setSampleRate(44100);
    //设置通道数.
    audioFormat.setChannelCount(2);
    //设置采样大小，一般为8位或16位.
    audioFormat.setSampleSize(16);
    //设置编码方式.
    audioFormat.setCodec("audio/pcm");
    //设置字节序.
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    //设置样本数据类型.
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    //音频设备信息.
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(audioFormat)) {
        qDebug()<<"default format not supported try to use nearest";
        audioFormat = info.nearestFormat(audioFormat);
    }
    mAudioOutput = new QAudioOutput(audioFormat, this);
    mStreamOut = mAudioOutput->start();
}

void AudioOutput::processOneFrame(const char *data, int len){
    int nleft = len;
    int nwritten = 0;
    const char *ptr = data;
    mStart = true;

    while (mStart && nleft > 0) {
        if ((nwritten = mStreamOut->write(ptr,nleft)) < 0) {
            qDebug("writen error:%d",errno);
            break;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
}
