#include <stdio.h>
#include <stdlib.h>
#include "core/video/demuxer/demuxer.h"
#include "core/video/decoder/decoder.h"
#include "core/video/scaler/scaler.h"
#include "core/video/videoOut/videoOut.h"
#include "core/video/Time/Time.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "[INFO]Usage: %s <video_file>\n", argv[0]);
        return -1;
    }

    DemuxerCtx demux;
    DecoderCtx decode;
    ScalerCtx scaler;
    VideoOutCtx videoOut;
    Time time;
    int scaler_initialized = 0;
    int videoOut_initialized = 0;

    // Initialize the format parser and find the video stream
    if (demuxer_init(&demux, argv[1]) != 0) {
        fprintf(stderr, "[ERR] Failed to initialize demuxer.\n");
        return -1;
    }

    // Grab the stream parameters extracted by the demuxer
    AVCodecParameters *pParams = demux.pFormatCtx->streams[demux.videoStream]->codecpar;

    // Pass the matching codec parameters and codec pointer into the decoder setup
    if (decoder_init(&decode, demux.pCodec, pParams) != 0) {
        fprintf(stderr, "[ERR] Failed to initialize decoder.\n");
        demuxer_close(&demux);
        return -1;
    }

    // Packet structure allocation to read data chunks into
    AVPacket *pPacket = av_packet_alloc();
    if (pPacket == NULL) 
    {
        fprintf(stderr, "[ERR] Failed to allocate packet.\n");
        decoder_close(&decode);
        demuxer_close(&demux);
        return -1;
    }

    printf("[INFO] Successfully initialized modular media engine!\n");

    // Main execution loop -> Read compressed packets until EOF (end of file)
    time.start = get_time_relative();
    while (av_read_frame(demux.pFormatCtx, pPacket) >= 0) 
    {
        // Intercept desktop OS interactions (e.g. user clicked window "X" button)
        if (videoOut_PollEvents() == -1) {
            break;
        }

        // Only decode if the packet matches our targeted video stream index
        if (pPacket->stream_index == demux.videoStream) 
        {
            int decode_status = decoder_start(&decode, pPacket);
            if (decode_status == 1) 
            {
                // DYNAMIC INITIALIZATION: Build the scaler and window using the real decoded frame parameters!
                if (!scaler_initialized) {
                    if (scaler_init(&scaler, 
                                    decode.pFrame->width, decode.pFrame->height, decode.pFrame->format, 
                                    decode.pFrame->width, decode.pFrame->height, AV_PIX_FMT_RGB24) != 0) 
                    {
                        fprintf(stderr, "[ERR] Failed to dynamically initialize scaler.\n");
                        break;
                    }
                    scaler_initialized = 1;

                    if (videoOut_init(&videoOut, decode.pFrame->height, decode.pFrame->width, "frvideoplayer") != 0) {
                        fprintf(stderr, "[ERR] Failed to dynamically initialize video window.\n");
                        break;
                    }
                    videoOut_initialized = 1;
                }

                // Perform the conversion scaling operation
                if (scaler_start(
                    &scaler, 
                    decode.pFrame->data, 
                    decode.pFrame->linesize, 
                    decode.pFrame->height) != 0)  
                    {
                        fprintf(stderr, "[ERR] Failed to scale frame.\n");
                    } 
                    else 
                    {
                        videOut_FrameRender(&videoOut, scaler.dstData[0], scaler.dstLinesize[0]);
                    }

                    time.current = get_time_relative();
                    time.elapsed = time.current - time.start;

                    double pts = get_pts(decode.pFrame, demux.pFormatCtx, demux.videoStream);
                    double delay = pts - time.elapsed;
                    if (delay > 0)
                    {
                        SDL_Delay((uint32_t)(delay * 1000.0));
                    }

                    av_frame_unref(decode.pFrame);
            }
            else if (decode_status < 0)
            {
                fprintf(stderr, "[ERR] Failed to decode packet.\n");
                av_packet_free(&pPacket);
                if (videoOut_initialized) { videoOut_close(&videoOut); }
                if (scaler_initialized) { scaler_close(&scaler); }
                decoder_close(&decode);
                demuxer_close(&demux);
                return -1;
            }
        }
        av_packet_unref(pPacket);
    }

    // Standard cleanup sequence before normal program exit
    av_packet_free(&pPacket);
    if (videoOut_initialized) { videoOut_close(&videoOut); }
    if (scaler_initialized) { scaler_close(&scaler); }
    
    decoder_close(&decode);
    demuxer_close(&demux);

    return 0;
}
