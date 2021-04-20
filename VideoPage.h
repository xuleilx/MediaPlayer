#ifndef VIDEOPAGE_H
#define VIDEOPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class DecLabel;
class VideoPage : public QWidget
{
    Q_OBJECT

public:
    VideoPage();
    ~VideoPage();
public slots:
    void slotGetOneFrame(QImage img);

private:
    QVBoxLayout *mLayout;
    DecLabel *mVideoLabel;
};

#endif // VIDEOPAGE_H
