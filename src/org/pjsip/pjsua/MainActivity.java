package org.pjsip.pjsua;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;
import com.smart.smartsip.*;
import com.smart.smartsip.videocodec.MediaCodecVideoView;

import java.util.ArrayList;


public class MainActivity extends Activity 
{
    public static String TAG = "SmartSipApp";
    
    private Handler mHandler = new Handler();
    
    private SmartSipApp mPjsuaInstance;
    private ArrayList<SmartSipCallInfo> mCurrentCallList = new ArrayList<SmartSipCallInfo>();
    
    private TextView mTextViewRegisterState;
    private Button mButtonInit;
    private Button mButtonDestroy;
    private Button mButtonMakeCall;
    private Button mButtonMakeCall2;
    private Button mButtonSendMessage;
    private Button mButtonOptions;
    private Button mButtonHangupAll;
    private Button mButtonLogin;
    private Button mButtonLogoff;
    private EditText mEditTextTargetUrl;
    private EditText mEditTextTargetUrl2;
    private ListView mListViewCallList;
    
    private MediaCodecVideoView mVideoView;
	//private FFmpegVideoView mVideoView;

    /**
     *
     * @author yrc
     */
    private ViewFlipper allFlipper;
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            // TODO Auto-generated method stub
            switch (msg.what) {
                case 1:
                    //切换到主页面
                    allFlipper.setDisplayedChild(1);
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	Log.d(TAG, "=== Activity::onCreate() ===");
    	super.onCreate(savedInstanceState);
    	setContentView(R.layout.activity_main);
        allFlipper = (ViewFlipper) findViewById(R.id.allFlipper);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                handler.sendEmptyMessage(1); //给UI主线程发送消息
            }
        }, 3000); //启动等待3秒钟
    	//实例化Pjsua
    	mPjsuaInstance = new SmartSipApp() 
    	{
			@Override
			protected void onRegisterStateChanged(int state) 
			{
				Log.d(TAG, "onRegisterStateChanged:" + state);
				MainActivity.this.refreshRegisterState(state);
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
		    	MainActivity.this.refreshCallList();
			}

			@Override
			protected void onReceiveMessageResponse(int code, int seq, String from, String data)
			{
				MainActivity.this.toast("OnReceiveMessageResponse:code = " + SmartSipMessageState.getDescription(code) + ", seq = " + seq + ", from = " + from + ", data = " + data);
			}

			@Override
			protected String onReceiveMessageRequest(String from, String data)
			{
				MainActivity.this.toast("OnReceiveMessageRequest:" + from + ", data = " + data);
				return "Hello, " + from + "," + " I am MainActivity!";
			}
			@Override
			protected void onReceiveOptionsResponse(int code, String url)
			{
				MainActivity.this.toast("onReceiveOptionsResponse:" + url + ", state = " + SmartSipOptionsState.getDescription(code));
			}
			@Override
			protected void onVideoGetFrame(byte[] data)
			{
				//Log.d(TAG, "onVideoGetFrame start: size = " + data.length);
				synchronized (this) 
				{
					MainActivity.this.mVideoView.putFrame(data);
				}
				//Log.d(TAG, "onVideoGetFrame end");
			}
		};

    	
		
		mTextViewRegisterState = (TextView)findViewById(R.id.textview_register_state);
    	mButtonInit = (Button)findViewById(R.id.button_init);
    	mButtonDestroy = (Button)findViewById(R.id.button_destroy);
    	mButtonMakeCall = (Button)findViewById(R.id.button_makecall);
    	mButtonMakeCall2 = (Button)findViewById(R.id.button_makecall2);
    	mButtonSendMessage = (Button)findViewById(R.id.button_sendim);
    	mButtonOptions = (Button)findViewById(R.id.button_options);
    	mButtonHangupAll = (Button)findViewById(R.id.button_hangupall);
    	mButtonLogin = (Button)findViewById(R.id.button_login);
    	mButtonLogoff = (Button)findViewById(R.id.button_logoff);
    	mEditTextTargetUrl = (EditText)findViewById(R.id.edittext_callurl);
    	mEditTextTargetUrl2 = (EditText)findViewById(R.id.edittext_callurl2);
    	mListViewCallList = (ListView)findViewById(R.id.listview_calllist);
    	
    	mVideoView = (MediaCodecVideoView)findViewById(R.id.video_view);
		//mVideoView = (FFmpegVideoView)findViewById(R.id.video_view);

    	mButtonInit.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				int result = mPjsuaInstance.init();
				if(result != 0)
				{
					MainActivity.this.toast("init failed! result = " + result);
					return;
				}
				mHandler.post(new Runnable() 
				{
					@Override
					public void run() 
					{
						mButtonInit.setEnabled(false);
					}
				});
				MainActivity.this.refreshCallList();
			}
		});
    	mButtonDestroy.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				int result = mPjsuaInstance.destroy();
				if(result != 0)
				{
					MainActivity.this.toast("destroy failed! result = " + result);
					return;
				}
				mHandler.post(new Runnable() 
				{
					@Override
					public void run() 
					{
						mButtonInit.setEnabled(true);
					}
				});
			}
		});
    	mButtonMakeCall.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				String target_url = mEditTextTargetUrl.getText().toString();
				int result = mPjsuaInstance.makeCall("sip:" + target_url + "@192.168.4.25");
				if(result != 0)
				{
					MainActivity.this.toast("makeCall failed! result = " + result);
					return;
				}
			}
		});
    	mButtonMakeCall2.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				String target_url = mEditTextTargetUrl2.getText().toString();
				int result = mPjsuaInstance.makeCall("sip:" + target_url);
				//int result = mPjsuaInstance.makeCall("sip:" + "wangshuo1@ekiga.net");
				if(result != 0)
				{
					MainActivity.this.toast("makeCall2 failed! result = " + result);
					return;
				}
			}
		});
    	mButtonSendMessage.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				String target_url = mEditTextTargetUrl.getText().toString();
				int result = mPjsuaInstance.sendMessage("sip:" + target_url + "@192.168.4.25", 
						"text/plain", "Hello, I am MainActivity!", 1);
				if(result != 0)
				{
					MainActivity.this.toast("makeCall failed! result = " + result);
					return;
				}
			}
		});
    	mButtonOptions.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				String target_url = mEditTextTargetUrl.getText().toString();
				int result = mPjsuaInstance.options("sip:" + target_url + "@192.168.4.25");
				if(result != 0)
				{
					MainActivity.this.toast("options failed! result = " + result);
					return;
				}
			}
		});
    	mButtonHangupAll.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				int result = mPjsuaInstance.hangUpAll();
				if(result != 0)
				{
					MainActivity.this.toast("hangUpAll failed! result = " + result);
					return;
				}
			}
		});
    	mButtonLogin.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				int result = mPjsuaInstance.login("301", "123456", "192.168.4.25");
				if(result != 0)
				{
					MainActivity.this.toast("login failed! result = " + result);
					return;
				}
			}
		});
    	mButtonLogoff.setOnClickListener(new View.OnClickListener() 
    	{
			@Override
			public void onClick(View v) 
			{
				int result = mPjsuaInstance.logout();
				if(result != 0)
				{
					MainActivity.this.toast("logout failed! result = " + result);
					return;
				}
			}
		});
    	
    	//this.mVideoView = (H264VideoView)findViewById(R.id.video_view);
    	//this.mVideoView.PlayVideo("/sdcard/test.avi");//pure.h264");
    	//this.mVideoView.setVideoPath("/sdcard/test.avi");
    	//this.mVideoView.start();
    	
	}
    
    
    @Override
    protected void onStart() {
    	Log.d(TAG, "=== Activity::onStart() ===");
    	super.onStart();
    }
    
    @Override
    protected void onRestart() {
    	Log.d(TAG, "=== Activity::onRestart() ===");
    	super.onRestart();
    }

    @Override
    protected void onResume() {
    	Log.d(TAG, "=== Activity::onResume() ===");
    	super.onResume();
    	//this.refreshCallList();
    }

    @Override
    protected void onPause() {
    	Log.d(TAG, "=== Activity::onPause() ===");
    	super.onPause();
    }

    @Override
    protected void onStop() {
    	Log.d(TAG, "=== Activity::onStop() ===");
    	super.onStop();
    }

    @Override
    protected void onDestroy() {
    	Log.d(TAG, "=== Activity::onDestroy() ===");
    	super.onDestroy();
    	if(this.mPjsuaInstance != null)
    	{
    		this.mPjsuaInstance.destroy();
    		this.mPjsuaInstance = null;
    	}
    }
    
    @Override
    protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
		super.onRestoreInstanceState(savedInstanceState);
    }
    
    
    private void refreshRegisterState(final int state)
    {
    	mHandler.post(new Runnable() 
    	{
			@Override
			public void run() 
			{
				mTextViewRegisterState.setText("Register Status: " + SmartSipRegisterState.getDescription(state));
			}
		});
    }
    
    private void refreshCallList()
    {
    	synchronized (this.mCurrentCallList) 
    	{
    		//初始化当前列表
    		mCurrentCallList.clear();
    		SmartSipCallInfo[] calls = mPjsuaInstance.getCurrentCallList();
    		if(calls != null)
    		{
    			for (SmartSipCallInfo temp_call : calls) 
    			{
    				mCurrentCallList.add(temp_call);
    			}
    		}
		}
    	this.refreshCallListUI();
    }
    
    private void refreshCallListUI()
    {
    	mHandler.post(new Runnable() 
    	{
			@Override
			public void run() 
			{
				MyAdapter adapter = new MyAdapter(MainActivity.this);
		    	MainActivity.this.mListViewCallList.setAdapter(adapter);
			}
		});
    }
    
    private void toast(final String message)
    {
    	mHandler.post(new Runnable() 
    	{
    		@Override
			public void run() 
			{
    			Toast.makeText(MainActivity.this, message, Toast.LENGTH_SHORT).show();
			}
    	});
    }
    
    
    public class MyAdapter extends BaseAdapter
    {
    	 
        private LayoutInflater mInflater;
         
         
        public MyAdapter(Context context){
            this.mInflater = LayoutInflater.from(context);
        }
        @Override
        public int getCount() 
        {
        	synchronized (MainActivity.this.mCurrentCallList) 
        	{
				return mCurrentCallList.size();
			}
        }
 
        @Override
        public Object getItem(int arg0) 
        {
        	synchronized (MainActivity.this.mCurrentCallList) 
        	{
				return mCurrentCallList.get(arg0);
			}
        }
 
        @Override
        public long getItemId(int arg0) 
        {
            return 0;
        }
 
        @Override
        public View getView(int position, View convertView, ViewGroup parent) 
        {
             
            ViewHolder holder = null;
            if (convertView == null) 
            {
                 
                holder =new ViewHolder();  
                 
                convertView = mInflater.inflate(R.layout.call_list_adapter, null);
                holder.name = (TextView)convertView.findViewById(R.id.textview_calllist_name);
                holder.state = (TextView)convertView.findViewById(R.id.textview_calllist_state);
                holder.hangup = (Button)convertView.findViewById(R.id.button_calllist_hangup);
                convertView.setTag(holder);
                 
            }
            else 
            {
                 
                holder = (ViewHolder)convertView.getTag();
            }
             
            synchronized (MainActivity.this.mCurrentCallList) 
        	{
            	final SmartSipCallInfo call = MainActivity.this.mCurrentCallList.get(position);
            	holder.name.setText(call.getName());
            	holder.state.setText("state:" + SmartSipInviteState.getDescription(call.getState()));
            	holder.hangup.setOnClickListener(new View.OnClickListener() 
                { 
                    @Override
                    public void onClick(View v) 
                    {
                        int result = MainActivity.this.mPjsuaInstance.hangUp(call.getName());   
                        if(result != 0)
        				{
        					MainActivity.this.toast("hangUp " + call.getName() + " failed! result = " + result);
        					return;
        				}
                    }
                });
        	}
            return convertView;
        }
        

        private class ViewHolder
        {
        	TextView name;
        	TextView state;
        	Button hangup;
        }
    }
}
