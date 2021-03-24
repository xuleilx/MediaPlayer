#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H
#include <QThread>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
}
#include "AudioOutput.h"
class AudioPlayer: public QThread
{
    Q_OBJECT

public:
    AudioPlayer();
    ~AudioPlayer();
    int init(QString filepath);
    void stop()
    {
        mStart = false;
        mAudioOutput->stop();
    }

private:
    int init_converted_samples(uint8_t ***converted_input_samples,
                               AVCodecContext *output_codec_context,
                               int frame_size);
    int convert_samples(const uint8_t **input_data,
                        uint8_t **converted_data, const int frame_size,
                        SwrContext *resample_context);
    int decode(AVPacket *pPacket);
    int openCodecCtx();
    int init_swr();

signals:
    void sigGetOneFrame(const char *data, int len);

protected:
    void run();

private:
    AVFormatContext *mpFormatCtx;
    AVCodecContext *mpCodecCtx;
    struct SwrContext *swr_ctx;
    int mStreamIndex;
    enum AVMediaType mType;
    volatile bool mStart;

    AudioOutput *mAudioOutput;
    // output audio param
    enum AVSampleFormat  mFmt;
    int mSampleRate;
    int mChannels;
};

#endif // AUDIOPLAYER_H
