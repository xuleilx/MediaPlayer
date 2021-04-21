#include "SDL2Widget.h"
#include <iostream>
#include <QTimer>

SDL2Widget::SDL2Widget(QWidget* parent):QWidget(parent)
{
    window=nullptr;
    render=nullptr;
    surface=nullptr;
    texture=nullptr;
    init();
}

SDL2Widget::~SDL2Widget()
{
    /* 销毁render和texture，释放内存 */
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
}

void SDL2Widget::slotFlushOneFrame(){
    /* 在window上创建一个render */
    /* SDL_RENDERER_ACCELERATED 必须在paintEvent中建立，否则卡死 */
    if(render == nullptr){
        render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(render == nullptr){
            std::cout<<"SDL_CreateRenderer failed!"<<std::endl;
        }
    }

    /* 加载图片，创建surface */
    if(texture == nullptr){
        texture = IMG_LoadTexture(render,"/home/xuleilx/mywork/picture/123.jpg");
        if(texture == nullptr){
            std::cout<<"IMG_LoadTexture failed!"<<std::endl;
        }
    }
    if(render==nullptr || texture == nullptr) return;
    /* 清空render的内容 */
    SDL_RenderClear(render);
    /* 将texture复制到render上 */
    SDL_RenderCopy(render, texture, NULL, NULL);
    /* 将render显示到window上 */
    SDL_RenderPresent(render);
}

void SDL2Widget::init(){
    /* SDL2初始化 */
    SDL_Init(SDL_INIT_EVERYTHING);

    /* 把QWidget转换为SDL_Window */
    window = SDL_CreateWindowFrom((void*)this->winId());
    if(window == nullptr){
        std::cout<<"SDL_CreateWindowFrom failed!"<<std::endl;
    }

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(slotFlushOneFrame()));
    timer->start(1000 / 30); // 30 fps
}

/* 绘图操作必须在paintEvent事件里进行 */
void SDL2Widget::paintEvent(QPaintEvent* event)
{
    setUpdatesEnabled(false);
}
