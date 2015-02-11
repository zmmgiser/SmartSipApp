package com.smart.smartsip;

/**
 * 通话状态
 * @author WangShuo
 *
 */
public class SmartSipInviteState
{
    public static final int SS_INV_STATE_NULL = 0;	    /**< Before INVITE is sent or received  */
    public static final int SS_INV_STATE_CALLING = 1;	    /**< After INVITE is sent		    */
    public static final int SS_INV_STATE_INCOMING = 2;	    /**< After INVITE is received.	    */
    public static final int SS_INV_STATE_EARLY = 3;	    /**< After response with To tag.	    */
    public static final int SS_INV_STATE_CONNECTING = 4;	    /**< After 2xx is sent/received.	    */
    public static final int SS_INV_STATE_CONFIRMED = 5;	    /**< After ACK is sent/received.	    */
    public static final int SS_INV_STATE_DISCONNECTED = 6;   /**< Session is terminated.		    */
    
    public static String getDescription(int state)
    {
    	switch (state) 
    	{
		case SS_INV_STATE_NULL:
			return "未启动";
		case SS_INV_STATE_CALLING:
			return "拨号中...";
		case SS_INV_STATE_INCOMING:
			return "来电中...";
		case SS_INV_STATE_EARLY:
			return "启动连接";
		case SS_INV_STATE_CONNECTING:
			return "连接中...";
		case SS_INV_STATE_CONFIRMED:
			return "正在通话";
		case SS_INV_STATE_DISCONNECTED:
			return "通话结束";
		default:
			return "错误";
		}
    }
}