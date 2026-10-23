#ifndef RESAMPLER_H
#define RESAMPLER_H

#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
#include <libavutil/samplefmt.h>


typedef struct {
    struct SwrContext *pSwrCtx;
    int DSTsamplerate;
    AVChannelLayout DSTChanLayout;
    enum AVSampleFormat DSTSampleFmt;


} ResamplerCtx;


int ResamplerInit(ResamplerCtx *ctx,
    AVChannelLayout SRCChanLayout,
    int SrcSampleRate,
    enum AVSampleFormat SRCSampleFmt,
    int DSTsamplerate,
    enum AVSampleFormat DSTSampleFmt
);

int ResamplerStart(ResamplerCtx *ctx, uint8_t **SrcData, int SrcNbSamples, uint8_t **DstData);
void ResamplerStop(ResamplerCtx *ctx);

#endif