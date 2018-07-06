package com.example.administrator.new_dianzixitongzonghesheji;

import android.app.AlertDialog;
import android.bluetooth.BluetoothSocket;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.support.v7.app.ActionBarActivity;
import android.text.Html;
import android.text.method.LinkMovementMethod;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import com.example.administrator.new_dianzixitongzonghesheji.bluetooth_lianjie.ConnectThread;
///////
public class MainActivity extends ActionBarActivity {

    Button btn_lianjie;
    Button btn_bangzhu;
    Button btn_guanyu;
    Button btn_tuichu;
    private TextView textView5;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btn_lianjie = (Button) findViewById(R.id.btn_lianjie);
        btn_bangzhu = (Button) findViewById(R.id.btn_bangzhu);
        btn_guanyu = (Button) findViewById(R.id.btn_guanyu);
        btn_tuichu = (Button) findViewById(R.id.btn_tuichu);
        textView5=(TextView)findViewById(R.id.textView5);

        btn_lianjie.setOnClickListener(button_click);
        btn_bangzhu.setOnClickListener(button_click);
        btn_guanyu.setOnClickListener(button_click);
        btn_tuichu.setOnClickListener(button_click);

        //设置需要显示的字符串
        String html2;
        html2="<big><a href='https://github.com/ChinaChenYirong'>智能锁(Smart Lock)</a></big>";
        //使用Html.fromHtml,把含HTML标签的字符串转换成可显示的文本样式
        CharSequence charSequence2= Html.fromHtml(html2);
        //通过setText给TextView赋值
        textView5.setText(charSequence2);
        //设定一个点击的响应
        textView5.setMovementMethod(LinkMovementMethod.getInstance());

    };

    //迁移
    private View.OnClickListener button_click = new View.OnClickListener() {




        @Override
        public void onClick(View view) {

            switch (view.getId()) {
                case R.id.btn_lianjie: {
                    Intent intent =new Intent(MainActivity.this,com.example.administrator.new_dianzixitongzonghesheji.bluetooth_lianjie.class);
                    startActivity(intent);
                    //   finish();
                }
                break;
                case R.id.btn_bangzhu: {
                    Intent intent =new Intent(MainActivity.this,com.example.administrator.new_dianzixitongzonghesheji.bangzhu.class);
                    startActivity(intent);
                    //   finish();
                }
                break;

                case R.id.btn_guanyu: {
                    Intent intent =new Intent(MainActivity.this,com.example.administrator.new_dianzixitongzonghesheji.guanyu.class);
                    startActivity(intent);
                    //   finish();
                }
                break;

                case R.id.btn_tuichu: {

                                            android.os.Process.killProcess(android.os.Process.myPid());
                                            finish();
                                            System.exit(0);
                }
                break;
            }

        }


    };

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);

            builder.setTitle("智能锁：");
            builder.setIcon(R.drawable.tuichu);
            builder.setMessage("确实要退出程序吗？")
                    .setPositiveButton("确定",
                            new DialogInterface.OnClickListener() {

                                public void onClick(DialogInterface dialog,
                                                    int which) {
                                    android.os.Process.killProcess(android.os.Process.myPid());
                                    finish();
                                    System.exit(0);

                                }
                            }).setNegativeButton("取消", null).show();

        }
        return false;
    }




}

