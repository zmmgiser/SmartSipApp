package com.smart.smartsip;

/**
 * APP状态。
 * @author WangShuo
 *
 */
public class SmartSipAppState
{
	/**
     * The library has not been initialized.
     */
	public static final int SS_STATE_NULL = 0;

    /**
     * After pjsua_create() is called but before pjsua_init() is called.
     */
	public static final int SS_STATE_CREATED = 1;

    /**
     * After pjsua_init() is called but before pjsua_start() is called.
     */
	public static final int SS_STATE_INIT = 2;

    /**
     * After pjsua_start() is called but before everything is running.
     */
	public static final int SS_STATE_STARTING = 3;

    /**
     * After pjsua_start() is called and before pjsua_destroy() is called.
     */
	public static final int SS_STATE_RUNNING = 4;

    /**
     * After pjsua_destroy() is called but before the function returns.
     */
	public static final int SS_STATE_CLOSING = 5;

	
    public static String getDescription(int state)
    {
    	switch (state) 
    	{
		case SS_STATE_NULL:
			return "未启动";
		case SS_STATE_CREATED:
			return "已创建";
		case SS_STATE_INIT:
			return "初始化";
		case SS_STATE_STARTING:
			return "启动中...";
		case SS_STATE_RUNNING:
			return "正在运行";
		case SS_STATE_CLOSING:
			return "关闭中...";
		default:
			return "错误";
		}
    }
}
