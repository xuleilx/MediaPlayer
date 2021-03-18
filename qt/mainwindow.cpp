#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QLabel>
#include <QImage>
#include "VideoPlayer.h"
#include "DecLabel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mVideoPlayer = new VideoPlayer();
    mVideoLabel = new DecLabel();
    connect(mVideoPlayer,SIGNAL(sigGetOneFrame(QImage)),mVideoLabel,SLOT(slotGetOneFrame(QImage)));

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_file_triggered()
{
    mVideoPlayer->stop();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home",
                                                    tr("Media (*)"));
    ui->gridLayout->addWidget(mVideoLabel);
    if(mVideoPlayer->init(fileName) == 0){
        mVideoPlayer->start();
    }
}
