package com.smart.smartsip;

import java.nio.ByteBuffer;


/**
 * SmartSip�ķ�װ��
 * ע�⣺���еĺ����������̰߳�ȫ�ģ����ڳ�ʼ���߳�ʹ�����к�����
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
		//����Ѿ����ڣ������ٹ���
		if(this.getAppState() != SmartSipAppState.SS_STATE_NULL)
		{
			this.destroy();
		}
	}

	/**
	 * ע��ص���
	 * @return
	 */
	private native void registerCallback();
	/**
	 * �����ص���
	 * @return
	 */
	private native void unregisterCallback();
	
	/**
	 * ��ʼ��ģ�顣ͬʱ��ע��ص���
	 * @return
	 */
	public native int init();
	/**
	 * ����ģ�顣
	 * @return
	 */
	public native int destroy();
	/**
	 * �ж�APP����״̬��
	 * �����Ŀ������ΪJava���ж���������JNI�㣬����1��instance�����Ž��̵Ĵ��ڶ����ڡ�
	 * @return
	 */
	public native int getAppState();
	/**
	 * ��绰��
	 * @param url : SIP�û���URL
	 * @return
	 */
	public native int makeCall(String url);
	/**
	 * �ӵ绰��
	 * @param url : �������緽�ӵ绰��
	 * @return
	 */
	public native int answerCall(String url);
	/**
	 * ��ĳһ�����硣
	 * @param url
	 * @return
	 */
	public native int hangUp(String url);
	/**
	 * �����е����硣
	 * @return
	 */
	public native int hangUpAll();
	/**
	 * ��ȡ���ڵ�ͨ����Ϣ�б�
	 * @return
	 */
	public native SmartSipCallInfo[] getCurrentCallList();
	/**
	 * ��ȡĳһ��ͨ����Ϣ��
	 * @param url
	 * @return
	 */
	public native SmartSipCallInfo getCurrentCall(String url);
	/**
	 * ����ص���
	 * @param url
	 */
	protected void onIncomingCall(String url)
	{
		
	}
	/**
	 * ����״̬�仯�ص���
	 * @param url
	 * @param state
	 * @param reason
	 */
	protected void onCallStateChanged(String url, int state, String reason)
	{
		
	}
	/**
	 * ��¼
	 * @param user
	 * @param passwd
	 * @param domain
	 * @return
	 */
	public native int login(String user, String passwd, String domain);
	/**
	 * ��ȡ��¼״̬
	 * @return
	 */
	public native int getRegisterState();
	/**
	 * ��ѯ�Է��������
	 * @param url
	 * @return
	 */
	public native int options(String url);
	/**
	 * �ǳ���
	 * @return
	 */
	public native int logout();
	/**
	 * ��¼״̬�ص���
	 * ֻ���ڵ�¼������²ſ��Դ�绰��
	 * @param state
	 */
	protected void onRegisterStateChanged(int state)
	{
		
	}
	/**
	 * �յ�Options�����
	 * @param code
	 */
	protected void onReceiveOptionsResponse(int code, String url)
	{
		
	}
	/**
	 * ������Ϣ��
	 * @param url
	 * @param mime
	 * @param text
	 * @param seq
	 * @return
	 */
	public native int sendMessage(String url, String mime, String text, int seq);
	/**
	 * �յ���Ϣ��Ӧ��
	 * @param code
	 * @param seq
	 * @param from
	 * @param data
	 */
	protected void onReceiveMessageResponse(int code, int seq, String from, String data)
	{
		
	}
	/**
	 * �յ���Ϣ����
	 * @param from
	 * @param data
	 * @return : ������Ӧ���ݡ�
	 */
	protected String onReceiveMessageRequest(String from, String data)
	{
		return "Hello";
	}
	
	/**
	 * �յ���Ƶ����
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