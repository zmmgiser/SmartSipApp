package com.smart.smartsip.videocodec;

import java.nio.ByteBuffer;

import android.content.Context;
import android.graphics.Canvas;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MediaCodecVideoView extends SurfaceView implements SurfaceHolder.Callback, IFrameFeeder
{
    int mWidth = 1280;
    int mHeight = 720;
    //int mWidth = 1280;
    //int mHeight = 720;
    //int mWidth = 768;
    //int mHeight = 432;
    
    H264FrameFeeder mFeeder = new H264FrameFeeder(this);
	
	MediaCodec mMediaCodec;
	MediaExtractor mMediaExtractor;
	
	SurfaceHolder mSurfaceHolder;
	Canvas mCanvas;
	ByteBuffer[] inputBuffers;
	ByteBuffer[] outputBuffers;
	
    public MediaCodecVideoView(Context context) 
    {
        this(context, null);
    }
    
    public MediaCodecVideoView(Context context, AttributeSet attrs)
    {   
        super(context, attrs);   
        
        setFocusable(true);
        this.mSurfaceHolder = this.getHolder();
        this.mSurfaceHolder.addCallback(this);
    }
    
	@Override
	public void surfaceCreated(SurfaceHolder holder) 
	{
		this.init();
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) 
	{
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) 
	{
		this.destroy();	
	}
	
	private void init() 
	{
		this.mFeeder.init(this.mWidth, this.mHeight);
		
		this.mMediaCodec = MediaCodec.createDecoderByType("video/avc");  
        MediaFormat mediaFormat = MediaFormat.createVideoFormat("video/avc", this.mWidth, this.mHeight);  
        this.mMediaCodec.configure(mediaFormat, this.mSurfaceHolder.getSurface(), null, 0);
        this.mMediaCodec.start();
        
        inputBuffers = this.mMediaCodec.getInputBuffers();  
        outputBuffers = this.mMediaCodec.getOutputBuffers(); 
	}

	private void destroy() 
	{
		this.mFeeder.destroy();
		if(this.mMediaCodec != null)
		{
			this.mMediaCodec.stop();
			this.mMediaCodec.release();
			this.mMediaCodec = null;
		}
	}
	
	public void putFrame(byte[] data)
	{
		this.mFeeder.putFrame(data);
	}

	
	@Override
	public void onFeedFrame(byte[] data) 
	{
    	
        int inputBufferIndex = this.mMediaCodec.dequeueInputBuffer(0);
        if (inputBufferIndex >= 0) 
        {  
            ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];  
            inputBuffer.clear();  
            inputBuffer.put(data, 0, data.length);
            //long cur_time = System.currentTimeMillis();
            //if(first_time == -1)
            //{
            //	first_time = cur_time;
            //	cur_time = 0;
            //}
            //else
            //{
            //	cur_time = cur_time - first_time;
            //}
            this.mMediaCodec.queueInputBuffer(inputBufferIndex, 0, data.length, 0, 0);
        }
        
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();  
        int outputBufferIndex = this.mMediaCodec.dequeueOutputBuffer(bufferInfo, 0);  
        while (outputBufferIndex >= 0) 
        {  
        	this.mMediaCodec.releaseOutputBuffer(outputBufferIndex, true);  
            outputBufferIndex = this.mMediaCodec.dequeueOutputBuffer(bufferInfo, 0);  
        }
        
	}
}
