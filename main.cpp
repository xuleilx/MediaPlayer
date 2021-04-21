#include "mainwindow.h"
#include <QApplication>
#include "SDL2Widget.h"
#include <QVBoxLayout>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    MainWindow w;
    //    w.show();
    QVBoxLayout* layout = new QVBoxLayout;
    SDL2Widget* sdlWidget = new SDL2Widget;
    QPushButton* button = new QPushButton("Button");
    layout->addWidget(sdlWidget);
    layout->addWidget(button);
    QWidget w;
    w.setLayout(layout);
    w.resize(640,480);
    w.show();
    return a.exec();
}
