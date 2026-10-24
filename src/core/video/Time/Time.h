#ifndef TIME_H
#define TIME_H

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

typedef struct TIME
{
    double start;
    double current;
    double elapsed;
    double delay;
} Time;

double get_time_relative();
double get_pts(AVFrame *pFrame, AVFormatContext *pFormatctx, int VideoStream);

#endif