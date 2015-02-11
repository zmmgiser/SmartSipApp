#ifndef __SS_USER_H__
#define __SS_USER_H__

typedef enum ss_reg_state
{
	SS_REG_STATE_OFFLINE,	    /**< ����  */
	SS_REG_STATE_ONLINE,	    /**< ����  */
} ss_reg_state;


typedef enum ss_opt_state
{
	SS_OPT_STATE_ONLINE,	    /**< ����  */
	SS_OPT_STATE_OFFLINE,	    /**< ����  */
	SS_OPT_STATE_UNKOWN,	    /**< δ֪  */
} ss_opt_state;


typedef struct ss_user_config
{
	/*
	ע��״̬�ص�
	@param state   ��¼״̬
	*/
	void (*on_user_reg_state)(ss_reg_state state);

	/*��������options�����Ļص�
	* @param code	״̬��
	*/
	void (*on_recive_options_response)(ss_opt_state state, char *from);
} ss_user_config;


/*��ʼ��
@ param main_cfg	��ʼ���ṹ��

@return 0 �ɹ���1 ʧ��
*/
int ss_user_init(ss_user_config *main_user_cfg);


/*��¼
@ param user	�û���
@ param passwd	����
@ param domain	��

@return 0 �ɹ���-1 ʧ��
*/
int ss_user_login(char *user, char *passwd, char *domain);

/*��õ�¼״̬
@return 0 �ɹ���-1 ʧ��
*/
int ss_user_get_register_state(ss_reg_state *state);

/*��ѯ�Է��Ƿ�����
@ param to	�Է���ַ

@return 0 �ɹ���-1 ʧ��
*/
int ss_user_options(char *to);

/*�ǳ���ǰ�ʺ�
@return 0 �ɹ���-1 ʧ��
*/
int ss_user_logout();


/*����
@return 0 �ɹ���-1 ʧ��
*/
int ss_user_destroy();


#endif	/* __SS_USER_H__ */