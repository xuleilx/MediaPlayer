#ifndef VIDEOSCALE_H
#define VIDEOSCALE_H
#include<QString>
#include<QFile>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
}

class VideoScale:public QObject
{
    Q_OBJECT

public:
    VideoScale();
    ~VideoScale();
    int setup(QString inFile,QString outFile);
    int setScale(int width,int height,enum AVPixelFormat format);
    int doScale();
    int tearDown();
    inline void stop()
    {
        mStart = false;
    }
    inline int getWidth(){
        return mWidth;
    }
    inline int getHeight(){
        return mHeight;
    }

private:
    int openInputFile(QString inFile);
    int openOutputFile(QString outFile);
    void setDstParam(int dst_w, int dst_h,enum AVPixelFormat dst_pix_fmt);
    int decodePacket(AVPacket *pPacket,AVFrame *pFrame);

private:
    AVFormatContext *mpInputFormatContext;
    AVCodecContext *mpInputCodecContext;
    struct SwsContext *mpSwsCtx;
    int mStreamIndex;

    QFile *mpOutFile;
    volatile bool mStart;

    int mWidth;
    int mHeight;
    enum AVPixelFormat mFormat;

    uint8_t *dst_data[4];
    int  dst_linesize[4];
};

#endif // VIDEOSCALE_H
