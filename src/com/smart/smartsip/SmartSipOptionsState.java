package com.smart.smartsip;

/**
 * 查询对方在线状态
 * @author WangShuo
 *
 */
public class SmartSipOptionsState
{
	/**
	 * 离线 
	 */
	public static final int SS_OPT_STATE_ONLINE = 0;
	/**
	 * 在线
	 */
	public static final int SS_OPT_STATE_OFFLINE = 1;
	/**
	 * 未知
	 */
	public static final int SS_OPT_STATE_UNKOWN = 2;
	
	
	public static String getDescription(int state)
    {
    	switch (state) 
    	{
		case SS_OPT_STATE_ONLINE:
			return "在线";
		case SS_OPT_STATE_OFFLINE:
			return "离线";
		case SS_OPT_STATE_UNKOWN:
			return "未知";
		default:
			return "错误";
		}
    }
}