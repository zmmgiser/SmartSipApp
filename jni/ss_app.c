#include "ss_app.h"

#define THIS_FILE "ss_app.c"

#ifndef SS_MAX_CALL
#   define SS_MAX_CALL    4
#endif

ss_app_config ss_app_cfg;

//#define TURN_SRV  "192.168.4.25:3478"
//#define STUN_SRV  "192.168.4.25:3478"
//#define TURN_SRV  "192.168.4.25:3478"
#define TURN_SRV  "numb.viagenie.ca"
#define STUN_SRV  "stun.ekiga.net:3478"
//#define TURN_SRV  "58.22.61.211:3478"
//#define STUN_SRV  "58.22.61.211:3478"
#define TURN_USR  "281318265@qq.com"
#define TURN_PWD  "123456"



/*
 * NAT type detection callback.
 */
static void on_nat_detect(const pj_stun_nat_detect_result *res)
{
    if (res->status != PJ_SUCCESS) {
	pjsua_perror(THIS_FILE, "NAT detection failed", res->status);
    } else {
	PJ_LOG(3, (THIS_FILE, "NAT detected as %s", res->nat_type_name));
    }
}

/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
			     pjsip_rx_data *rdata)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

	if(ss_app_cfg.on_app_incoming_call) {
		(*ss_app_cfg.on_app_incoming_call)(ci.remote_info.ptr);
		}
    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
			 (int)ci.remote_info.slen,
			 ci.remote_info.ptr));

    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
	// When media is active, connect call to sound device.
	pjsua_conf_connect(ci.conf_slot, 0);
	pjsua_conf_connect(0, ci.conf_slot);
    }
}

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;
	/*ss_inv_state state;*/
	
	PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);

	if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
		if(ss_app_cfg.on_app_call_state) {
		(*ss_app_cfg.on_app_call_state)(ci.remote_info.ptr, ci.state, ci.last_status_text.ptr);
		}
	}else {
		if(ss_app_cfg.on_app_call_state) {
			(*ss_app_cfg.on_app_call_state)(ci.remote_info.ptr, ci.state, NULL);
		}
	}
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
			 (int)ci.state_text.slen,
			 ci.state_text.ptr));
}

/**
 * 创建扩展视频设备
 *
 * @param cfg	    用户账户设置
 *
 * @return	    PJ_SUCCESS on success, or the error code.
 */
#if defined(PJMEDIA_VIDEO_DEV_HAS_EXTEND) && (PJMEDIA_VIDEO_DEV_HAS_EXTEND == 1)
static void video_put_frame(void **buf, pj_size_t *size,void **low_resolution_buf,pj_size_t *low_resolution_size)
{

}

static void video_get_frame(void *buf, pj_size_t size)
{
	if(ss_app_cfg.on_video_get_frame)
	{
		(*ss_app_cfg.on_video_get_frame)(buf, (int)size);
	}
}

pj_status_t pjsua_add_extend_video(pjsua_acc_config *cfg)
{
	{
		//创建视频传输设备 --extend-dev
		pjmedia_vid_dev_factory *extend_factory;
		pj_status_t status;
		status = pjmedia_extend_dev_create_factory(pjsua_get_pool_factory(),
												1,
												&extend_factory);
		if (status != PJ_SUCCESS) {
			PJ_PERROR(1,(THIS_FILE, status, "Error creating Extend factory"));
			return status;
		}
		{
			pjmedia_extend_dev_param extenddp;
			pjmedia_vid_dev_index extendid;

			pjmedia_extend_dev_param_default(&extenddp);
			extenddp.on_camera_put_frame=&video_put_frame;

			status =  pjmedia_extend_dev_alloc(extend_factory, &extenddp, &extendid);
			if (status != PJ_SUCCESS) {
				PJ_PERROR(1,(THIS_FILE, status,
						 "Error creating Extend Dev player"
						 ));
				return status;
				}
			PJ_LOG(4,(THIS_FILE, "Extend Dev player %.*s created, dev_id=%d",
				(int)extenddp.title.slen, extenddp.title.ptr, extendid));

			cfg->vid_cap_dev=extendid;
		}
	}

	{
		//创建视频输入设备 --extend-sdl-dev
		pjmedia_vid_dev_factory *extend_sdl_factory;
		pj_status_t status = pjmedia_extend_sdl_dev_create_factory(pjsua_get_pool_factory(),
											1,
											&extend_sdl_factory);
		if (status != PJ_SUCCESS) {
			PJ_PERROR(1,(THIS_FILE, status, "Error creating Extend SDL factory"));
			return status;
		}
		{
			pjmedia_extend_dev_param extend_sdl_dp;
			pjmedia_vid_dev_index extend_sdl_id;

			pjmedia_extend_sdl_dev_param_default(&extend_sdl_dp);
			extend_sdl_dp.on_camera_get_frame=&video_get_frame;

			status =  pjmedia_extend_sdl_dev_alloc(extend_sdl_factory, &extend_sdl_dp, &extend_sdl_id);
			if (status != PJ_SUCCESS) {
				PJ_PERROR(1,(THIS_FILE, status,
						 "Error creating Extend SDL Dev player"
						 ));
				return status;
				}
			PJ_LOG(4,(THIS_FILE, "Extend SDL Dev player %.*s created, dev_id=%d",
				(int)extend_sdl_dp.title.slen, extend_sdl_dp.title.ptr, extend_sdl_id));

			cfg->vid_rend_dev = extend_sdl_id;
		}
	}

	//开启视频传输
	//cfg->vid_out_auto_transmit = 1;
	cfg->vid_in_auto_show = 1;

	return PJ_SUCCESS;
}
#endif


int ss_get_state(ss_state *state) {
	int pj_state;
	pj_state = pjsua_get_state();
	*state = pj_state;
	return 0;
}


/* 初始化 */
int ss_app_init(ss_app_config *main_app_cfg, ss_app_log_config *main_app_log_cfg) {
	pj_status_t status;

	pj_memcpy(&ss_app_cfg, main_app_cfg, sizeof(ss_app_cfg));

    status = pjsua_create();

    /* Init pjsua */
    {
	pjsua_config cfg;
	pjsua_logging_config log_cfg;
	pjsua_media_config med_cfg;

	pjsua_config_default(&cfg);
	cfg.cb.on_incoming_call = &on_incoming_call;
	cfg.cb.on_call_media_state = &on_call_media_state;
	cfg.cb.on_call_state = &on_call_state;

	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = main_app_log_cfg->log_level;

#if defined (CUSTOM_LOG) && (CUSTOM_LOG == 1)
	log_cfg.cb = main_app_log_cfg->on_ss_app_log;
#endif

    pjsua_media_config_default(&med_cfg);

#if defined (USE_LOCAL_TRANSPORT) && (USE_LOCAL_TRANSPORT == 0)
    med_cfg.enable_ice = 1;
	//med_cfg.enable_turn=1;
	//med_cfg.turn_server=pj_str(TURN_SRV);
	//med_cfg.turn_auth_cred.type=PJ_STUN_AUTH_CRED_STATIC;
	//med_cfg.turn_auth_cred.data.static_cred.realm=pj_str("*");
	//med_cfg.turn_auth_cred.data.static_cred.username=pj_str(TURN_USR);
	//med_cfg.turn_auth_cred.data.static_cred.data_type = PJ_STUN_PASSWD_PLAIN;
	//med_cfg.turn_auth_cred.data.static_cred.data =pj_str(TURN_PWD);

	cfg.stun_srv_cnt = 1;
	cfg.stun_srv[0] = pj_str(STUN_SRV);
	cfg.cb.on_nat_detect=&on_nat_detect;
#endif

	PJ_LOG(4,(THIS_FILE, "start pjsua_init!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	status = pjsua_init(&cfg, &log_cfg, &med_cfg);
	PJ_LOG(4,(THIS_FILE, "end pjsua_init:%d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", status));
	}

#if defined (USE_LOCAL_TRANSPORT) && (USE_LOCAL_TRANSPORT == 0)
    //Add UDP transport.
    {
    	pjsua_transport_config cfg;
    	pjsua_transport_config_default(&cfg);
    	cfg.port = 5060;
    	PJ_LOG(4,(THIS_FILE, "start pjsua_transport_create!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
    	PJ_LOG(4,(THIS_FILE, "end pjsua_transport_create:%d!!!!!!!!!!!!!!!!!!!!!!!!", status));
	}
#endif

	if(status == PJ_SUCCESS)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/* 运行 */
int ss_app_run() {
	pj_status_t status;
	status = pjsua_start();

	if(status == PJ_SUCCESS)
		return 0;
	else
		return -1;
	}


/* 打电话 */
int ss_app_make_call(char *call_uri)
{
	pj_status_t status;
    pj_str_t uri = pj_str(call_uri);
	pjsua_acc_id acc_id = 0;
	int i;
	/*j = 0;*/
	for (i=0; i<SS_MAX_CALL; ++i) {
		if (pjsua_call_is_active(i)) {
			return -1;
		}
	}
	status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);

	if(status == PJ_SUCCESS)
		return 0;
	else
		return -1;
	}


/* 获得active call的列表 */
int ss_app_get_current_call_list(ss_call_list *ss_cl) {
	pj_status_t status;
	pjsua_call_info call_info;
	int i,j;
	j = 0;
	for (i=0; i<SS_MAX_CALL; ++i) {
		if (pjsua_call_is_active(i)) {
			status = pjsua_call_get_info(i, &call_info);
			ss_cl->ss_ci[j].remote_info.ptr = ss_cl->ss_ci[j].buf_.remote_info;
			ss_cl->ss_ci[j].remote_info.slen = 0;
			pj_memcpy(ss_cl->ss_ci[j].remote_info.ptr, call_info.remote_info.ptr, sizeof(ss_cl->ss_ci[j].buf_.remote_info));
			ss_cl->ss_ci[j].call_id = call_info.id;
			ss_cl->ss_ci[j].state = call_info.state;
      		ss_cl->ss_ci[j].duration_sec = call_info.connect_duration.sec;
			j++;
		}
    }
	ss_cl->len = j;
	if(status == PJ_SUCCESS)
		return 0;
	else
		return -1;
	}


/* 获得指定 call信息 */
int ss_app_get_call(char *who, ss_call_info *ss_ci) {
	pj_status_t status;
	int flag = -1;
	pj_str_t target;
	pj_str_t temp;
	char data[50];
	int i;
	pjsua_call_info call_info;

	temp = pj_str(who);
#if defined (SIP_URL_HAS_BRACKET) && (SIP_URL_HAS_BRACKET == 1)
	target.ptr =  data;
	target.slen = 0;
	pj_strcat2(&target,"<");
	pj_strcat(&target, &temp);
	pj_strcat2(&target,">");
#endif
	for (i=0; i<SS_MAX_CALL; ++i) {
		if (pjsua_call_is_active(i)) {
			status = pjsua_call_get_info(i, &call_info);
#if defined (SIP_URL_HAS_BRACKET) && (SIP_URL_HAS_BRACKET == 1)
			if (!pj_strcmp(&call_info.remote_info, &target))
#else
			if (!pj_strcmp(&call_info.remote_info, &temp))
#endif
			{
				flag = 0;
				/*printf("%s\n", call_info.remote_info.ptr);
				printf("%d\n", call_info.connect_duration.sec);*/
				ss_ci->remote_info.ptr = ss_ci->buf_.remote_info;
				ss_ci->remote_info.slen = 0;
				pj_memcpy(ss_ci->remote_info.ptr, call_info.remote_info.ptr, sizeof(ss_ci->buf_.remote_info));
				ss_ci->call_id = call_info.id;
				ss_ci->state = call_info.state;
      			ss_ci->duration_sec = call_info.connect_duration.sec;
				break;
			}
		}
    }
	if(flag == 0) {
		return 0;
	} else {
		return -1;
	}
}


/* 接听指定call */
int ss_app_answer_call(char *who) {
	pj_status_t status;
	int flag = -1;
	pj_str_t target;
	pj_str_t temp;
	char data[50];
	pjsua_call_id current_call = PJSUA_INVALID_ID;
	int i;
	pjsua_call_info call_info;

	temp = pj_str(who);
#if defined (SIP_URL_HAS_BRACKET) && (SIP_URL_HAS_BRACKET == 1)
	target.ptr =  data;
	target.slen = 0;
	pj_strcat2(&target,"<");
	pj_strcat(&target, &temp);
	pj_strcat2(&target,">");
#endif
	for (i=0; i<SS_MAX_CALL; ++i) {
		if (pjsua_call_is_active(i)) {
			status = pjsua_call_get_info(i, &call_info);
#if defined (SIP_URL_HAS_BRACKET) && (SIP_URL_HAS_BRACKET == 1)
			if (!pj_strcmp(&call_info.remote_info, &target))
#else
			if (!pj_strcmp(&call_info.remote_info, &temp))
#endif
			{
				/*printf("%s\n", call_info.remote_info.ptr);*/
				flag = 0;
				current_call = i;
				break;
			}
		}
    }

	if (flag == 0) {
		status = pjsua_call_answer(current_call, 200, NULL, NULL);
		if(status == PJ_SUCCESS) {
			return 0;
		} else {
			return -1;
		}
	}
	return -1;
}


/* 挂断指定call */
int ss_app_hangup(char *who) {
	pj_status_t status;
	int flag = -1;
	pj_str_t target;
	pj_str_t temp;
	char data[50];
	pjsua_call_id current_call = PJSUA_INVALID_ID;
	int i;
	pjsua_call_info call_info;

	temp = pj_str(who);
#if defined (SIP_URL_HAS_BRACKET) && (SIP_URL_HAS_BRACKET == 1)
	target.ptr =  data;
	target.slen = 0;
	pj_strcat2(&target,"<");
	pj_strcat(&target, &temp);
	pj_strcat2(&target,">");
#endif
	for (i=0; i<SS_MAX_CALL; ++i) {
		if (pjsua_call_is_active(i)) {
			status = pjsua_call_get_info(i, &call_info);
#if defined (SIP_URL_HAS_BRACKET) && (SIP_URL_HAS_BRACKET == 1)
			if (!pj_strcmp(&call_info.remote_info, &target))
#else
			if (!pj_strcmp(&call_info.remote_info, &temp))
#endif
			{
				/*printf("%s\n", call_info.remote_info.ptr);*/
				flag = 0;
				current_call = i;
				break;
			}
		}
    }

	if (flag == 0) {
		status = pjsua_call_hangup(current_call, 0, NULL, NULL);
		if(status == PJ_SUCCESS) {
			return 0;
		} else {
			return -1;
		}
	}
	return -1;
}


/* 挂断所有电话 */
int ss_app_hangup_all() {
	pjsua_call_hangup_all();
	return 0;
}


/* 销毁 */
int ss_app_destroy() {
	pj_status_t status;
	status = pjsua_destroy();
	if(status == PJ_SUCCESS) {
		return 0;
	} else {
		return -1;
	}
}

