#ifndef SDL2WIDGET_H
#define SDL2WIDGET_H

#include <QWidget>
#include <SDL.h>
#include <SDL_image.h>

class SDL2Widget:public QWidget
{
    Q_OBJECT
public:
    SDL2Widget(QWidget* parent=0);
    void init();
    ~SDL2Widget();
protected:
    void paintEvent(QPaintEvent* event);
public slots:
    void slotFlushOneFrame();

private:
    SDL_Window *window;
    SDL_Renderer *render;
    SDL_Surface *surface;
    SDL_Texture *texture;
};

#endif // SDL2WIDGET_H
