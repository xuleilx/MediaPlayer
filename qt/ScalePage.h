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

class ScalePage:public QWidget
{
public:
    ScalePage();
public slots:
    void inputFile();
    void outputFile();
    void run();
private:
    QGridLayout *mLayout;

    QLineEdit *mpFileText;
    QLineEdit *mpWidthText;
    QLineEdit *mpHeightText;
    QComboBox *mpPixFmtCmbBox;
    QLineEdit *mpFileTextOut;
};

#endif // SCALEPAGE_H
