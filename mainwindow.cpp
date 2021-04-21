#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <string>
#include <thread>
#include "SDL2Widget.h"
#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    win = nullptr;
    ren = nullptr;
    surface = nullptr;
    tex = nullptr;

    ui->setupUi(this);
}

int MainWindow::createSDLWindow(){
    std::string imagePath = "/home/xuleilx/mywork/picture/123.bmp";
}

MainWindow::~MainWindow()
{

    delete ui;
}
