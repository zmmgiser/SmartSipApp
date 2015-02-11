package com.smart.smartsip;

/**
 * ͨ��״̬
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
			return "δ����";
		case SS_INV_STATE_CALLING:
			return "������...";
		case SS_INV_STATE_INCOMING:
			return "������...";
		case SS_INV_STATE_EARLY:
			return "��������";
		case SS_INV_STATE_CONNECTING:
			return "������...";
		case SS_INV_STATE_CONFIRMED:
			return "����ͨ��";
		case SS_INV_STATE_DISCONNECTED:
			return "ͨ������";
		default:
			return "����";
		}
    }
}