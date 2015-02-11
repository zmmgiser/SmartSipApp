package com.smart.smartsip.videocodec;

import android.util.Log;


public class H264FrameFeeder
{
	//这个是768*432的HEADER
	private static byte[] HEADER_768_432 = 
		{
		0x0,0x0,0x1,0x67,0x42,(byte)0xc0,0x1e,(byte)0xb6,(byte)0x80,(byte)0xc0,0x37,(byte)0xa1,
		0x0,0x0,0x3,0x0,0x1,0x0,0x0,0x3,0x0,0x32,(byte)0x8f,0x16,0x2e,(byte)0xa0,
		0x0,0x0,0x1,0x68,(byte)0xce,0x3c,(byte)0x80,
		0x0,0x0,0x1,0x6,0x6,0x2,0x5,(byte)0xf1,(byte)0x80
		};
	private static byte[] HEADER_1280_720 = 
		{0x0,0x0,0x1,0x67,0x42,(byte)0xc0,0x1f,(byte)0x91,(byte)0xa0,0x14,0x1,0x6e,(byte)0x84,0x0,0x0,0x3,0x0,0x4,0x0,0x0,0x3,0x0,(byte)0xca,0x3c,0x60,(byte)0xca,(byte)0x80,
		0x0,0x0,0x1,0x68,(byte)0xce,0x3c,(byte)0x80};
	
    private int mWidth = 1280;
    private int mHeight = 720;
    private boolean mFoundHeader = false;
    private boolean mRunning = false;
    private int mFrameIndex = 0;
	private IFrameFeeder mClient = null;
	
	public H264FrameFeeder(IFrameFeeder client)
	{
		this.mClient = client;
	}
	
	public void init(int width, int height)
	{
		this.mRunning = true;
		this.mFoundHeader = false;
		this.mWidth = width;
		this.mHeight = height;
	}
	
	public void destroy()
	{
		this.mRunning = false;
		this.mFoundHeader = false;
	}
	
	public void putFrame(byte[] data)
	{
    	Log.d("SmartSipApp", "putFrame : " + data.length + "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		if(this.mRunning == false || this.mClient == null || data.length < 4)
		{
			Log.e("SmartSipApp", "putFrame unsupport!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			return;
		}
		byte type = 0;
		boolean is_nalu = false;
		//查看是否为NALU单元
		if(data[0] == 0 && data[1] == 0)
		{
			if(data[2] == 1)
			{
				is_nalu = true;
				type = (byte) (data[3] & 0x1F);
			}
			else if(data[2] == 0 && data[3] == 1)
			{
				is_nalu = true;
				type = (byte) (data[4] & 0x1F);
			}
		}
		//非NALU，则返回
		if(is_nalu == false)
		{
	    	Log.e("SmartSipApp", "not nalu, return!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			return;
		}
		this.mFrameIndex ++;
		//如果还没找到头，则需要解析头判断
        if(this.mFoundHeader == false)
        {
        	//SPS头
	        //if(type == 7)
	        if(type == 7)
	        {
	        	this.mFoundHeader = true;
	        	Log.d("SmartSipApp", "found header!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	        }
	        //非SPS头
	        else
	        {
	        	//this.mFoundHeader = true;
	        	//this.mClient.onFeedFrame(HEADER_1280_720);
		    	Log.e("SmartSipApp", "not found header, return!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	        	return;
	        }
        }
        Log.d("SmartSipApp", "" + this.mFrameIndex + " : putFrame[" + data.length + "]:\n" + 
        		"0x" + Integer.toHexString(data[0] & 0xFF) + "," + 
        		"0x" + Integer.toHexString(data[1] & 0xFF) + "," + 
        		"0x" + Integer.toHexString(data[2] & 0xFF) + "," + 
        		"0x" + Integer.toHexString(data[3] & 0xFF) + "," + 
        		"0x" + Integer.toHexString(data[4] & 0xFF) + "," + 
        		"0x" + Integer.toHexString(data[5] & 0xFF) + "," + 
        		"0x" + Integer.toHexString(data[6] & 0xFF) + "," + 
        		"0x" + Integer.toHexString(data[7] & 0xFF) + "," + 
        		"!!!!!!!!!!!!!!!!!!!!!");
		
        if(this.random(95,  100) == true)
        {
        	//return;
        }
		//long start_time = System.currentTimeMillis(); 
        this.mClient.onFeedFrame(data);
    	//long end_time = System.currentTimeMillis();
        //Log.d("SmartSipApp", "draw use time = " + (end_time - start_time));
	}
	
	private boolean random(int a, int b)
	{
		return Math.random() * b > a;
	}
}