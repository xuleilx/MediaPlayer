#include "VideoPlayer.h"
#include <QDebug>

VideoPlayer::VideoPlayer()
{
}

int VideoPlayer::init(QString filepath){
    mpFormatCtx = avformat_alloc_context();
    mpCodec = NULL;
    mpCodecParameters = NULL;
    mStreamIndex = -1;
    mStart = false;

    if (!mpFormatCtx) {
        qDebug()<<"ERROR could not allocate memory for Format Context";
        goto error;
    }
    if(avformat_open_input(&mpFormatCtx,filepath.toStdString().data(),NULL,NULL)!=0){
        qDebug()<<"Error could not open the file";
        goto error;
    }
    if(avformat_find_stream_info(mpFormatCtx,NULL) < 0){
        qDebug()<<"Error could not get the stream info";
        goto error;
    }
    for(int i=0;i< mpFormatCtx->nb_streams;i++){
        AVCodecParameters *pLocalCodecParameters = mpFormatCtx->streams[i]->codecpar;
        AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
        if (pLocalCodec == NULL) {
            qDebug()<<"ERROR unsupported codec!";
            // In this example if the codec is not found we just skip it
            continue;
        }
        if(pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO){
            mStreamIndex = i;
            mpCodecParameters = pLocalCodecParameters;
            mpCodec = pLocalCodec;
            qDebug("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
        }else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            qDebug("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
        }
    }

    if (mStreamIndex == -1) {
        qDebug("File %s does not contain a video stream!", filepath.toStdString().data());
        goto error;
    }
    return 0;
error:
    if(mpFormatCtx) avformat_close_input(&mpFormatCtx);
    return -1;
}


int VideoPlayer::decodePacket(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame)
{
    int ret=-1;
    uint8_t *dst_data[4];
    int  dst_linesize[4];
    int dst_bufsize;
    // Supply raw packet data as input to a decoder
    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
    int response = avcodec_send_packet(pCodecContext, pPacket);

    if (response < 0) {
        //        qDebug("Error while sending a packet to the decoder: %s", av_err2str(response));
        return response;
    }

    while (response >= 0)
    {
        // Return decoded output data (into a frame) from a decoder
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
        response = avcodec_receive_frame(pCodecContext, pFrame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            return 0;
        } else if (response < 0) {
            //            qDebug("Error while receiving a frame from the decoder: %s", av_err2str(response));
            return response;
        }

        if (response >= 0) {
//            qDebug("Frame %d (type=%c, size=%d bytes, format=%d) pts %d key_frame %d [DTS %d]",
//                   pCodecContext->frame_number,
//                   av_get_picture_type_char(pFrame->pict_type),
//                   pFrame->pkt_size,
//                   pFrame->format,
//                   pFrame->pts,
//                   pFrame->key_frame,
//                   pFrame->coded_picture_number);

            // Check if the frame is a planar YUV 4:2:0, 12bpp
            // That is the format of the provided .mp4 file
            // RGB formats will definitely not give a gray image
            // Other YUV image may do so, but untested, so give a warning
            if (pFrame->format != AV_PIX_FMT_YUV420P)
            {
                qDebug("Warning: the generated file may not be a grayscale image, but could e.g. be just the R component if the video format is RGB");
            }

            /* buffer is going to be written to rawvideo file, no alignment */
            if ((ret = av_image_alloc(dst_data, dst_linesize,
                                      pCodecContext->width, pCodecContext->height, dst_pix_fmt, 1)) < 0) {
                fprintf(stderr, "Could not allocate destination image\n");
                break;
            }
            dst_bufsize = ret;

            /* convert to destination format */
            sws_scale(mSwsCtx, pFrame->data,
                      pFrame->linesize, 0, pFrame->height, dst_data, dst_linesize);
            //把这个RGB数据 用QImage加载
            //            QImage imageP("/home/xuleilx/mywork/picture/123.jpg");
            //            emit sigGetOneFrame(imageP);
            QImage image(dst_data[0],pCodecContext->width, pCodecContext->height,QImage::Format_RGB32);
            emit sigGetOneFrame(image);  //发送信号，将次imamge发送到要显示的控件，用paintEvent绘制出来
        }
    }
    av_freep(&dst_data[0]);
    return 0;
}

void VideoPlayer::run(){
    AVFrame *pFrame =NULL;
    AVPacket *pPacket=NULL;
    int response = 0;
    mStart = true;
    AVCodecContext *pCodecContext = avcodec_alloc_context3(mpCodec);
    if(!pCodecContext){
        qDebug("failed to allocated memory for AVCodecContext");
        goto exit;
    }

    if(avcodec_parameters_to_context(pCodecContext,mpCodecParameters)<0){
        qDebug("failed to copy codec params to codec context");
        goto exit;
    }

    if(avcodec_open2(pCodecContext,mpCodec,NULL)<0){
        qDebug("failed to open codec through avcodec_open2");
        goto exit;
    }

    pFrame = av_frame_alloc();
    if (!pFrame)
    {
        qDebug("failed to allocated memory for AVFrame");
        goto exit;
    }

    pPacket = av_packet_alloc();
    if (!pPacket)
    {
        qDebug("failed to allocated memory for AVPacket");
        goto exit;
    }

    /* create scaling context */
    qDebug()<<"pCodecContext->width, pCodecContext->height: "<<pCodecContext->width<<' '<<pCodecContext->height;
    mSwsCtx = sws_getContext(pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt,
                             pCodecContext->width, pCodecContext->height, dst_pix_fmt,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!mSwsCtx) {
        fprintf(stderr,
                "Impossible to create scale context for the conversion "
                "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
                av_get_pix_fmt_name(pCodecContext->pix_fmt), pCodecContext->width, pCodecContext->height,
                av_get_pix_fmt_name(dst_pix_fmt),pCodecContext->width, pCodecContext->height);
        goto exit;
    }

    av_opt_show2(mSwsCtx,NULL,AV_OPT_FLAG_VIDEO_PARAM,0);

    while (mStart && (av_read_frame(mpFormatCtx, pPacket) >= 0))
    {
        msleep(15); // 30fps
        // if it's the video stream
        if (pPacket->stream_index == mStreamIndex) {
//            qDebug("AVPacket->pts %" PRId64, pPacket->pts);
            response = decodePacket(pPacket, pCodecContext, pFrame);
            if (response < 0)
                break;
        }
        av_packet_unref(pPacket);
    }

    qDebug("releasing all the resources");
exit:
    if(mSwsCtx) sws_freeContext(mSwsCtx);
    if(mpFormatCtx)avformat_close_input(&mpFormatCtx);
    if(pPacket)av_packet_free(&pPacket);
    if(pFrame)av_frame_free(&pFrame);
    if(pCodecContext) avcodec_free_context(&pCodecContext);
}
