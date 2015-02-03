package org.pjsip.pjsua;

import android.app.Application;
import android.util.Log;
import com.smart.smartsip.SmartSipApp;

/**
 * Created by Hiccup on 2015-02-02.
 */
public class SoleApplication extends Application{

    public static String TAG = "SmartSipApp";
    private SmartSipApp mPjsuaInstance;

    @Override
    public void onCreate() {
        super.onCreate();
        mPjsuaInstance = new SmartSipApp()
        {
            @Override
            protected void onRegisterStateChanged(int state)
            {
                Log.d(TAG, "onRegisterStateChanged:" + state);
                /*mApplication.this.refreshRegisterState(state);*/
            }

            @Override
            protected void onIncomingCall(String url)
            {
                Log.d(TAG, "onIncomingCall:" + url);
            }

            @Override
            protected void onCallStateChanged(String url, int state, String reason)
            {
                Log.d(TAG, "onCallStateChanged:" + url + ", state = " + state);
                /*mApplication.this.refreshCallList();*/
            }

            @Override
            protected void onReceiveMessageResponse(int code, int seq, String from, String data)
            {
                /*mApplication.this.toast("OnReceiveMessageResponse:code = " + SmartSipMessageState.getDescription(code) + ", seq = " + seq + ", from = " + from + ", data = " + data);*/
            }

            @Override
            protected String onReceiveMessageRequest(String from, String data)
            {
                /*mApplication.this.toast("OnReceiveMessageRequest:" + from + ", data = " + data);*/
                return "Hello, " + from + "," + " I am MainActivity!";
            }
            @Override
            protected void onReceiveOptionsResponse(int code, String url)
            {
                /*mApplication.this.toast("onReceiveOptionsResponse:" + url + ", state = " + SmartSipOptionsState.getDescription(code));*/
            }
            @Override
            protected void onVideoGetFrame(byte[] data)
            {
                //Log.d(TAG, "onVideoGetFrame start: size = " + data.length);
                synchronized (this)
                {
                   /* mApplication.this.mVideoView.putFrame(data);*/
                }
                //Log.d(TAG, "onVideoGetFrame end");
            }
        };
    }

    public SmartSipApp getPjsuaInstance()
    {
        return mPjsuaInstance;
    }
}
