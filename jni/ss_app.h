#ifndef __SS_APP_H__
#define __SS_APP_H__

#include <pjsua-lib/pjsua.h>

#define CUSTOM_LOG 1
#define SIP_URL_HAS_BRACKET 0
#define USE_LOCAL_TRANSPORT 1

typedef enum ss_state
{
    /**
     * The library has not been initialized.
     */
    SS_STATE_NULL,

    /**
     * After pjsua_create() is called but before pjsua_init() is called.
     */
    SS_STATE_CREATED,

    /**
     * After pjsua_init() is called but before pjsua_start() is called.
     */
    SS_STATE_INIT,

    /**
     * After pjsua_start() is called but before everything is running.
     */
    SS_STATE_STARTING,

    /**
     * After pjsua_start() is called and before pjsua_destroy() is called.
     */
    SS_STATE_RUNNING,

    /**
     * After pjsua_destroy() is called but before the function returns.
     */
    SS_STATE_CLOSING

} ss_state;


/* �ַ��� */
typedef struct ss_str_t
{
	char *ptr;
	int slen;
} ss_str_t;


/* call��Ϣ */
typedef struct ss_call_info
{
	/* call id */
	int call_id;

	/*uri string*/
	ss_str_t remote_info;

	/*ͨ��ʱ�䣬��λ��s*/
	int duration_sec;

	/*ͨ��״̬*/
	int state;
	struct {
	char	remote_info[128];
    } buf_;
} ss_call_info;


/* call�б� */
typedef struct ss_call_list
{
	int len;
	ss_call_info ss_ci[4];
} ss_call_list;


typedef enum ss_inv_state
{
    SS_INV_STATE_NULL,	    /**< Before INVITE is sent or received  */
    SS_INV_STATE_CALLING,	    /**< After INVITE is sent		    */
    SS_INV_STATE_INCOMING,	    /**< After INVITE is received.	    */
    SS_INV_STATE_EARLY,	    /**< After response with To tag.	    */
    SS_INV_STATE_CONNECTING,	    /**< After 2xx is sent/received.	    */
    SS_INV_STATE_CONFIRMED,	    /**< After ACK is sent/received.	    */
    SS_INV_STATE_DISCONNECTED,   /**< Session is terminated.		    */
} ss_inv_state;


typedef struct ss_app_config
{
	/*
	��绰�ص�
	@param msg   ������Ϣ
	*/
	void (*on_app_make_call)(const char *msg);

	/*
	����ص�
	@param remote_uri   �������
	*/
	void (*on_app_incoming_call)(const char *remote_uri);

	/*
	call״̬�ص�
	@param state		call״̬
	@param reason		��ʧ�ܣ�ʧ��ԭ��
	*/
	void (*on_app_call_state)(char* remote_uri, ss_inv_state state, const char *reason);
#if defined(PJMEDIA_VIDEO_DEV_HAS_EXTEND) && (PJMEDIA_VIDEO_DEV_HAS_EXTEND == 1)
	/**
	 *�յ���Ƶ���ݡ�
	 */
	void (*on_video_get_frame)(char *buf, int size);

} ss_app_config;
#endif

/* ��ӡ */
typedef struct ss_app_log_config
{
	/*��ӡ�ȼ�*/
	unsigned log_level;

	/*��ӡ��Ϣ�ص�*/
	void (*on_ss_app_log)(int level, const char *data, int len);

} ss_app_log_config;



int ss_get_state(ss_state *state);
/*
��ʼ��
@param main_cfg		��ʼ���ṹ��

@return 0 �ɹ���-1 ʧ��
*/
int ss_app_init(ss_app_config *main_app_cfg, ss_app_log_config *main_log_config);

/*����
@return 0 �ɹ���-1 ʧ��
*/
int ss_app_run();

/*
��绰
@param call_uri	�����ַ

@return 0 �ɹ���-1 ʧ��
*/
int ss_app_make_call(char *call_uri);

/*
���active call���б�
@param call_l	active call���б�

@return 0 �ɹ���-1 ʧ��
*/
int ss_app_get_current_call_list(ss_call_list *ss_cl);

/*
���ָ�� call��Ϣ
@param who	ָ�� call
@param call_s	ָ�� call�ṹ��

@return 0 �ɹ���-1 ʧ��
*/
int ss_app_get_call(char *who, ss_call_info *ss_ci);

/*
��ָ�� call��Ϊ��ǰcall
@param call_s	ָ�� call

@return 0 �ɹ���-1 ʧ��
*/
int ss_app_set_call(char *who);

/*
����ָ��call
@param who	ָ�� call

@return 0 �ɹ���-1 ʧ��
*/
int ss_app_answer_call(char *who);

/*
�Ҷ�ָ��call
@param who	ָ�� call

@return 0 �ɹ���-1 ʧ��
*/
int ss_app_hangup(char *who);

/*�Ҷ����е绰
@return 0 �ɹ���-1 ʧ��
*/
int ss_app_hangup_all();

/*����
@return 0 �ɹ���-1 ʧ��
*/
int ss_app_destroy();

#endif	/* __SS_APP_H__ */
