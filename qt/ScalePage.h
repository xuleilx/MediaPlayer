#ifndef SCALEPAGE_H
#define SCALEPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QtDebug>

class ScalePage:public QWidget
{
public:
    ScalePage();
private:
    QGridLayout *mLayout;
};

#endif // SCALEPAGE_H
