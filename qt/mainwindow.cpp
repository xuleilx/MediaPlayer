#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QStackedLayout>
#include <QtDebug>

#include "VideoPlayer.h"
#include "DecLabel.h"
#include "AudioPlayer.h"
#include "AudioOutput.h"
#include "VideoPage.h"
#include "ScalePage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mpStackedLayout = new QStackedLayout();
    mpVideoPage = new VideoPage();
    mpScalePage = new ScalePage();
    mpStackedLayout->addWidget(mpVideoPage);
    mpStackedLayout->addWidget(mpScalePage);

    mVideoPlayer = new VideoPlayer();
    mAudioPlayer = new AudioPlayer();

    connect(mVideoPlayer,SIGNAL(sigGetOneFrame(QImage)),mpVideoPage,SLOT(slotGetOneFrame(QImage)));

    ui->setupUi(this);
    centralWidget()->setLayout(mpStackedLayout);
}
void MainWindow::switchPage()
{
    qDebug()<<"Cliecked";
    int nCount = mpStackedLayout->count();
    int nIndex = mpStackedLayout->currentIndex();

    // 获取下一个需要显示的页面索引
    ++nIndex;

    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex >= nCount)
        nIndex = 0;

    mpStackedLayout->setCurrentIndex(nIndex);
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

    qDebug()<<"Layout Count: "<< mpStackedLayout->count()
           <<" current: "<< mpStackedLayout->currentIndex();

    mpStackedLayout->setCurrentIndex(0);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home/xuleilx/mywork/video",
                                                    tr("Media (*)"));
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

void MainWindow::on_actionConvertPixelFormat_triggered()
{
    qDebug()<<"Layout Count: "<< mpStackedLayout->count()
           <<" current: "<< mpStackedLayout->currentIndex();

    mpStackedLayout->setCurrentIndex(1);
}
