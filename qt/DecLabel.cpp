#include "DecLabel.h"
#include<QPainter>
#include <QtDebug>

DecLabel::DecLabel():QLabel()
{
}

//接收sig_GetOneFrame信号，调用update在界面中显示图像
void DecLabel::slotGetOneFrame(QImage img)
{
    mImage = img;
    update(); //调用update将执行 paintEvent函数
}
//需要重写paintEvent事件
void DecLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (mImage.size().width() <= 0) return;
    ///将图像按比例缩放成和窗口一样大小
     QImage img = mImage.scaled(this->size(),Qt::IgnoreAspectRatio);
     int x = this->width() - img.width();
     int y = this->height() - img.height();
     painter.drawImage(QPoint(x,y),img); //画出图像
}
