#include "AudioPlayer.h"
#include "QtDebug"

AudioPlayer::AudioPlayer():mpFormatCtx(NULL),mType(AVMEDIA_TYPE_AUDIO),mpCodecCtx(NULL)
{
    mAudioOutput = new AudioOutput();
    // output param ,This is for QAudioOutput.
    mFmt = AV_SAMPLE_FMT_S16;
    mSampleRate = 44100;
    mChannels = 2;
}

AudioPlayer::~AudioPlayer(){
    if(mpFormatCtx)avformat_close_input(&mpFormatCtx);
    if(mpCodecCtx) avcodec_free_context(&mpCodecCtx);
    if(mAudioOutput) delete mAudioOutput;
}

/**
 * Initialize a temporary storage for the specified number of audio samples.
 * The conversion requires temporary storage due to the different format.
 * The number of audio samples to be allocated is specified in frame_size.
 * @param[out] converted_input_samples Array of converted samples. The
 *                                     dimensions are reference, channel
 *                                     (for multi-channel audio), sample.
 * @param      output_codec_context    Codec context of the output file
 * @param      frame_size              Number of samples to be converted in
 *                                     each round
 * @return Error code (0 if successful)
 */
int AudioPlayer::init_converted_samples(uint8_t ***converted_input_samples,
                                        AVCodecContext *output_codec_context,
                                        int frame_size)
{
    int error;

    /* Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats).
     */
    if (!(*converted_input_samples = (uint8_t **)calloc(2,sizeof(**converted_input_samples)))) {
        fprintf(stderr, "Could not allocate converted input sample pointers\n");
        return AVERROR(ENOMEM);
    }

    /* Allocate memory for the samples of all channels in one consecutive
     * block for convenience. */
    if ((error = av_samples_alloc(*converted_input_samples, NULL,
                                  mChannels,
                                  frame_size,
                                  mFmt, 0)) < 0) {
        fprintf(stderr,
                "Could not allocate converted input samples (error '%s')\n",
                (error));
        av_freep(&(*converted_input_samples)[0]);
        free(*converted_input_samples);
        return error;
    }
    return 0;
}

int AudioPlayer::initSwr()
{
    int ret = -1;
    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        fprintf(stderr, "Could not allocate resampler context\n");
        goto error;
    }

    /* set options */
    av_opt_set_int       (swr_ctx, "in_channel_count",   mpCodecCtx->channels,       0);
    av_opt_set_int       (swr_ctx, "in_sample_rate",     mpCodecCtx->sample_rate,    0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt",      mpCodecCtx->sample_fmt, 0);
    av_opt_set_int       (swr_ctx, "out_channel_count",  mChannels,       0);
    av_opt_set_int       (swr_ctx, "out_sample_rate",    mSampleRate,    0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt",     mFmt,     0);

    /* initialize the resampling context */
    if ((ret = swr_init(swr_ctx)) < 0) {
        fprintf(stderr, "Failed to initialize the resampling context\n");
        goto error;
    }
error:
    return ret;
}

int AudioPlayer::initFormat(QString filepath)
{
    if(avformat_open_input(&mpFormatCtx,filepath.toStdString().data(),NULL,NULL)<0){
        qDebug("Could not open source file %s\n", filepath.toStdString().data());
        return -1;
    }

    if(avformat_find_stream_info(mpFormatCtx,NULL)<0){
        qDebug("Could not find stream information\n");
        return -1;
    }
    return 0;
}

int AudioPlayer::init(QString filepath){
    mpFormatCtx = NULL;
    mStart = false;

    // AVFormat
    if(initFormat(filepath)!=0)
        goto error;

    av_dump_format(mpFormatCtx,0,filepath.toStdString().data(),0);

    // AVCodec
    if (initCodecCtx()!=0)
        goto error;

    // swresample
    /* create resampler context */
    if(initSwr()<0){
        qDebug("Could not init swr\n");
        goto error;
    }

    mAudioOutput->init();

    return 0;

error:
    if(mpFormatCtx) avformat_close_input(&mpFormatCtx);
    return -1;
}
/**
 * Convert the input audio samples into the output sample format.
 * The conversion happens on a per-frame basis, the size of which is
 * specified by frame_size.
 * @param      input_data       Samples to be decoded. The dimensions are
 *                              channel (for multi-channel audio), sample.
 * @param[out] converted_data   Converted samples. The dimensions are channel
 *                              (for multi-channel audio), sample.
 * @param      frame_size       Number of samples to be converted
 * @param      resample_context Resample context for the conversion
 * @return Error code (0 if successful)
 */
int AudioPlayer::convert_samples(const uint8_t **input_data,
                                 uint8_t **converted_data, const int frame_size,
                                 SwrContext *resample_context)
{
    int error;

    /* Convert the samples using the resampler. */
    if ((error = swr_convert(resample_context,
                             converted_data, frame_size,
                             input_data    , frame_size)) < 0) {
        fprintf(stderr, "Could not convert input samples (error '%s')\n",
                (error));
        return error;
    }

    return 0;
}
int AudioPlayer::decode(AVPacket *pPacket){
    AVFrame *pFrame = NULL;
    int response = -1;
    int data_size;

    uint8_t **dst_data = NULL;
    int dst_bufsize,dst_linesize;

    pFrame = av_frame_alloc();
    if(!pFrame){
        qDebug("Could not alloc the frame\n");
        response = -1;
        goto exit;
    }

    response = avcodec_send_packet(mpCodecCtx,pPacket);
    if (response < 0) {
        fprintf(stderr, "Error submitting the packet to the decoder\n");
        goto exit;
    }

    while(response >= 0){
        response = avcodec_receive_frame(mpCodecCtx, pFrame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            return 0;
        } else if (response < 0) {
            qDebug("Error while receiving a frame from the decoder");
            return response;
        }
        qDebug("Frame %d (size=%d bytes, format=%d) pts %d [DTS %d]",
               mpCodecCtx->frame_number,
               pFrame->pkt_size,
               pFrame->format,
               pFrame->pts,
               pFrame->pkt_dts);

        data_size = av_get_bytes_per_sample(mpCodecCtx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            fprintf(stderr, "Failed to calculate data size\n");
            exit(1);
        }

        // convert frame
        /* Initialize the temporary storage for the converted input samples. */
        if (init_converted_samples(&dst_data,NULL,pFrame->nb_samples))
            goto exit;

        /* Convert the input samples to the desired output sample format.
         * This requires a temporary storage provided by converted_input_samples. */
        if (convert_samples((const uint8_t**)pFrame->extended_data, dst_data,
                            pFrame->nb_samples, swr_ctx))
            goto exit;
        // end convert
        dst_bufsize = av_samples_get_buffer_size(&dst_linesize, mChannels,
                                                 pFrame->nb_samples, mFmt, 0);

        // play data
        mAudioOutput->processOneFrame((const char *)dst_data[0], dst_bufsize);

        av_frame_unref(pFrame);
    }

exit:
    return response;
}

int AudioPlayer::initCodecCtx(){
    AVStream *st;
    AVCodec *dec;
    int ret;
    ret = av_find_best_stream(mpFormatCtx,mType,-1,-1,NULL,0);
    if(ret<0){
        qDebug( "Could not find %s stream in input file.\n", av_get_media_type_string(mType));
        return ret;
    }
    mStreamIndex = ret;
    st = mpFormatCtx->streams[mStreamIndex];

    dec = avcodec_find_decoder(st->codecpar->codec_id);
    if(!dec){
        qDebug("Failed to find %s codec\n", av_get_media_type_string(mType));
        return AVERROR(EINVAL);
    }

    mpCodecCtx = avcodec_alloc_context3(dec);
    if(!mpCodecCtx){
        qDebug("Failed to allocate the %s codec context\n",av_get_media_type_string((mType)));
        return AVERROR(ENOMEM);
    }

    if((ret =avcodec_parameters_to_context(mpCodecCtx,st->codecpar)) < 0 ){
        qDebug("Failed to copy %s codec parameters to decoder context\n", av_get_media_type_string(mType));
        return ret;
    }

    if((ret = avcodec_open2(mpCodecCtx,dec,NULL))<0){
        qDebug("Failed to open %s codec\n", av_get_media_type_string(mType));
        return ret;
    }

    return 0;
}

void AudioPlayer::run(){
    AVPacket *pPacket = NULL;
    int response = 0;

    if(!mpFormatCtx || !mpCodecCtx) goto exit;

    mStart = true;
    pPacket = av_packet_alloc();
    if(!pPacket){
        qDebug("Could not alloc the packet\n");
        goto exit;
    }

    while(mStart && (av_read_frame(mpFormatCtx,pPacket)>=0)){
        if (pPacket->stream_index == mStreamIndex){
            response = decode(pPacket);
            if (response < 0)
                break;
        }
        av_packet_unref(pPacket);
    }

    if(pPacket)av_packet_free(&pPacket);
exit:
    return;
}
