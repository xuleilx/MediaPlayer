#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include "VideoPlayer.h"
#include "DecLabel.h"
#include "AudioPlayer.h"
#include "AudioOutput.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mVideoPlayer = new VideoPlayer();
    mVideoLabel = new DecLabel();
    mAudioPlayer = new AudioPlayer();

    connect(mVideoPlayer,SIGNAL(sigGetOneFrame(QImage)),mVideoLabel,SLOT(slotGetOneFrame(QImage)));

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event){
    qDebug()<<"closeEvent";
    mVideoPlayer->stop();
    mAudioPlayer->stop();
}

void MainWindow::on_actionPlay_video_triggered()
{
    mVideoPlayer->stop();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home/xuleilx/mywork/video",
                                                    tr("Media (*)"));
    ui->gridLayout->addWidget(mVideoLabel);
    if(mVideoPlayer->init(fileName) == 0){
        mVideoPlayer->start();
    }
}

void MainWindow::on_actionPlay_audio_triggered()
{
    mAudioPlayer->stop();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home/xuleilx/mywork/audio",
                                                    tr("Media (*)"));
    if(mAudioPlayer->init(fileName) == 0){
        mAudioPlayer->start();
    }
}
