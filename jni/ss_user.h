#ifndef __SS_USER_H__
#define __SS_USER_H__

typedef enum ss_reg_state
{
	SS_REG_STATE_OFFLINE,	    /**< 离线  */
	SS_REG_STATE_ONLINE,	    /**< 在线  */
} ss_reg_state;


typedef enum ss_opt_state
{
	SS_OPT_STATE_ONLINE,	    /**< 在线  */
	SS_OPT_STATE_OFFLINE,	    /**< 离线  */
	SS_OPT_STATE_UNKOWN,	    /**< 未知  */
} ss_opt_state;


typedef struct ss_user_config
{
	/*
	注册状态回调
	@param state   登录状态
	*/
	void (*on_user_reg_state)(ss_reg_state state);

	/*主动发送options触发的回调
	* @param code	状态码
	*/
	void (*on_recive_options_response)(ss_opt_state state, char *from);
} ss_user_config;


/*初始化
@ param main_cfg	初始化结构体

@return 0 成功，1 失败
*/
int ss_user_init(ss_user_config *main_user_cfg);


/*登录
@ param user	用户名
@ param passwd	密码
@ param domain	域

@return 0 成功，-1 失败
*/
int ss_user_login(char *user, char *passwd, char *domain);

/*获得登录状态
@return 0 成功，-1 失败
*/
int ss_user_get_register_state(ss_reg_state *state);

/*查询对方是否在线
@ param to	对方地址

@return 0 成功，-1 失败
*/
int ss_user_options(char *to);

/*登出当前帐号
@return 0 成功，-1 失败
*/
int ss_user_logout();


/*销毁
@return 0 成功，-1 失败
*/
int ss_user_destroy();


#endif	/* __SS_USER_H__ */