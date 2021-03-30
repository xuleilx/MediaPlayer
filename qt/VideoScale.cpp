#include "VideoScale.h"
#include <QDebug>
#include <QDataStream>

VideoScale::VideoScale()
{
    mpOutFile = new QFile;
}

int VideoScale::init(QString inFile,QString outFile){
    int ret;
    /* Open the input file for reading. */
    if ((ret = openInputFile(inFile) < 0))
        goto end;
    /* Open the output file for writing. */
    if ((ret = openOutputFile(outFile)) < 0)
        goto end;
end:
    return 0;

}

void VideoScale::setDstParam(int dst_w, int dst_h, enum AVPixelFormat dst_pix_fmt)
{
    mWidth = dst_w;
    mHeight = dst_h;
    mFormat = dst_pix_fmt;
}

int VideoScale::setScale(int dst_w,int dst_h,enum AVPixelFormat dst_pix_fmt){
    /* create scaling context */
    int ret;
    int src_w, src_h;
    enum AVPixelFormat src_pix_fmt;

    src_w = mpInputCodecContext->width;
    src_h = mpInputCodecContext->height;
    src_pix_fmt = mpInputCodecContext->pix_fmt;

    if(dst_w == 0 || dst_h == 0){
        setDstParam(src_w, src_h, dst_pix_fmt);
    }else{
        setDstParam(dst_w, dst_h, dst_pix_fmt);
    }

    mpSwsCtx = sws_getContext(src_w, src_h, src_pix_fmt,
                              mWidth, mHeight, dst_pix_fmt,
                              SWS_BILINEAR, NULL, NULL, NULL);
    if (!mpSwsCtx) {
        fprintf(stderr,
                "Impossible to create scale context for the conversion "
                "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
                av_get_pix_fmt_name(src_pix_fmt), src_w, src_h,
                av_get_pix_fmt_name(dst_pix_fmt), mWidth, mHeight);
        ret = AVERROR(EINVAL);
        goto end;
    }

    /* buffer is going to be written to rawvideo file, no alignment */
    if ((ret = av_image_alloc(dst_data, dst_linesize,
                              mWidth, mHeight, dst_pix_fmt, 1)) < 0) {
        fprintf(stderr, "Could not allocate destination image\n");
        goto end;
    }

end:
    return ret;
}

int VideoScale::doScale(){
    AVFrame *pFrame =NULL;
    AVPacket *pPacket=NULL;
    int response = 0;
    static int count = 0;
    mStart = true;

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
    qDebug()<<"mpInputCodecContext->width, mpInputCodecContext->height: "<<mpInputCodecContext->width<<' '<<mpInputCodecContext->height;
    mpSwsCtx = sws_getContext(mpInputCodecContext->width, mpInputCodecContext->height, mpInputCodecContext->pix_fmt,
                              mWidth, mHeight, mFormat,SWS_BILINEAR, NULL, NULL, NULL);
    if (!mpSwsCtx) {
        fprintf(stderr,
                "Impossible to create scale context for the conversion "
                "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
                av_get_pix_fmt_name(mpInputCodecContext->pix_fmt), mpInputCodecContext->width, mpInputCodecContext->height,
                av_get_pix_fmt_name(mFormat),mWidth, mHeight);
        goto exit;
    }

    av_opt_show2(mpSwsCtx,NULL,AV_OPT_FLAG_VIDEO_PARAM,0);

    while (mStart && (av_read_frame(mpInputFormatContext, pPacket) >= 0))
    {
        // if it's the video stream
        if (pPacket->stream_index == mStreamIndex) {
            response = decodePacket(pPacket, pFrame);
            if (response < 0)
                break;
        }
        av_packet_unref(pPacket);
        count++;
        if(count >=100) break;
    }
    return 0;
exit:
    if(mpSwsCtx) sws_freeContext(mpSwsCtx);
    if(mpInputFormatContext)avformat_close_input(&mpInputFormatContext);
    if(pPacket)av_packet_free(&pPacket);
    if(pFrame)av_frame_free(&pFrame);
    if(mpInputCodecContext) avcodec_free_context(&mpInputCodecContext);
}

int VideoScale::openOutputFile(QString outFile){
    mpOutFile->setFileName(outFile);
    if (!mpOutFile->open(QIODevice::WriteOnly))
        return -1;
    return 0;
}

/**
 * Open an input file and the required decoder.
 * @return Error code (0 if successful)
 */
int VideoScale::openInputFile(QString inFile)
{
    AVCodec *dec = NULL;
    AVStream *st;
    AVDictionary *opts = NULL;
    int error;
    int ret;
    enum AVMediaType type = AVMEDIA_TYPE_VIDEO;

    mpInputFormatContext = avformat_alloc_context();
    /* Open the input file to read from it. */
    if ((error = avformat_open_input(&mpInputFormatContext, inFile.toStdString().data(), NULL,
                                     NULL)) < 0) {
        qDebug()<<"Could not open input file "<< inFile;
        mpInputFormatContext = NULL;
        return error;
    }

    /* Get information on the input file (number of streams etc.). */
    if ((error = avformat_find_stream_info(mpInputFormatContext, NULL)) < 0) {
        qDebug()<<"Could not open find stream info ";
        avformat_close_input(&mpInputFormatContext);
        return error;
    }

    ret = av_find_best_stream(mpInputFormatContext, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), inFile.toStdString().data());
        return ret;
    } else {
        mStreamIndex = ret;
        st = mpInputFormatContext->streams[mStreamIndex];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        mpInputCodecContext = avcodec_alloc_context3(dec);
        if (!mpInputCodecContext) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(mpInputCodecContext, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders */
        if ((ret = avcodec_open2(mpInputCodecContext, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
    }

    return 0;
}

int VideoScale::decodePacket(AVPacket *pPacket,AVFrame *pFrame)
{
    int ret=-1;
    int dst_bufsize=-1;

    // Supply raw packet data as input to a decoder
    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
    int response = avcodec_send_packet(mpInputCodecContext, pPacket);

    if (response < 0) {
        //        qDebug("Error while sending a packet to the decoder: %s", av_err2str(response));
        return response;
    }

    while (response >= 0)
    {
        // Return decoded output data (into a frame) from a decoder
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
        response = avcodec_receive_frame(mpInputCodecContext, pFrame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            return 0;
        } else if (response < 0) {
            //            qDebug("Error while receiving a frame from the decoder: %s", av_err2str(response));
            return response;
        }

        if (response >= 0) {
            /* buffer is going to be written to rawvideo file, no alignment */
            if ((ret = av_image_alloc(dst_data, dst_linesize,
                                      mWidth, mHeight, mFormat, 1)) < 0) {
                fprintf(stderr, "Could not allocate destination image\n");
                break;
            }
            dst_bufsize = ret;
            /* convert to destination format */
            sws_scale(mpSwsCtx, pFrame->data,
                      pFrame->linesize, 0, pFrame->height, dst_data, dst_linesize);
            QDataStream out(mpOutFile);   // we will serialize the data into the file
            out.writeRawData((const char *)dst_data[0],dst_bufsize);
        }
    }
    av_freep(&dst_data[0]);
    return 0;
}
