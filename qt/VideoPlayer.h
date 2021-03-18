#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
#include <QString>
#include <QThread>
#include <QImage>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
}


class VideoPlayer:public QThread
{
    Q_OBJECT
    enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB32;
public:
    VideoPlayer();
    int init(QString filepath);
    void stop()
    {
        mStart = false;
    }
private:
    int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame);

signals:
    void sigGetOneFrame(QImage img);

protected:
    void run();

private:
    volatile bool mStart;
    AVFormatContext *mpFormatContext;
    AVCodec *mpCodec;
    AVCodecParameters *mpCodecParameters;
    struct SwsContext *mSwsCtx;
    int video_stream_index;

};

#endif // VIDEOPLAYER_H
