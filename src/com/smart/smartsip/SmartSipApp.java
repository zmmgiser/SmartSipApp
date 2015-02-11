package com.smart.smartsip;

import java.nio.ByteBuffer;


/**
 * SmartSip的封装。
 * 注意：所有的函数都不是线程安全的，请在初始化线程使用所有函数。
 * @author WangShuo
 *
 */
public class SmartSipApp
{
    private int mVideoWidth = 768;
    private int mVideoHeight = 432;
    private byte[] mVideoPixels;
    private ByteBuffer mVideoBuffer;
    
	
	public SmartSipApp()
	{
		this.registerCallback();
		//如果已经存在，则销毁过。
		if(this.getAppState() != SmartSipAppState.SS_STATE_NULL)
		{
			this.destroy();
		}
	}

	/**
	 * 注册回调。
	 * @return
	 */
	private native void registerCallback();
	/**
	 * 撤销回调。
	 * @return
	 */
	private native void unregisterCallback();
	
	/**
	 * 初始化模块。同时将注册回调。
	 * @return
	 */
	public native int init();
	/**
	 * 撤销模块。
	 * @return
	 */
	public native int destroy();
	/**
	 * 判断APP运行状态。
	 * 这个的目的是因为Java会有对象概念，而在JNI层，仅仅1个instance，随着进程的存在而存在。
	 * @return
	 */
	public native int getAppState();
	/**
	 * 打电话。
	 * @param url : SIP用户的URL
	 * @return
	 */
	public native int makeCall(String url);
	/**
	 * 接电话。
	 * @param url : 根据来电方接电话。
	 * @return
	 */
	public native int answerCall(String url);
	/**
	 * 挂某一个来电。
	 * @param url
	 * @return
	 */
	public native int hangUp(String url);
	/**
	 * 挂所有的来电。
	 * @return
	 */
	public native int hangUpAll();
	/**
	 * 获取当期的通话信息列表。
	 * @return
	 */
	public native SmartSipCallInfo[] getCurrentCallList();
	/**
	 * 获取某一个通话信息。
	 * @param url
	 * @return
	 */
	public native SmartSipCallInfo getCurrentCall(String url);
	/**
	 * 来电回调。
	 * @param url
	 */
	protected void onIncomingCall(String url)
	{
		
	}
	/**
	 * 来电状态变化回调。
	 * @param url
	 * @param state
	 * @param reason
	 */
	protected void onCallStateChanged(String url, int state, String reason)
	{
		
	}
	/**
	 * 登录
	 * @param user
	 * @param passwd
	 * @param domain
	 * @return
	 */
	public native int login(String user, String passwd, String domain);
	/**
	 * 获取登录状态
	 * @return
	 */
	public native int getRegisterState();
	/**
	 * 查询对方在线与否。
	 * @param url
	 * @return
	 */
	public native int options(String url);
	/**
	 * 登出。
	 * @return
	 */
	public native int logout();
	/**
	 * 登录状态回调。
	 * 只有在登录的情况下才可以打电话。
	 * @param state
	 */
	protected void onRegisterStateChanged(int state)
	{
		
	}
	/**
	 * 收到Options结果。
	 * @param code
	 */
	protected void onReceiveOptionsResponse(int code, String url)
	{
		
	}
	/**
	 * 发送消息。
	 * @param url
	 * @param mime
	 * @param text
	 * @param seq
	 * @return
	 */
	public native int sendMessage(String url, String mime, String text, int seq);
	/**
	 * 收到消息响应。
	 * @param code
	 * @param seq
	 * @param from
	 * @param data
	 */
	protected void onReceiveMessageResponse(int code, int seq, String from, String data)
	{
		
	}
	/**
	 * 收到消息请求。
	 * @param from
	 * @param data
	 * @return : 返回响应内容。
	 */
	protected String onReceiveMessageRequest(String from, String data)
	{
		return "Hello";
	}
	
	/**
	 * 收到视频数据
	 * @param data
	 */
	protected void onVideoGetFrame(byte[] data)
	{
		
	}
	
    static 
    {
        System.loadLibrary("ssapp");
    }
}