#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SDL.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    int createSDLWindow();
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Surface *surface;
    SDL_Texture *tex;
};

#endif // MAINWINDOW_H
