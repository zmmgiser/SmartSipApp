package com.smart.smartsip;

/**
 * ��ѯ�Է�����״̬
 * @author WangShuo
 *
 */
public class SmartSipOptionsState
{
	/**
	 * ���� 
	 */
	public static final int SS_OPT_STATE_ONLINE = 0;
	/**
	 * ����
	 */
	public static final int SS_OPT_STATE_OFFLINE = 1;
	/**
	 * δ֪
	 */
	public static final int SS_OPT_STATE_UNKOWN = 2;
	
	
	public static String getDescription(int state)
    {
    	switch (state) 
    	{
		case SS_OPT_STATE_ONLINE:
			return "����";
		case SS_OPT_STATE_OFFLINE:
			return "����";
		case SS_OPT_STATE_UNKOWN:
			return "δ֪";
		default:
			return "����";
		}
    }
}