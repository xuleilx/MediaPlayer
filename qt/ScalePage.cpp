#include "ScalePage.h"
#include <QFileDialog>
#include <QThread>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
}
#include "VideoScale.h"

ScalePage::ScalePage()
{
    mLayout = new QGridLayout(this);

    QLabel *pFileLabel = new QLabel("输入文件: ");
    mpFileText = new QLineEdit();
    QPushButton *pFileBtn = new QPushButton("打开");
    mLayout->addWidget(pFileLabel,0,0);
    mLayout->addWidget(mpFileText,0,1);
    mLayout->addWidget(pFileBtn,0,2);
    connect(pFileBtn, &QPushButton::clicked, this, &ScalePage::inputFile);

    QLabel *pFileLabelOut = new QLabel("输出文件: ");
    mpFileTextOut = new QLineEdit();
    QPushButton *pFileOutBtn = new QPushButton("打开");
    mLayout->addWidget(pFileLabelOut,1,0);
    mLayout->addWidget(mpFileTextOut,1,1);
    mLayout->addWidget(pFileOutBtn,1,2);
    connect(pFileOutBtn, &QPushButton::clicked, this, &ScalePage::outputFile);

    QLabel *pWidth = new QLabel("长: ");
    mpWidthText = new QLineEdit();
    mpWidthText->setValidator(new QIntValidator(1, 9999, this));
    mLayout->addWidget(pWidth,2,0);
    mLayout->addWidget(mpWidthText,2,1);

    QLabel *pHeight = new QLabel("宽: ");
    mpHeightText = new QLineEdit();
    mpHeightText->setValidator(new QIntValidator(1, 9999, this));
    mLayout->addWidget(pHeight,3,0);
    mLayout->addWidget(mpHeightText,3,1);

    QLabel *pPixFmt = new QLabel("像素格式: ");
    mpPixFmtCmbBox = new QComboBox();
    //QMap自动根据 key排序
    QMap<QString, int> formatMap;
    formatMap.insert("RGB24",AV_PIX_FMT_RGB24);
    formatMap.insert("RGB32",AV_PIX_FMT_RGB32);
    formatMap.insert("YUV420P",AV_PIX_FMT_YUV420P);
    formatMap.insert("YUV422P",AV_PIX_FMT_YUV422P);
    formatMap.insert("YUV444P",AV_PIX_FMT_YUV444P);
    formatMap.insert("YUYV422",AV_PIX_FMT_YUYV422);
    mpPixFmtCmbBox->clear();
    foreach(const QString &str,formatMap.keys())
        mpPixFmtCmbBox->addItem(str,formatMap.value(str));
    mLayout->addWidget(pPixFmt,4,0);
    mLayout->addWidget(mpPixFmtCmbBox,4,1);

    pRunBtn = new QPushButton("Start");
    mLayout->addWidget(pRunBtn,5,2);
    connect(pRunBtn, &QPushButton::clicked, this, &ScalePage::run);

    QPushButton *pReset = new QPushButton("Reset");
    mLayout->addWidget(pReset,6,2);
    connect(pReset, &QPushButton::clicked, this, &ScalePage::reset);

    mpVideoScale = new VideoScale;
}
void ScalePage::inputFile(){
    QString inputFileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "/home/xuleilx/mywork/video",
                                                         tr("Media (*)"));
    mpFileText->setText(inputFileName);
}

void ScalePage::outputFile(){
    QString outputFileName =  QFileDialog::getSaveFileName(this, tr("Save File"),
                                                           "/home/xuleilx/mywork/",
                                                           tr("RAW (*)"));
    mpFileTextOut->setText(outputFileName);
}

void ScalePage::run(){
    qDebug()<<mpFileText->text();
    qDebug()<<mpWidthText->text().toInt();
    qDebug()<<mpHeightText->text().toInt();
    qDebug()<<mpPixFmtCmbBox->currentData().toInt();
    qDebug()<<mpFileTextOut->text();

    if(mpFileText->text().isEmpty()||mpFileTextOut->text().isEmpty()) return;

    if(mpVideoScale->setup(mpFileText->text(),mpFileTextOut->text())<0)
        return;
    if(mpVideoScale->setScale(mpWidthText->text().toInt(),
                              mpHeightText->text().toInt(),
                              (enum AVPixelFormat)mpPixFmtCmbBox->currentData().toInt())<0)
        return;
    mpVideoScale->doScale();
    mpVideoScale->tearDown();
    pRunBtn->setEnabled(false);
    mpWidthText->setText(QString::number(mpVideoScale->getWidth()));
    mpHeightText->setText(QString::number(mpVideoScale->getHeight()));
    pRunBtn->setText("Done");
}

void ScalePage::reset(){
    mpFileText->setText("");
    mpWidthText->setText("");
    mpHeightText->setText("");
    mpPixFmtCmbBox->setCurrentIndex(0);
    mpFileTextOut->setText("");
    pRunBtn->setText("Start");
    pRunBtn->setEnabled(true);
}
