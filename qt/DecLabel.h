#ifndef DECLABEL_H
#define DECLABEL_H
#include <QLabel>
#include <QImage>

class DecLabel : public QLabel
{
    Q_OBJECT
public:
    DecLabel();
    void paintEvent(QPaintEvent *);

public slots:
    void slotGetOneFrame(QImage img);

private:
    QImage mImage;

};

#endif // DECLABEL_H
