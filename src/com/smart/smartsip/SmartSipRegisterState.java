package com.smart.smartsip;

/**
 * ��¼״̬
 * @author WangShuo
 *
 */
public class SmartSipRegisterState
{
	/**
	 * ���� 
	 */
	public static final int SS_REG_STATE_OFFLINE = 0;
	/**
	 * ����
	 */
	public static final int SS_REG_STATE_ONLINE = 1;
	
	
	public static String getDescription(int state)
    {
    	switch (state) 
    	{
		case SS_REG_STATE_OFFLINE:
			return "����";
		case SS_REG_STATE_ONLINE:
			return "����";
		default:
			return "����";
		}
    }
}