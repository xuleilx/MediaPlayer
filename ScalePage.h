#ifndef SCALEPAGE_H
#define SCALEPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QtDebug>
#include <QString>

class VideoScale;

class ScalePage:public QWidget
{
public:
    ScalePage();

public slots:
    void inputFile();
    void outputFile();
    void run();
    void reset();

private:
    QGridLayout *mLayout;
    VideoScale *mpVideoScale;

    QLineEdit *mpFileText;
    QLineEdit *mpWidthText;
    QLineEdit *mpHeightText;
    QComboBox *mpPixFmtCmbBox;
    QLineEdit *mpFileTextOut;

    QPushButton *pRunBtn;
};

#endif // SCALEPAGE_H
