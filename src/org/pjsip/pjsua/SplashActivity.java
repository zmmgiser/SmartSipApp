package org.pjsip.pjsua;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.widget.Toast;
import com.smart.smartsip.SmartSipApp;

public class SplashActivity extends Activity {

    private SoleApplication app;
    private final int SPLASH_DISPLAY_LENGHT = 1000; // —”≥Ÿ“ª√Î
    private Handler sHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);
        app = (SoleApplication) getApplication();

        SmartSipApp mPjsuaInstance = app.getPjsuaInstance();
        int result = mPjsuaInstance.init();
        if(result != 0)
        {
            SplashActivity.this.toast("init failed!!!!!!!!!!!!! result = " + result);
            return;
        }
        SplashActivity.this.toast("init succeeded!");
        /*sHandler.post(new Runnable()
        {
            @Override
            public void run()
            {
                Intent sIntent = new Intent(SplashActivity.this, MainActivity.class);
                startActivity(sIntent);
            }
        });*/
        sHandler.postDelayed(new Runnable() {
            public void run() {
                Intent mainIntent = new Intent(SplashActivity.this,
                        MainActivity.class);
                SplashActivity.this.startActivity(mainIntent);
                SplashActivity.this.finish();
            }

        }, SPLASH_DISPLAY_LENGHT);
    }

    private void toast(final String message)
    {
        sHandler.post(new Runnable()
        {
            @Override
            public void run()
            {
                Toast.makeText(SplashActivity.this, message, Toast.LENGTH_SHORT).show();
            }
        });
    }
}
