#include "resampler.h"
#include <stdlib.h>


int ResamplerInit(ResamplerCtx *ctx,
    AVChannelLayout SRCChanLayout,
    int SrcSampleRate,
    enum AVSampleFormat SRCSampleFmt,
    int DSTsamplerate,
    enum AVSampleFormat DSTSampleFmt
) {
    
    int ret;

    ctx->pSwrCtx = NULL;
    ctx->DSTSampleFmt = DSTSampleFmt;
    ctx->DSTsamplerate = DSTsamplerate;

    // Allocate mathematical resampling framework using FFMPEGs modern signature
    // Configure the  default stereo channel layout for the destination output source
    av_channel_layout_default(&ctx->DSTChanLayout, 2);

    // Allocate mathematical resampling framework using FFMPEGs modern signature
    ret = swr_alloc_set_opts2(&ctx->pSwrCtx,
        &ctx->DSTChanLayout, DSTSampleFmt, DSTsamplerate,
        &SRCChanLayout, SRCSampleFmt, SrcSampleRate,
        0, NULL
    );
    if (ret < 0 || ctx->pSwrCtx == NULL) {
        return -1; //Allocation failure for the resampler context layout
    }

    // Engine initialization (configuartion parameter layers)
    if (swr_init(ctx->pSwrCtx) < 0) {
        swr_free(&ctx->pSwrCtx);
        return -1; //Initialization failure for the resampler context
    }

    return 0;

}



int ResamplerStart(ResamplerCtx *ctx, uint8_t **SrcData, int SrcNbSamples, uint8_t **DstData) {
    int ret = swr_convert(ctx->pSwrCtx, 
        DstData, SrcNbSamples,
        (const uint8_t **)SrcData, SrcNbSamples
    );

    if (ret < 0) { return -1; }

    return ret;

}


void ResamplerStop(ResamplerCtx *ctx) {
    if (ctx) {
        if (ctx->pSwrCtx) {
            swr_free(&ctx->pSwrCtx);
        }
        av_channel_layout_uninit(&ctx->DSTChanLayout);
    }


}