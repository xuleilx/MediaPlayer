#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H
#include <QObject>
class QAudioOutput;
class QIODevice;

class AudioOutput:public QObject
{
    Q_OBJECT

public:
    AudioOutput();
    ~AudioOutput();

    int init();
    void processOneFrame(const char *data, int len);
    void stop()
    {
        mStart = false;
    }

private:
    volatile bool mStart;
    QAudioOutput *mAudioOutput;
    QIODevice *mStreamOut;
};

#endif // AUDIOOUTPUT_H
