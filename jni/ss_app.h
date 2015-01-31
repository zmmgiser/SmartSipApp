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


/* 字符串 */
typedef struct ss_str_t
{
	char *ptr;
	int slen;
} ss_str_t;


/* call信息 */
typedef struct ss_call_info
{
	/* call id */
	int call_id;

	/*uri string*/
	ss_str_t remote_info;

	/*通话时间，单位：s*/
	int duration_sec;

	/*通话状态*/
	int state;
	struct {
	char	remote_info[128];
    } buf_;
} ss_call_info;


/* call列表 */
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
	打电话回调
	@param msg   返回信息
	*/
	void (*on_app_make_call)(const char *msg);

	/*
	来电回调
	@param remote_uri   来电号码
	*/
	void (*on_app_incoming_call)(const char *remote_uri);

	/*
	call状态回调
	@param state		call状态
	@param reason		若失败，失败原因
	*/
	void (*on_app_call_state)(char* remote_uri, ss_inv_state state, const char *reason);
#if defined(PJMEDIA_VIDEO_DEV_HAS_EXTEND) && (PJMEDIA_VIDEO_DEV_HAS_EXTEND == 1)
	/**
	 *收到视频数据。
	 */
	void (*on_video_get_frame)(char *buf, int size);

} ss_app_config;
#endif

/* 打印 */
typedef struct ss_app_log_config
{
	/*打印等级*/
	unsigned log_level;

	/*打印信息回调*/
	void (*on_ss_app_log)(int level, const char *data, int len);

} ss_app_log_config;



int ss_get_state(ss_state *state);
/*
初始化
@param main_cfg		初始化结构体

@return 0 成功，-1 失败
*/
int ss_app_init(ss_app_config *main_app_cfg, ss_app_log_config *main_log_config);

/*运行
@return 0 成功，-1 失败
*/
int ss_app_run();

/*
打电话
@param call_uri	拨打地址

@return 0 成功，-1 失败
*/
int ss_app_make_call(char *call_uri);

/*
获得active call的列表
@param call_l	active call的列表

@return 0 成功，-1 失败
*/
int ss_app_get_current_call_list(ss_call_list *ss_cl);

/*
获得指定 call信息
@param who	指定 call
@param call_s	指定 call结构体

@return 0 成功，-1 失败
*/
int ss_app_get_call(char *who, ss_call_info *ss_ci);

/*
将指定 call设为当前call
@param call_s	指定 call

@return 0 成功，-1 失败
*/
int ss_app_set_call(char *who);

/*
接听指定call
@param who	指定 call

@return 0 成功，-1 失败
*/
int ss_app_answer_call(char *who);

/*
挂断指定call
@param who	指定 call

@return 0 成功，-1 失败
*/
int ss_app_hangup(char *who);

/*挂断所有电话
@return 0 成功，-1 失败
*/
int ss_app_hangup_all();

/*销毁
@return 0 成功，-1 失败
*/
int ss_app_destroy();

#endif	/* __SS_APP_H__ */
