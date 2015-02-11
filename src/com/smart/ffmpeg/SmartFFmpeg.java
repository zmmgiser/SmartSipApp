package com.smart.ffmpeg;


public class SmartFFmpeg  
{
	
	public SmartFFmpeg() 
	{
		
	}
	
    static 
    {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("smartffmpeg");
    }
	
	public native int init(int width, int height);
	public native int destroy(); 
	public native int decodeFrame(byte[] in, int insize, byte[] out);
}