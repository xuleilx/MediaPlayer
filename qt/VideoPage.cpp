#include "VideoPage.h"
#include "DecLabel.h"

VideoPage::VideoPage()
{

    mLayout = new QVBoxLayout(this);
    mVideoLabel = new DecLabel();
    mLayout->addWidget(mVideoLabel);
}
VideoPage::~VideoPage(){
    if(mLayout) delete mLayout;
    if(mVideoLabel) delete mVideoLabel;
}

void VideoPage::slotGetOneFrame(QImage img){
    mVideoLabel->slotGetOneFrame(img);
}
