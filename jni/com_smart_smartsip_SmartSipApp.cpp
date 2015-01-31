#define LOG_TAG "SmartSipApp"
#define PJ_LOG_TAG "SmartSipPj"

#include <jni.h>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ss_app.h"
#include "ss_user.h"

#ifdef __cplusplus
}
#endif

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


static const char *SmartSipApp_classPathName = "com/smart/smartsip/SmartSipApp";
static const char *SmartSipCallInfo_classPathName = "com/smart/smartsip/SmartSipCallInfo";

JavaVM *android_jvm;
jclass android_ssapp_class;
jmethodID android_ssapp_on_user_reg_state_changed;
jmethodID android_ssapp_on_incoming_call;
jmethodID android_ssapp_on_call_state_changed;
jmethodID android_ssapp_on_video_get_frame;
jmethodID android_ssapp_on_recive_options_response;
jmethodID android_ssapp_on_recive_im_response;
jmethodID android_ssapp_on_recive_im_request;


jclass android_sscallinfo_class;
jmethodID android_sscallinfo_class_constructor;


jobject android_ssapp_instance = NULL;


static bool attach_jvm(JNIEnv **jni_env)
{
    if (android_jvm->GetEnv((void **)jni_env, JNI_VERSION_1_4) < 0)
    {
		//LOGD("attach_jvm!!!!!!!!!!!!!!!!!!!!!!!");
        if (android_jvm->AttachCurrentThread(jni_env, NULL) < 0)
        {
            jni_env = NULL;
            return false;
        }
        return true;
    }

    return false;
}

static void detach_jvm(bool attached)
{
	if(attached == true)
	{
		//LOGD("detach_jvm!!!!!!!!!!!!!!!!!!!!!!!");
		android_jvm->DetachCurrentThread();
	}
}

//#define detach_jvm(attached) \
//    if (attached) \
//        android_jvm->DetachCurrentThread();


static void on_app_log(int level, const char *data, int len)
{
	switch(level)
	{
		case 0:
		case 1:
			__android_log_print(ANDROID_LOG_ERROR, PJ_LOG_TAG, "%s", data);
			//LOGE("%s", data);
			break;
		case 2:
		case 3:
			__android_log_print(ANDROID_LOG_INFO, PJ_LOG_TAG, "%s", data);
			//LOGI("%s", data);
			break;
		case 4:
		case 5:
		case 6:
			__android_log_print(ANDROID_LOG_DEBUG, PJ_LOG_TAG, "%s", data);
			//LOGD("%s", data);
			break;
		default:
			break;
	}
}


//函数声明
static void on_app_make_call(const char *msg);
static void on_app_incoming_call(const char *remote_uri);
static void on_app_call_state(char* remote_uri, ss_inv_state state, const char *reason);
static void on_video_get_frame(char *buf, int size);
static void on_user_reg_state(ss_reg_state reg_state);
static void on_user_recive_options_response(ss_opt_state state, char *from);
static void on_recive_im_response(ss_status_code code, int seq, char *from, char *data);
static void on_recive_im_request(char *from, char *data, ss_response_msg *ss_msg);


/////////////////////////////////////////////////////////////////////////////////////////////
//start 主模块

/**
 * 主模块初始化
 */
static jint com_smart_smartsip_SmartSipApp_init(JNIEnv* env, jobject thiz)
{
	int result = 0;
	//基础回调
	ss_app_config main_ss_app_cfg;
	main_ss_app_cfg.on_app_make_call = on_app_make_call;
	main_ss_app_cfg.on_app_incoming_call = on_app_incoming_call;
	main_ss_app_cfg.on_app_call_state = on_app_call_state;
	main_ss_app_cfg.on_video_get_frame = on_video_get_frame;
	//打印回调
	ss_app_log_config main_ss_app_log_cfg;
	main_ss_app_log_cfg.log_level = 6;
	main_ss_app_log_cfg.on_ss_app_log = on_app_log;
	//基础模块初始化
	LOGD("start ss_app_init");
	result = ss_app_init(&main_ss_app_cfg, &main_ss_app_log_cfg);
	if(result != 0)
	{
		goto fail;
	}
	LOGD("end ss_app_init");
	//用户模块初始化
	ss_user_config main_ss_user_cfg;
	main_ss_user_cfg.on_user_reg_state = on_user_reg_state;
	main_ss_user_cfg.on_recive_options_response = on_user_recive_options_response;
	LOGD("start ss_user_init");
	result = ss_user_init(&main_ss_user_cfg);
	if(result != 0)
	{
		goto fail;
	}
	LOGD("end ss_user_init");
	//通讯模块初始化
	ss_im_config main_im_cfg;
	main_im_cfg.on_recive_im_response = on_recive_im_response;
	main_im_cfg.on_recive_im_request = on_recive_im_request;
	LOGD("start ss_im_init");
	result = ss_im_init(&main_im_cfg);
	if(result != 0)
	{
		goto fail;
	}
	LOGD("end ss_im_init");
	//运行基础模块
	LOGD("start ss_app_run");
	result = ss_app_run();
	if(result != 0)
	{
		goto fail;
	}
	LOGD("end ss_app_run");

	return result;

	fail:
	ss_app_destroy();

	return result;
}

static void com_smart_smartsip_SmartSipApp_unregisterCallback(JNIEnv* env, jobject thiz)
{
	if(android_ssapp_instance != NULL)
	{
		env->DeleteGlobalRef(android_ssapp_instance);
		android_ssapp_instance = NULL;
	}
}

static void com_smart_smartsip_SmartSipApp_registerCallback(JNIEnv* env, jobject thiz)
{
	com_smart_smartsip_SmartSipApp_unregisterCallback(env, thiz);
	android_ssapp_instance = env->NewGlobalRef(thiz);
}



/**
 * 主模块销毁
 */
static jint com_smart_smartsip_SmartSipApp_destroy(JNIEnv* env, jobject thiz)
{
	int result = 0;
	LOGD("start ss_app_destroy");
	result = ss_app_destroy();
	LOGD("end ss_app_destroy");
	return result;
}

/**
 * 判断APP运行状态。
 * 这个的目的是因为上层Java会有对象概念，而在JNI层，仅仅1个instance。
 */
static jint com_smart_smartsip_SmartSipApp_getAppState(JNIEnv* env, jobject thiz)
{
	int result = 0;
	ss_state state = SS_STATE_NULL;
	result = ss_get_state(&state);
	return (int)state;
}


/**
 * 打电话
 */
static jint com_smart_smartsip_SmartSipApp_makeCall(JNIEnv* env, jobject thiz, jstring call_uri)
{
	int result = 0;
	char *urlStr = (char*)(env->GetStringUTFChars(call_uri, NULL));
	result = ss_app_make_call(urlStr);
	env->ReleaseStringUTFChars(call_uri, urlStr);
	return result;
}

/**
 * 获取当期通话列表
 */
static jobjectArray com_smart_smartsip_SmartSipApp_getCurrentCallList(JNIEnv* env, jobject thiz)
{
	int result = 0;
	ss_call_list ss_cl;
	result = ss_app_get_current_call_list(&ss_cl);
	if(result != 0)
	{
		return NULL;
	}
	int length = ss_cl.len;
	jobjectArray array = (jobjectArray)env->NewObjectArray(length, android_sscallinfo_class, NULL);
	for(int i = 0; i < length; i++)
	{
		LOGD("com_smart_smartsip_SmartSipApp_getCurrentCallList[%d]:%s,state=%d!!!", i, ss_cl.ss_ci[i].remote_info.ptr, ss_cl.ss_ci[i].state);
		jstring j_name = env->NewStringUTF(ss_cl.ss_ci[i].remote_info.ptr);
		jobject j_call_info = env->NewObject(android_sscallinfo_class, android_sscallinfo_class_constructor, j_name,
				ss_cl.ss_ci[i].duration_sec, ss_cl.ss_ci[i].state);
		env->DeleteLocalRef(j_name);
		env->SetObjectArrayElement(array, i, j_call_info);
		env->DeleteLocalRef(j_call_info);
	}
	return array;
}

/**
 * 获取当前通话信息
 */
static jobject com_smart_smartsip_SmartSipApp_getCurrentCall(JNIEnv* env, jobject thiz, jstring call_url)
{
	int result = 0;
	ss_call_info ss_ci;

	char *call_url_Str = (char*)(env->GetStringUTFChars(call_url, NULL));
	result = ss_app_get_call(call_url_Str, &ss_ci);
	env->ReleaseStringUTFChars(call_url, call_url_Str);

	if(result != 0)
	{
		return NULL;
	}

	jstring j_name = env->NewStringUTF(ss_ci.remote_info.ptr);
	jobject j_call_info = env->NewObject(android_sscallinfo_class, android_sscallinfo_class_constructor, j_name,
			ss_ci.duration_sec, ss_ci.state);
	env->DeleteLocalRef(j_name);

	return j_call_info;
}

/**
 * 挂电话
 */
static jint com_smart_smartsip_SmartSipApp_hangUp(JNIEnv* env, jobject thiz, jstring call_uri)
{
	int result = 0;
	char *urlStr = (char*)(env->GetStringUTFChars(call_uri, NULL));
	LOGD("com_smart_smartsip_SmartSipApp_hangUp:%s!!!!\n", urlStr);
	result = ss_app_hangup(urlStr);
	env->ReleaseStringUTFChars(call_uri, urlStr);
	return result;
}

/**
 * 接电话
 */
static jint com_smart_smartsip_SmartSipApp_answerCall(JNIEnv* env, jobject thiz, jstring call_uri)
{
	int result = 0;
	char *urlStr = (char*)(env->GetStringUTFChars(call_uri, NULL));
	result = ss_app_answer_call(urlStr);
	env->ReleaseStringUTFChars(call_uri, urlStr);
	return result;
}

/**
 * 挂所有电话
 */
static jint com_smart_smartsip_SmartSipApp_hangUpAll(JNIEnv* env, jobject thiz)
{
	int result = 0;
	result = ss_app_hangup_all();
	return result;
}

//end 主模块
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//start 主模块回调

/**
 * 打电话回调
 */
static void on_app_make_call(const char *msg)
{
	LOGD("message:%s\n", msg);
}

/**
 * 来电回调
 */
static void on_app_incoming_call(const char *remote_uri)
{
	LOGD("call from:%s\n", remote_uri);
	JNIEnv* env;
	if(android_ssapp_instance == NULL)
	{
		return;
	}
	bool attached = attach_jvm(&env);

	jstring temp_uri = env->NewStringUTF(remote_uri);
	env->CallVoidMethod(android_ssapp_instance, android_ssapp_on_incoming_call, temp_uri);
	env->DeleteLocalRef(temp_uri);

	detach_jvm(attached);
}

/**
 * 连接状态变化回调
 */
static void on_app_call_state(char* remote_uri, ss_inv_state state, const char *reason)
{
	LOGD("remote_uri:%s, state:%d, reason:%s\n", remote_uri, state, reason);
	JNIEnv* env;
	if(android_ssapp_instance == NULL)
	{
		return;
	}
	bool attached = attach_jvm(&env);

	jstring temp_remote_uri = NULL;
	jstring temp_reason = NULL;

	if(remote_uri != NULL)
	{
		temp_remote_uri = env->NewStringUTF(remote_uri);
	}
	else
	{
		temp_remote_uri = env->NewStringUTF("");
	}
	if(reason != NULL)
	{
		temp_reason = env->NewStringUTF(reason);
	}
	else
	{
		temp_reason = env->NewStringUTF("");
	}


	env->CallVoidMethod(android_ssapp_instance, android_ssapp_on_call_state_changed, temp_remote_uri, (int)state, temp_reason);
	env->DeleteLocalRef(temp_remote_uri);
	env->DeleteLocalRef(temp_reason);

	detach_jvm(attached);
}

static void on_video_get_frame(char *buf, int size)
{
	LOGD("on_video_get_frame: size=%d", size);

	JNIEnv* env;
	if(android_ssapp_instance == NULL)
	{
		return;
	}
	bool attached = attach_jvm(&env);

	//通过建立byte[]来缓存。
	//并不合理，因为频繁的new byte[]。
	jbyteArray dataArray;
	dataArray = env->NewByteArray(size);
	env->SetByteArrayRegion(dataArray, 0, size, (jbyte*)buf);
    //jbyte* data = env->GetByteArrayElements(dataArray, NULL);
    //memcpy(data, buf, size);

	env->CallVoidMethod(android_ssapp_instance, android_ssapp_on_video_get_frame, dataArray);

	//env->ReleaseByteArrayElements(dataArray, (jbyte*)buf, JNI_ABORT);
	env->DeleteLocalRef(dataArray);

	detach_jvm(attached);
}

//end 主模块回调
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//start 用户模块
/**
 * 登录
 */
static jint com_smart_smartsip_SmartSipApp_login(JNIEnv* env, jobject thiz, jstring user, jstring passwd, jstring domain)
{
	int result = 0;
	char *userStr = (char*)(env->GetStringUTFChars(user, NULL));
	char *passwdStr = (char*)(env->GetStringUTFChars(passwd, NULL));
	char *domainStr = (char*)(env->GetStringUTFChars(domain, NULL));

	result = ss_user_login(userStr, passwdStr, domainStr);

	env->ReleaseStringUTFChars(user, userStr);
	env->ReleaseStringUTFChars(passwd, passwdStr);
	env->ReleaseStringUTFChars(domain, domainStr);

	return result;
}

/**
 * 获得登录状态
 */
static jint com_smart_smartsip_SmartSipApp_getRegisterState(JNIEnv* env, jobject thiz)
{
	int result = 0;
	ss_reg_state state = SS_REG_STATE_OFFLINE;

	result = ss_user_get_register_state(&state);
	if(result != 0)
	{
		return result;
	}
	return (int)state;
}

/**
 * 查询对方是否在线
 */
static jint com_smart_smartsip_SmartSipApp_options(JNIEnv* env, jobject thiz, jstring url)
{
	int result = 0;
	char *urlStr = (char*)(env->GetStringUTFChars(url, NULL));

	result = ss_user_options(urlStr);

	env->ReleaseStringUTFChars(url, urlStr);

	return result;
}

/**
 * 登出当前帐号
 */
static jint com_smart_smartsip_SmartSipApp_logout(JNIEnv* env, jobject thiz)
{
	int result = 0;

	result = ss_user_logout();

	return result;
}

//end 用户模块
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//start 用户模块回调

/**
 * Register状态变化回调处理
 */
static void on_user_reg_state(ss_reg_state state)
{
	LOGD("on_user_reg_state:%d\n", (int)state);
	JNIEnv* env;
	if(android_ssapp_instance == NULL)
	{
		return;
	}
	bool attached = attach_jvm(&env);

	env->CallVoidMethod(android_ssapp_instance, android_ssapp_on_user_reg_state_changed, (int)state);

	detach_jvm(attached);
}

/**
 * Options状态返回
 */
static void on_user_recive_options_response(ss_opt_state state, char *from)
{
	LOGD("on_user_recive_options_response:%s:%d\n", from, state);
	JNIEnv* env;
	if(android_ssapp_instance == NULL)
	{
		return;
	}
	bool attached = attach_jvm(&env);
	jstring temp_from = NULL;

	if(from != NULL)
	{
		temp_from = env->NewStringUTF(from);
	}
	else
	{
		temp_from = env->NewStringUTF("");
	}
	env->CallVoidMethod(android_ssapp_instance, android_ssapp_on_recive_options_response, (int)state, temp_from);
	env->DeleteLocalRef(temp_from);

	detach_jvm(attached);
}

//end 用户模块回调
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//start 通讯模块
static jint com_smart_smartsip_SmartSipApp_sendMessage(JNIEnv* env, jobject thiz, jstring url, jstring mime, jstring text, jint seq)
{
	int result = 0;
	char *urlStr = (char*)(env->GetStringUTFChars(url, NULL));
	char *mimeStr = (char*)(env->GetStringUTFChars(mime, NULL));
	char *textStr = (char*)(env->GetStringUTFChars(text, NULL));

	result = ss_im_send(urlStr, mimeStr, textStr, seq);

	env->ReleaseStringUTFChars(url, urlStr);
	env->ReleaseStringUTFChars(mime, mimeStr);
	env->ReleaseStringUTFChars(text, textStr);

	return result;
}
//end 通讯模块
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//start 通讯模块回调
static void on_recive_im_response(ss_status_code code, int seq, char *from, char *data)
{
	LOGD("code=%d, seq=%d, received from: %s, received data: %s!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", code, seq, from, data);
	JNIEnv* env;
	if(android_ssapp_instance == NULL)
	{
		return;
	}
	bool attached = attach_jvm(&env);
	jstring temp_from = NULL;
	jstring temp_data = NULL;

	if(from != NULL)
	{
		temp_from = env->NewStringUTF(from);
	}
	else
	{
		temp_from = env->NewStringUTF("");
	}
	if(data != NULL)
	{
		temp_data = env->NewStringUTF(data);
	}
	else
	{
		temp_data = env->NewStringUTF("");
	}

	env->CallVoidMethod(android_ssapp_instance, android_ssapp_on_recive_im_response, (int)code, seq, temp_from, temp_data);
	env->DeleteLocalRef(temp_from);
	env->DeleteLocalRef(temp_data);

	detach_jvm(attached);
}

//static char* s_reponse = (char *)"Hi, I got you!";
static char* none_reponse = (char *)"";

static void on_recive_im_request(char *from, char *data, ss_response_msg *ss_msg)
{
	LOGD("received from: %s, received data: %s!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", from, data);
	JNIEnv* env;
	if(android_ssapp_instance == NULL)
	{
		ss_msg->slen = strlen(none_reponse);
		memcpy(ss_msg->buf_, none_reponse, ss_msg->slen);
		return;
	}

	bool attached = attach_jvm(&env);
	jstring temp_from = NULL;
	jstring temp_data = NULL;

	if(from != NULL)
	{
		temp_from = env->NewStringUTF(from);
	}
	else
	{
		temp_from = env->NewStringUTF("");
	}
	if(data != NULL)
	{
		temp_data = env->NewStringUTF(data);
	}
	else
	{
		temp_data = env->NewStringUTF("");
	}

	jstring response = (jstring)env->CallObjectMethod(android_ssapp_instance, android_ssapp_on_recive_im_request, temp_from, temp_data);
	char *responseStr = (char*)(env->GetStringUTFChars(response, NULL));
	ss_msg->slen = strlen(responseStr);
	memcpy(ss_msg->buf_, responseStr, ss_msg->slen);
	env->ReleaseStringUTFChars(response, responseStr);

	env->DeleteLocalRef(temp_from);
	env->DeleteLocalRef(temp_data);
	env->DeleteLocalRef(response);

	detach_jvm(attached);
}

//end 通讯模块回调
/////////////////////////////////////////////////////////////////////////////////////////////

static JNINativeMethod methods[] =
{
	//基础模块
	{ "registerCallback", "()V", (void*) com_smart_smartsip_SmartSipApp_registerCallback },
	{ "unregisterCallback", "()V", (void*) com_smart_smartsip_SmartSipApp_unregisterCallback },
	{ "init", "()I", (void*) com_smart_smartsip_SmartSipApp_init },
	{ "destroy", "()I", (void*) com_smart_smartsip_SmartSipApp_destroy },
	{ "getAppState", "()I", (void*) com_smart_smartsip_SmartSipApp_getAppState },
	{ "makeCall", "(Ljava/lang/String;)I", (void*) com_smart_smartsip_SmartSipApp_makeCall },
	{ "getCurrentCallList", "()[Lcom/smart/smartsip/SmartSipCallInfo;", (void*) com_smart_smartsip_SmartSipApp_getCurrentCallList },
	{ "getCurrentCall", "(Ljava/lang/String;)Lcom/smart/smartsip/SmartSipCallInfo;", (void*) com_smart_smartsip_SmartSipApp_getCurrentCall },
	{ "answerCall", "(Ljava/lang/String;)I", (void*) com_smart_smartsip_SmartSipApp_answerCall },
	{ "hangUp", "(Ljava/lang/String;)I", (void*) com_smart_smartsip_SmartSipApp_hangUp },
	{ "hangUpAll", "()I", (void*) com_smart_smartsip_SmartSipApp_hangUpAll },
	//用户模块
	{ "login", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", (void*) com_smart_smartsip_SmartSipApp_login },
	{ "getRegisterState", "()I", (void*) com_smart_smartsip_SmartSipApp_getRegisterState },
	{ "options", "(Ljava/lang/String;)I", (void*) com_smart_smartsip_SmartSipApp_options },
	{ "logout", "()I", (void*) com_smart_smartsip_SmartSipApp_logout },
	//通讯模块
	{ "sendMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)I", (void*) com_smart_smartsip_SmartSipApp_sendMessage },
};

/**
 * 注册Native函数
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL)
        return JNI_FALSE;

    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
    {
    LOGE("register nativers error");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}


/**
 *初始化Classes
 */
static void initClasses(JNIEnv* env)
{
	jclass clazz;
	clazz = env->FindClass(SmartSipApp_classPathName);
	android_ssapp_class = (jclass)env->NewGlobalRef(clazz);
	//基础模块
	android_ssapp_on_incoming_call = env->GetMethodID(clazz, "onIncomingCall", "(Ljava/lang/String;)V");
	android_ssapp_on_call_state_changed = env->GetMethodID(clazz, "onCallStateChanged", "(Ljava/lang/String;ILjava/lang/String;)V");
	android_ssapp_on_video_get_frame = env->GetMethodID(clazz, "onVideoGetFrame", "([B)V");
	//用户模块
	android_ssapp_on_user_reg_state_changed = env->GetMethodID(clazz, "onRegisterStateChanged", "(I)V");
	android_ssapp_on_recive_options_response = env->GetMethodID(clazz, "onReceiveOptionsResponse", "(ILjava/lang/String;)V");
	//通讯模块
	android_ssapp_on_recive_im_response = env->GetMethodID(clazz, "onReceiveMessageResponse", "(IILjava/lang/String;Ljava/lang/String;)V");
	android_ssapp_on_recive_im_request = env->GetMethodID(clazz, "onReceiveMessageRequest", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

	clazz = env->FindClass(SmartSipCallInfo_classPathName);
	android_sscallinfo_class = (jclass)env->NewGlobalRef(clazz);
	android_sscallinfo_class_constructor = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;II)V");
}

extern "C" int android_aud_dev_JNI_OnLoad(JavaVM* vm, void* reserved);

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	android_jvm = vm;
	JNIEnv* env = NULL;
	jint result = -1;

	LOGI("Entering JNI_OnLoad!!!!!!!!!!!!!\n");

	//pjmedia的audio dev JNI_OnLoad
	android_aud_dev_JNI_OnLoad(vm, reserved);

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		goto bail;
	}
	assert(env != NULL);
	if (!registerNativeMethods(env, SmartSipApp_classPathName, methods, sizeof(methods) / sizeof(methods[0])))
	{
		goto bail;
	}

	initClasses(env);

	//success -- return valid version number
	result = JNI_VERSION_1_4;

	bail: LOGE("Leaving JNI_OnLoad (result=0x%x)\n", result);
	return result;
}
