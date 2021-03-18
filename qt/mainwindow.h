#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
class VideoPlayer;
class DecLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_file_triggered();

private:
    Ui::MainWindow *ui;
    VideoPlayer *mVideoPlayer;
    DecLabel *mVideoLabel;
};

#endif // MAINWINDOW_H
