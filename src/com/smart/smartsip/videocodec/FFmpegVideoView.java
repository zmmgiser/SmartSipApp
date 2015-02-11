package com.smart.smartsip.videocodec;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import com.smart.ffmpeg.SmartFFmpeg;

import java.nio.ByteBuffer;

public class FFmpegVideoView extends SurfaceView implements SurfaceHolder.Callback, IFrameFeeder
{
    SmartFFmpeg mFfmpeg = new SmartFFmpeg();
    
    int mWidth = 768;
    int mHeight = 432;
    
    //int mWidth = 1280;
    //int mHeight = 720;
    //int mWidth = 720;
    //int mHeight = 480;
    //int mWidth = 1280;
    //int mHeight = 720;
    
    H264FrameFeeder mFeeder = new H264FrameFeeder(this);

    byte [] mPixel = null;
    ByteBuffer buffer = null;
	Bitmap mVideoBitmap = null;
	
	SurfaceHolder mSurfaceHolder;
	Canvas mCanvas;
	
    public FFmpegVideoView(Context context) 
    {
        this(context, null);
    }
    
    public FFmpegVideoView(Context context, AttributeSet attrs)
    {   
        super(context, attrs);   
        setFocusable(true);
        
        //设置SufaceHolder的Callback
        this.mSurfaceHolder = this.getHolder();
        this.mSurfaceHolder.addCallback(this);
        
    }
    
    /**
     * 这个被JNI调用，用于获取可以用来填充数据的Buffer
     * @param width
     * @param height
     * @return
     */
    public byte[] getBuffer(int width, int height)
    {
    	synchronized (this) 
    	{
        	if(this.mWidth != width || this.mHeight != height)
        	{
        		this.mWidth = width;
        		this.mHeight = height;
        		this.resetBuffer();
        	}
        	return this.mPixel;
		}
    }
    
    private void resetBuffer()
    {
    	this.mPixel = new byte[this.mWidth * this.mHeight * 2];
    	int i = mPixel.length;
        for(i = 0; i< mPixel.length; i++)
        {
        	mPixel[i]=(byte)0x00;
        }
    	this.buffer = ByteBuffer.wrap(this.mPixel);
    	if(this.mVideoBitmap != null)
    	{
    		this.mVideoBitmap.recycle();
    	}
    	this.mVideoBitmap = Bitmap.createBitmap(this.mWidth, this.mHeight, Config.RGB_565);
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
	
	public void init() 
	{
		this.mFeeder.init(this.mWidth, this.mHeight);
        //初始化Buffer
       	this.resetBuffer();
        //初始化FFmpeg
        this.mFfmpeg.init(this.mWidth, this.mHeight); 
	}

	public void destroy() 
	{
		this.mFeeder.destroy();
		this.mFfmpeg.destroy();
	}
	
	public void putFrame(byte[] data)
	{
		this.mFeeder.putFrame(data);
	}

	@Override
	public void onFeedFrame(byte[] data) 
	{
		long start_time = System.currentTimeMillis();
    	int iTemp = this.mFfmpeg.decodeFrame(data, data.length, this.mPixel);   
    	long end_time = System.currentTimeMillis();
    	Log.d("SmartSipApp", "decodeFrame use time = " + (end_time - start_time));
        if(iTemp > 0)
        {
            synchronized (this.mSurfaceHolder) 
            {  
            	this.mCanvas = this.mSurfaceHolder.lockCanvas();    
            	this.buffer.position(0);
                this.mVideoBitmap.copyPixelsFromBuffer(buffer);
                this.mCanvas.drawBitmap(this.mVideoBitmap, 0, 0, null); 
                this.mSurfaceHolder.unlockCanvasAndPost(this.mCanvas);  
            } 
            start_time = System.currentTimeMillis();
            Log.d("SmartSipApp", "draw use time = " + (start_time - end_time));
        }
	}
}
