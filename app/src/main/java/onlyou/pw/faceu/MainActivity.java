package onlyou.pw.faceu;


import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.IOException;

@SuppressWarnings("deprecation")
public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    final String src = Environment.getExternalStorageDirectory()+"/mkv.mkv";
//    final String src = Environment.getExternalStorageDirectory()+"/ffmpeg.mp4";
    final String out = Environment.getExternalStorageDirectory()+"/ff_yuv123.pcm";
    SurfaceView sur;
    TextView progress;
    Button forward10,back10,cut;
    int current_time;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        sur=findViewById(R.id.surfaceView);
        progress=findViewById(R.id.progress);
        Button play = findViewById(R.id.play);
        play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread(){
                    @Override
                    public void run() {
                        super.run();
                        play(src,out,sur.getHolder().getSurface());
                    }
                };
                thread.start();
//                test(src);
            }
        });
        final Button pause = findViewById(R.id.pause);
        pause.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                    pause();
            }
        });
        final Button continue_ = findViewById(R.id.continue_);
        continue_.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                    continue_();
            }
        });


        forward10=findViewById(R.id.forward10);
        forward10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                forword(current_time+10);
            }
        });


        back10=findViewById(R.id.back10);
        back10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                forword(current_time-10);
            }
        });

        cut=findViewById(R.id.cut);
        cut.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                cut();
            }
        });

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},1);
        }
        requestPermission();
    }


    public void requestPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED ||
                ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            // 申请 相机 麦克风权限
            ActivityCompat.requestPermissions(this, new String[]{
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE}, 100);
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public  native  void play(String path_in, String path_out, Surface sur);
    public native void pause();
    public native void continue_();
    public  native  void forword(int sec);
    public native void cut();
    public  native  void back(int sec);


    public native  void test(String src);

    public void onCallTimeInfo(final int currentTime, final int totalTime)
    {
        runOnUiThread(new Runnable() {
            @SuppressLint("SetTextI18n")
            @Override
            public void run() {
                progress.setText(currentTime+"/"+totalTime);
                current_time=currentTime;
            }
        });
//        Log.e("+++++++++++++",currentTime+"");
//
    }


}
