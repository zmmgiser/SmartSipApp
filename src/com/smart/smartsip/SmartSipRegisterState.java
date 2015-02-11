package com.smart.smartsip;

/**
 * 登录状态
 * @author WangShuo
 *
 */
public class SmartSipRegisterState
{
	/**
	 * 离线 
	 */
	public static final int SS_REG_STATE_OFFLINE = 0;
	/**
	 * 在线
	 */
	public static final int SS_REG_STATE_ONLINE = 1;
	
	
	public static String getDescription(int state)
    {
    	switch (state) 
    	{
		case SS_REG_STATE_OFFLINE:
			return "离线";
		case SS_REG_STATE_ONLINE:
			return "在线";
		default:
			return "错误";
		}
    }
}