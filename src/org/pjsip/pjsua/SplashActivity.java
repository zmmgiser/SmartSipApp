package org.pjsip.pjsua;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import com.smart.smartsip.SmartSipApp;

/**
 * Created by Hiccup on 2015-02-02.
 */
public class SplashActivity extends Activity {

    private mApplication app;
    private final int SPLASH_DISPLAY_LENGHT = 3000; // —”≥Ÿ»˝√Î
    private Handler sHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);
        app = (mApplication) getApplication();

        SmartSipApp mPjsuaInstance = app.getmPjsuaInstance();
        int result = mPjsuaInstance.init();
       /* Log.d("",(result);*/
        if(result != 0)
        {
           /* SplashActivity.this.toast("init failed! result = " + result);*/
            return;
        }
        sHandler.post(new Runnable()
        {
            @Override
            public void run()
            {
                Intent sIntent = new Intent(SplashActivity.this, MainActivity.class);
                startActivity(sIntent);
            }
        });
        /*new Handler().postDelayed(new Runnable() {
            public void run() {
                Intent mainIntent = new Intent(SplashActivity.this,
                        MainActivity.class);
                SplashActivity.this.startActivity(mainIntent);
                SplashActivity.this.finish();
            }

        }, SPLASH_DISPLAY_LENGHT);*/
    }
}
