#include "ScalePage.h"
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
    QTextEdit *pFileText = new QTextEdit();
    QPushButton *pFileBtn = new QPushButton("文件");
    mLayout->addWidget(pFileLabel,0,0);
    mLayout->addWidget(pFileText,0,1);
    mLayout->addWidget(pFileBtn,0,2);

    QLabel *pWidth = new QLabel("长: ");
    QTextEdit *pWidthText = new QTextEdit();
    mLayout->addWidget(pWidth,1,0);
    mLayout->addWidget(pWidthText,1,1);

    QLabel *pHeight = new QLabel("宽: ");
    QTextEdit *pHeightText = new QTextEdit();
    mLayout->addWidget(pHeight,2,0);
    mLayout->addWidget(pHeightText,2,1);

    QLabel *pPixFmt = new QLabel("像素格式: ");
    QComboBox *pPixFmtCmbBox = new QComboBox();
    //QMap自动根据 key排序
    QMap<QString, int> formatMap;
    formatMap.insert("GRAY8",AV_PIX_FMT_GRAY8);
    formatMap.insert("YUV420P",AV_PIX_FMT_YUV420P);
    formatMap.insert("YUV422P",AV_PIX_FMT_YUV422P);
    formatMap.insert("YUV444P",AV_PIX_FMT_YUV444P);
    formatMap.insert("YUYV422",AV_PIX_FMT_YUYV422);
    formatMap.insert("RGB24",AV_PIX_FMT_RGB24);
    pPixFmtCmbBox->clear();
    foreach(const QString &str,formatMap.keys())
        pPixFmtCmbBox->addItem(str,formatMap.value(str));

    mLayout->addWidget(pPixFmt,3,0);
    mLayout->addWidget(pPixFmtCmbBox,3,1);

    QLabel *pFileLabelOut = new QLabel("输出文件路径: ");
    QTextEdit *pFileTextOut = new QTextEdit();
    QPushButton *pFileOutBtn = new QPushButton("文件");
    mLayout->addWidget(pFileLabelOut,4,0);
    mLayout->addWidget(pFileTextOut,4,1);
    mLayout->addWidget(pFileOutBtn,4,2);

    QPushButton *pRunBtn = new QPushButton("执行");
    mLayout->addWidget(pRunBtn,5,3);
}
