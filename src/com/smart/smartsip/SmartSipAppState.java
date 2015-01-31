package com.smart.smartsip;

/**
 * APP״̬��
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
			return "δ����";
		case SS_STATE_CREATED:
			return "�Ѵ���";
		case SS_STATE_INIT:
			return "��ʼ��";
		case SS_STATE_STARTING:
			return "������...";
		case SS_STATE_RUNNING:
			return "��������";
		case SS_STATE_CLOSING:
			return "�ر���...";
		default:
			return "����";
		}
    }
}
