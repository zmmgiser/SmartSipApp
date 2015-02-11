#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
//#include <time.h>
#include <sys/time.h>


struct AVFormatContext *pFormatCtx;
struct AVCodecContext  *pAVCodecCtx;
struct AVCodec         *pAVCodec;
struct AVPacket        mAVPacket;
struct AVFrame         *pAVFrame; 
struct AVFrame         *pAVFrameYUV;
struct SwsContext      *pImageConvertCtx ;

int iWidth = 0;
int iHeight = 0;

#define LOG_TAG "H264Android.c"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


jint Java_com_smart_ffmpeg_SmartFFmpeg_init(JNIEnv* env, jobject thiz, jint width, jint height)
{
	iWidth = width;
	iHeight = height;
	
	if(pAVCodecCtx != NULL)
	{
		avcodec_close(pAVCodecCtx);
		pAVCodecCtx = NULL;
	}	
	if(pAVFrame != NULL)
	{
		av_free(pAVFrame);
		pAVFrame = NULL;
	}
	// Register all formats and codecs
    av_register_all();
	LOGD("avcodec register success");
	pAVCodec = avcodec_find_decoder(CODEC_ID_H264);
	if(pAVCodec == NULL)
	{
		goto fail;
	}
	LOGD("avcodec find decoder success");
	//init AVCodecContext
	pAVCodecCtx = avcodec_alloc_context3(pAVCodec);
	if(pAVCodecCtx == NULL)
	{
		goto fail;
	}
	LOGD("avcodec alloc context success");
	/* Width/height may be overriden by ffmpeg after first decoding. */
	pAVCodecCtx->width  = pAVCodecCtx->coded_width  = iWidth;
	pAVCodecCtx->height = pAVCodecCtx->coded_height = iHeight;
	pAVCodecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
  	pAVCodecCtx->workaround_bugs = FF_BUG_AUTODETECT;
	/* we do not send complete frames */
	//if(pAVCodec->capabilities & CODEC_CAP_TRUNCATED)
        //pAVCodecCtx->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
	/* open it */   
    if (avcodec_open(pAVCodecCtx, pAVCodec) < 0)
    {
    	goto fail;
    }
	LOGD("avcodec open success");
	pAVFrame = avcodec_alloc_frame();
	if(pAVFrame == NULL)
	{
		goto fail;
	}
	pAVFrameYUV = avcodec_alloc_frame();
	if(pAVFrameYUV == NULL)
	{
		goto fail;
	}
	LOGD("avcodec context  success");

	return 0;

	fail:
	LOGE("Java_com_smart_ffmpeg_SmartFFmpeg_init failed");
	if(pAVCodecCtx != NULL)
	{
		avcodec_close(pAVCodecCtx);
		pAVCodecCtx = NULL;
	}
	if(pAVFrame != NULL)
	{
		av_free(pAVFrame);
		pAVFrame=NULL;
	}
	if(pAVFrameYUV != NULL)
	{
		av_free(pAVFrameYUV);
		pAVFrameYUV = NULL;
	}

	return -1;
}

jint Java_com_smart_ffmpeg_SmartFFmpeg_destroy(JNIEnv* env, jobject thiz)
{
	if(pAVCodecCtx != NULL)
	{
		avcodec_close(pAVCodecCtx);
		pAVCodecCtx = NULL;
	}
	if(pAVFrame != NULL)
	{
		av_free(pAVFrame);
		pAVFrame=NULL;
	}
	if(pAVFrameYUV != NULL)
	{
		av_free(pAVFrameYUV);
		pAVFrameYUV = NULL;
	}
	return 0;
}


jint Java_com_smart_ffmpeg_SmartFFmpeg_decodeFrame(JNIEnv* env, jobject thiz, jbyteArray in, jint inbuf_size, jbyteArray out)
{
	int i;

	jbyte * inbuf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * Picture = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);

	avcodec_get_frame_defaults(pAVFrame);

	av_init_packet(&mAVPacket);

	mAVPacket.data = inbuf;
	mAVPacket.size = inbuf_size;
	int len = -1, got_picture = 0;
	//decode frame
	struct timeval start, end;
	gettimeofday( &start, NULL );
	len = avcodec_decode_video2(pAVCodecCtx, pAVFrame, &got_picture, &mAVPacket);
	gettimeofday( &end, NULL );
	int timeuse = (1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec) / 1000;
	LOGD("avcodec_decode_video2 use itme:%d", timeuse);
	//decode error
	if(len < 0)
	{
		LOGE("len = -1,decode error");
		goto result;
	}
	//got a picture, start change YUV420P to RGB565
	if(got_picture > 0)
	{
		LOGD("GOT PICTURE");
		pImageConvertCtx = sws_getContext(pAVCodecCtx->width, pAVCodecCtx->height, pAVCodecCtx->pix_fmt,
				pAVCodecCtx->width, pAVCodecCtx->height,
				PIX_FMT_RGB565LE, SWS_BICUBIC, NULL, NULL, NULL);
		avpicture_fill((AVPicture*)pAVFrameYUV, (uint8_t*)Picture, PIX_FMT_RGB565LE, pAVCodecCtx->width, pAVCodecCtx->height);
		sws_scale(pImageConvertCtx, (const uint8_t* const*)pAVFrame->data, pAVFrame->linesize,0, pAVCodecCtx->height, pAVFrameYUV->data, pAVFrameYUV->linesize);
	}
	else
	{
		LOGE("GOT PICTURE fail");
	}

	result:
    (*env)->ReleaseByteArrayElements(env, in, inbuf, 0);    
    (*env)->ReleaseByteArrayElements(env, out, Picture, 0); 

	return len;	
}
