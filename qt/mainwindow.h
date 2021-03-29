#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QStackedLayout>
// video
class VideoPlayer;
class DecLabel;
// audio
class AudioPlayer;

class VideoPage;
class ScalePage;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionPlay_video_triggered();

    void on_actionPlay_audio_triggered();

    void on_actionConvertPixelFormat_triggered();

private:
    Ui::MainWindow *ui;
    QStackedLayout *mpStackedLayout;
    // video
    VideoPlayer *mVideoPlayer;
    DecLabel *mVideoLabel;
    // audio
    AudioPlayer *mAudioPlayer;

    VideoPage *mpVideoPage;
    ScalePage *mpScalePage;
};

#endif // MAINWINDOW_H
