#include "ss_app.h"
#include "ss_user.h"
#include <pjsua-lib/pjsua_internal.h>


#define THIS_FILE   "ss_user.c"

ss_user_config ss_user_cfg;



/* The module instance. */
static pjsip_module ss_user = 
{
    NULL, NULL,				/* prev, next.		*/
    { "mod-ss-user", 12 },		/* Name.		*/
    -1,					/* Id			*/
    PJSIP_MOD_PRIORITY_APPLICATION,	/* Priority	        */
    NULL,				/* load()		*/
    NULL,				/* start()		*/
    NULL,				/* stop()		*/
    NULL,				/* unload()		*/
    NULL,			/* on_rx_request()	*/
    NULL,				/* on_rx_response()	*/
    NULL,				/* on_tx_request.	*/
    NULL,				/* on_tx_response()	*/
    NULL,				/* on_tsx_state()	*/

};


static void user_callback(void *token, pjsip_event *e)
{
	pj_str_t from;
	ss_opt_state state;
    /*pjsua_im_data *im_data = (pjsua_im_data*) token;*/
	pjsip_rx_data *i_data;
	pjsua_acc_id acc_id = 0;
    if (e->type == PJSIP_EVENT_TSX_STATE) {

	pjsip_transaction *tsx = e->body.tsx_state.tsx;

	/* Ignore provisional response, if any */
	if (tsx->status_code < 200)
	    return;

	i_data = e->body.tsx_state.src.rdata;

	switch(tsx->status_code) {

		case 200: state = 0;break;

		case 404: state = 1;break;

		case 408: state = 1;break;

		default: state = 2;
	}

	if (e->body.tsx_state.type == PJSIP_EVENT_RX_MSG &&
	    (tsx->status_code == 401 || tsx->status_code == 407)) 
	{
		pjsip_rx_data *rdata = e->body.tsx_state.src.rdata;
		pjsip_tx_data *tdata;
	    pjsip_auth_clt_sess auth;
	    pj_status_t status;

		/*if(ss_user_cfg.on_recive_options_response) {
			(*ss_user_cfg.on_recive_options_response)(state, NULL);
		}*/

	    PJ_LOG(4,(THIS_FILE, "Resending OPTIONS with authentication"));

	    /* Create temporary authentication session */
	    pjsip_auth_clt_init(&auth,pjsua_var.endpt,rdata->tp_info.pool, 0);
    
	    pjsip_auth_clt_set_credentials(&auth, 
		pjsua_var.acc[acc_id].cred_cnt,
		pjsua_var.acc[acc_id].cred);

	    pjsip_auth_clt_set_prefs(&auth, 
				     &pjsua_var.acc[acc_id].cfg.auth_pref);

	    status = pjsip_auth_clt_reinit_req(&auth, rdata, tsx->last_tx,
					       &tdata);
	    if (status == PJ_SUCCESS) {
		/*pjsua_im_data *im_data2;*/

		/* Must duplicate im_data */
		/*im_data2 = pjsua_im_data_dup(tdata->pool, im_data);*/

		/* Increment CSeq */
		/*PJSIP_MSG_CSEQ_HDR(tdata->msg)->cseq++;*/

		/* Re-send request */
		status = pjsip_endpt_send_request( pjsua_var.endpt, tdata, -1,
						   NULL, &user_callback);
		if (status == PJ_SUCCESS) {
		    /* Done */
		    return;
		}
	    }
	}

	if (tsx->status_code/100 == 2) {
		from.ptr = (char*)pj_pool_alloc(i_data->tp_info.pool, PJSIP_MAX_URL_SIZE);
		/*在这里选择to里的uri是因为options里发送方与接收方的from头和to头都是一样的，
		所以对方发过来的信息也是from：发送方地址，to：接收方地址*/
		from.slen = pjsip_uri_print(PJSIP_URI_IN_FROMTO_HDR, 
				i_data->msg_info.to->uri,
				from.ptr, PJSIP_MAX_URL_SIZE);
		if(ss_user_cfg.on_recive_options_response){
			(*ss_user_cfg.on_recive_options_response)(state, from.ptr);
		}
	   /* PJ_LOG(4,(THIS_FILE, 
		      "Message \'%s\' delivered successfully",
		      im_data->body.ptr));*/
	} else {
		if(ss_user_cfg.on_recive_options_response){
			(*ss_user_cfg.on_recive_options_response)(state, NULL);
		}
	    /*PJ_LOG(3,(THIS_FILE, 
		      "Failed to deliver message \'%s\': %d/%.*s",
		      im_data->body.ptr,
		      tsx->status_code,
		      (int)tsx->status_text.slen,
		      tsx->status_text.ptr));*/
	}
	}
}


static int ss_user_module_login(char *user, char *passwd, char *domain)
{
#if defined (USE_LOCAL_TRANSPORT) && (USE_LOCAL_TRANSPORT == 0)
	pj_status_t status;
	pjsua_acc_id acc_id = 0;
	pj_str_t sip_user;
	pj_str_t sip_passwd;
	pj_str_t sip_domain;
	pjsua_acc_config cfg;
	pj_str_t temp1;
	pj_str_t temp2;

	char data1[30];
	char data2[30];

	sip_user = pj_str(user);
	sip_passwd = pj_str(passwd);
	sip_domain = pj_str(domain);

	pjsua_acc_config_default(&cfg);

#if defined(PJMEDIA_VIDEO_DEV_HAS_EXTEND) && (PJMEDIA_VIDEO_DEV_HAS_EXTEND == 1)
	//视频添加
	pjsua_add_extend_video(&cfg);
#endif

	//账户注册
	temp1.ptr =  data1;
	temp1.slen=0;
	temp2.ptr = data2;
	temp2.slen=0;
	pj_strcat2(&temp1,"sip:");
	pj_strcat(&temp1, &sip_user);
	pj_strcat2(&temp1,"@") ;
	pj_strcat(&temp1,&sip_domain);
	pj_strcat2(&temp2, "sip:");
	pj_strcat(&temp2, &sip_domain);
	cfg.id = temp1;
	cfg.reg_uri = temp2;
	cfg.cred_count = 1;
	cfg.cred_info[0].realm = sip_domain;
	cfg.cred_info[0].scheme = pj_str("digest");
	cfg.cred_info[0].username = sip_user;
	cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cfg.cred_info[0].data = sip_passwd;

	/*
	cfg.turn_cfg.enable_turn = 1;
	cfg.turn_cfg.turn_server = pj_str(TURN_SRV);
	cfg.turn_cfg.turn_auth_cred.type = PJ_STUN_AUTH_CRED_STATIC;
	cfg.turn_cfg.turn_auth_cred.data.static_cred.realm = pj_str("*");
	cfg.turn_cfg.turn_auth_cred.data.static_cred.username = pj_str(TURN_USR);
	cfg.turn_cfg.turn_auth_cred.data.static_cred.data_type = PJ_STUN_PASSWD_PLAIN;
	cfg.turn_cfg.turn_auth_cred.data.static_cred.data = pj_str(TURN_PWD);
	*/

	status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
	if (status == PJ_SUCCESS) {
		status = pjsua_acc_set_online_status(acc_id, PJ_TRUE);
	}
	if(status == PJ_SUCCESS) {
		return 0;
	} else {
		return -1;
	}

#else
	////本地账户
	{
		pjsua_acc_config cfg;
		pjsua_transport_config  rtp_cfg;
		pj_status_t status;
		pj_pool_t *tmp_pool;
		pjsua_acc_id aid;
		pjsip_transport_type_e type = PJSIP_TRANSPORT_UDP;
		pjsua_transport_id transport_id = -1;
		pjsua_transport_config  udp_cfg;
		pjsua_transport_config_default(&udp_cfg);
		udp_cfg.port = 5060;
		tmp_pool = pjsua_pool_create("tmp-pjsua", 1000, 1000);
		status = pjsua_transport_create(type,
						&udp_cfg,
						&transport_id);
		if (status != PJ_SUCCESS)
			printf("error");
		/* Add local account */
		pjsua_acc_add_local(transport_id, PJ_TRUE, &aid);
		pjsua_acc_get_config(aid, tmp_pool, &cfg);
		//pjsua_acc_config_default(&cfg);
	    //视频添加
#if defined(PJMEDIA_VIDEO_DEV_HAS_EXTEND) && (PJMEDIA_VIDEO_DEV_HAS_EXTEND == 1)
		//视频添加
		pjsua_add_extend_video(&cfg);
#endif
		pjsua_acc_modify(aid, &cfg);
		pj_pool_release(tmp_pool);
		return 0;
	}
#endif
}


static void on_user_module_reg_state(pjsua_acc_id acc_id)
{
	pj_status_t status;
			pjsua_acc_info acc_info;
			pjsua_acc_get_info(acc_id, &acc_info);
	if (ss_user_cfg.on_user_reg_state) {
			(*ss_user_cfg.on_user_reg_state)(acc_info.online_status);
		}
	if (acc_info.online_status == 0) {
		status = pjsua_acc_del(acc_id);
	}
}


/* 初始化 */
int ss_user_init(ss_user_config *main_user_cfg) {
	pj_status_t status;
	pj_memcpy(&ss_user_cfg, main_user_cfg, sizeof(ss_user_cfg));
	pjsua_var.ua_cfg.cb.on_reg_state = on_user_module_reg_state;
    status = pjsip_endpt_register_module(pjsua_var.endpt, &ss_user);
    if(status == PJ_SUCCESS) {
		return 0;
	} else {
		return -1;
	}
}


/* 登录 */
int ss_user_login(char *user, char *passwd, char *domain) {
	int state;
	pjsua_acc_id acc_id = 0;
	pjsua_acc_info acc_info;

	if (pjsua_acc_is_valid(acc_id)){
		pjsua_acc_get_info(acc_id, &acc_info);
		if (acc_info.online_status ==1) {
			return -1;
		} else {
			state = ss_user_module_login(user, passwd, domain);
			if(state == 0) 
				return 0;
			else
				return -1;
		}
	} else {
		state = ss_user_module_login(user, passwd, domain);
		if(state == 0)
			return 0;
		else
		return -1;
	}
}


/* 获得登录状态 */
int ss_user_get_register_state(ss_reg_state *state) {
	pj_status_t status;
	pjsua_acc_id acc_id = 0;
	pjsua_acc_info acc_info;
	status = pjsua_acc_get_info(acc_id, &acc_info);
	*state = acc_info.online_status;
	if(status == PJ_SUCCESS) {
		return 0;
	} else {
		return -1;
	}
}


/* 登出 */
int ss_user_logout() {
	pj_status_t status;
	pjsua_acc_id acc_id = 0;
	/*pjsua_acc_info acc_info;*/
	if (pjsua_acc_is_valid(acc_id)) {
		status = pjsua_acc_set_online_status(acc_id, PJ_FALSE);
		status = pjsua_acc_set_registration(acc_id, PJ_FALSE);
		/*status = pjsua_acc_del(acc_id);*/
		if (status == PJ_SUCCESS)
			return 0;
		else
			return -1;
	}
	return -1;
}


/* 查询对方是否在线 */
int ss_user_options(char *to) {
	pj_status_t status;
	pj_str_t server;
	pj_str_t target_url;
    pjsip_method method;
    pjsip_tx_data *tdata;
	/*pjsua_im_data *im_data;*/
	pjsua_acc *acc;
	pjsua_acc_id acc_id = 0;
	server = pj_str("sip:192.168.4.25:5060");
	acc = &pjsua_var.acc[acc_id];
    target_url = pj_str(to);

    pjsip_method_set(&method, PJSIP_OPTIONS_METHOD);
	/*im_data = PJ_POOL_ZALLOC_T(tdata->pool, pjsua_im_data);
    im_data->acc_id = acc_id;
    im_data->call_id = PJSUA_INVALID_ID;*/
    /*pj_strdup_with_null(tdata->pool, &im_data->to, target_url);*/
    status = pjsip_endpt_create_request( pjsua_var.endpt, &method, &target_url, &acc->cfg.id, &target_url,
					 NULL, NULL, -1, 
					 NULL, &tdata );
    if (status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "error: unable to create request", status);
	return 1;
    }
	status = pjsip_endpt_send_request( pjsua_var.endpt, tdata, -1, 
				       NULL, &user_callback);
	if (status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Unable to send request", status);
		return -1;	
    }

    return 0;
}

/* 销毁 */
int ss_user_destroy() {
	pj_status_t status;
	status = pjsip_endpt_unregister_module(pjsua_var.endpt, &ss_user);
	if(status == PJ_SUCCESS) {
		return 0;
	} else {
		return -1;
	}
}
