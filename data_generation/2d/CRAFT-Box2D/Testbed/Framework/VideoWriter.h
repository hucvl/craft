//
//  VideoWriter.h
//  Testbed
//
//  Created by Tayfun Ateş on 17.11.2019.
//

#ifndef VideoWriter_h
#define VideoWriter_h

#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

static AVCodecContext *c = NULL;
static AVFrame *frame;
static int pts = 0;
static AVPacket pkt;
static FILE *file;
struct SwsContext *sws_context = NULL;

/*
Convert RGB24 array to YUV. Save directly to the `frame`,
modifying its `data` and `linesize` fields
*/
static void ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t *rgb) {
    const int in_linesize[1] = { 3 * c->width };
    sws_context = sws_getCachedContext(sws_context,
            c->width, c->height, AV_PIX_FMT_RGB24,
            c->width, c->height, AV_PIX_FMT_YUV420P,
            0, 0, 0, 0);
    sws_scale(sws_context, (const uint8_t * const *)&rgb, in_linesize, 0,
            c->height, frame->data, frame->linesize);
}


/* Allocate resources and write header data to the output file. */
void ffmpeg_encoder_start(const char *filename, AVCodecID codec_id, int fps, int width, int height) {
    AVCodec *codec;
    int ret;
    codec = avcodec_find_encoder(codec_id);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    c->bit_rate = 4000000;
    c->width = width;
    c->height = height;
    c->time_base.num = 1;
    c->time_base.den = fps;
    c->gop_size = 25;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    if (codec_id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;
    ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height, c->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }
}

/*
Write trailing data to the output file
and free resources allocated by ffmpeg_encoder_start.
*/
void ffmpeg_encoder_finish(void) {
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    int got_output, ret;
    do {
        fflush(stdout);
        ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }
        if (got_output) {
            fwrite(pkt.data, 1, pkt.size, file);
            av_packet_unref(&pkt);
        }
    } while (got_output);
    fwrite(endcode, 1, sizeof(endcode), file);
    fclose(file);
    avcodec_close(c);
    av_free(c);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
}

/*
Encode one frame from an RGB24 input and save it to the output file.
Must be called after ffmpeg_encoder_start, and ffmpeg_encoder_finish
must be called after the last call to this function.
*/
void ffmpeg_encoder_encode_frame(uint8_t *rgb) {
    int ret, got_output;
    ffmpeg_encoder_set_frame_yuv_from_rgb(rgb);
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
    if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
        exit(1);
    }
    if (got_output) {
        fwrite(pkt.data, 1, pkt.size, file);
        av_packet_unref(&pkt);
    }
}

void flipVertically(unsigned char *buffer, const unsigned int width, const unsigned int height, const int bytes_per_pixel)
{
    const unsigned int rows = height / 2; // Iterate only half the buffer to get a full flip
    const unsigned int row_stride = width * bytes_per_pixel;
    unsigned char* temp_row = (unsigned char*)malloc(row_stride);

    for (unsigned int row_index = 0; row_index < rows; row_index++)
    {
        unsigned char* source = buffer + (row_index * row_stride);
        unsigned char* target = buffer + ((height - row_index - 1) * row_stride);
        memcpy(temp_row, source,   row_stride);
        memcpy(source,   target,   row_stride);
        memcpy(target,   temp_row, row_stride);
    }

    free(temp_row);
    temp_row = NULL;
}

static void videoFlush(unsigned char* rgb, const int& width, const int& height) {
    frame->pts = pts;
    flipVertically(rgb, width, height, 3);
    ffmpeg_encoder_encode_frame(rgb);
    pts++;
}


void init(const std::string& filePath, const int& width, const int& height)
{
    avcodec_register_all();
    ffmpeg_encoder_start(filePath.c_str(), AV_CODEC_ID_MPEG1VIDEO, 60, width, height);
}

void deinit()
{
    ffmpeg_encoder_finish();
    pts = 0;
}

#endif /* VideoWriter_h */
