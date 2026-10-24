#include "Time.h"

double get_time_relative()
{
    return (double)av_gettime_relative() / 1000000.0;
}

double get_pts(AVFrame *pFrame, AVFormatContext *pFormatctx, int VideoStream)
{
    double pts = pFrame->pts;
    if (pts == AV_NOPTS_VALUE)
    {
        pts = pFrame->best_effort_timestamp;
    }

    if (pts == AV_NOPTS_VALUE)
    {
        return 0.0; // failback
    }

    return pts * av_q2d(pFormatctx->streams[VideoStream]->time_base);
}