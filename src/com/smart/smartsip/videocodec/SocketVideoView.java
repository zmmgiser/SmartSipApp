package com.smart.smartsip.videocodec;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaPlayer;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.smart.ffmpeg.SmartFFmpeg;

public class SocketVideoView extends SurfaceView implements SurfaceHolder.Callback
{
    
    int mWidth = 1280;
    int mHeight = 720;

    byte [] mPixel = null;
    ByteBuffer buffer = null;
	Bitmap mVideoBitmap = null;
	
	SurfaceHolder mSurfaceHolder;
	Canvas mCanvas;
	
	MediaPlayer mPlayer;
	LocalServerSocket mServer;
	LocalSocket mSocketServer;
	LocalSocket mSocketClient;
	
    public SocketVideoView(Context context) 
    {
        this(context, null);
    }
    
    public SocketVideoView(Context context, AttributeSet attrs)
    {   
        super(context, attrs);   
        setFocusable(true);
        //初始化Buffer
       	this.resetBuffer();
        //设置SufaceHolder的Callback
        this.mSurfaceHolder = this.getHolder();
        this.mSurfaceHolder.addCallback(this);
        

        
    }
    
    private void startLocalServer()
    {
    	Thread t = new Thread(new Runnable() 
    	{
			@Override
			public void run() 
			{

				try 
				{
					mServer = new LocalServerSocket("h264");
					mSocketServer = mServer.accept();
				} 
				catch (IOException e) 
				{
					e.printStackTrace();
				}
			}
		});
    	t.start();
    	try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
    private void startLocalClient()
    {
        this.mSocketClient = new LocalSocket();
        LocalSocketAddress address = new LocalSocketAddress("h264");
    	try 
    	{
			this.mSocketClient.connect(address);
		} 
    	catch (IOException e) 
    	{
			e.printStackTrace();
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
    
    public void putFrame(byte[] data)
    {
    	try 
    	{
			OutputStream os = this.mSocketServer.getOutputStream();
			os.write(data);
		}
    	catch (IOException e) 
    	{
			e.printStackTrace();
		}
    }

	@Override
	public void surfaceCreated(SurfaceHolder holder) 
	{
        this.startLocalServer();
        this.startLocalClient();
        
        this.mPlayer = new MediaPlayer();
        this.mPlayer.setSurface(this.mSurfaceHolder.getSurface());
        try 
        {
			this.mPlayer.setDataSource(this.mSocketClient.getFileDescriptor());
			this.mPlayer.prepare();
			this.mPlayer.start();
		} 
        catch (IllegalArgumentException e) 
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
        catch (IllegalStateException e) 
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
        catch (IOException e) 
        {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) 
	{
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) 
	{
		
	}
}
