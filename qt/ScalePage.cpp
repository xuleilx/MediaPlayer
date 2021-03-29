#include "ScalePage.h"
#include <QFileDialog>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
}

ScalePage::ScalePage()
{
    mLayout = new QGridLayout(this);

    QLabel *pFileLabel = new QLabel("文件: ");
    mpFileText = new QLineEdit();
    QPushButton *pFileBtn = new QPushButton("文件");
    mLayout->addWidget(pFileLabel,0,0);
    mLayout->addWidget(mpFileText,0,1);
    mLayout->addWidget(pFileBtn,0,2);
    connect(pFileBtn, &QPushButton::clicked, this, &ScalePage::inputFile);

    QLabel *pWidth = new QLabel("长: ");
    mpWidthText = new QLineEdit();
    mpWidthText->setValidator(new QIntValidator(1, 9999, this));
    mLayout->addWidget(pWidth,1,0);
    mLayout->addWidget(mpWidthText,1,1);

    QLabel *pHeight = new QLabel("宽: ");
    mpHeightText = new QLineEdit();
    mpHeightText->setValidator(new QIntValidator(1, 9999, this));
    mLayout->addWidget(pHeight,2,0);
    mLayout->addWidget(mpHeightText,2,1);

    QLabel *pPixFmt = new QLabel("像素格式: ");
    mpPixFmtCmbBox = new QComboBox();
    //QMap自动根据 key排序
    QMap<QString, int> formatMap;
    formatMap.insert("GRAY8",AV_PIX_FMT_GRAY8);
    formatMap.insert("YUV420P",AV_PIX_FMT_YUV420P);
    formatMap.insert("YUV422P",AV_PIX_FMT_YUV422P);
    formatMap.insert("YUV444P",AV_PIX_FMT_YUV444P);
    formatMap.insert("YUYV422",AV_PIX_FMT_YUYV422);
    formatMap.insert("RGB24",AV_PIX_FMT_RGB24);
    mpPixFmtCmbBox->clear();
    foreach(const QString &str,formatMap.keys())
        mpPixFmtCmbBox->addItem(str,formatMap.value(str));

    mLayout->addWidget(pPixFmt,3,0);
    mLayout->addWidget(mpPixFmtCmbBox,3,1);

    QLabel *pFileLabelOut = new QLabel("输出文件路径: ");
    mpFileTextOut = new QLineEdit();
    QPushButton *pFileOutBtn = new QPushButton("文件");
    mLayout->addWidget(pFileLabelOut,4,0);
    mLayout->addWidget(mpFileTextOut,4,1);
    mLayout->addWidget(pFileOutBtn,4,2);
    connect(pFileOutBtn, &QPushButton::clicked, this, &ScalePage::outputFile);

    QPushButton *pRunBtn = new QPushButton("执行");
    mLayout->addWidget(pRunBtn,5,3);
    connect(pRunBtn, &QPushButton::clicked, this, &ScalePage::run);
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
    qDebug()<<mpWidthText->text();
    qDebug()<<mpHeightText->text();
    qDebug()<<mpPixFmtCmbBox->currentText();
    qDebug()<<mpFileTextOut->text();
}
