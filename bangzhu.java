package com.example.administrator.new_dianzixitongzonghesheji;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.text.Html;
import android.text.method.LinkMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

/**
 * Created by 56305 on 2018/6/30.
 */
public class bangzhu extends ActionBarActivity {


    private TextView textView9;
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.bangzhu);

        textView9=(TextView)findViewById(R.id.textView9);

        //设置需要显示的字符串
        String html2;
        html2="<big><a href='https://github.com/ChinaChenYirong'>智能锁(Smart Lock)</a></big>";
        //使用Html.fromHtml,把含HTML标签的字符串转换成可显示的文本样式
        CharSequence charSequence2= Html.fromHtml(html2);
        //通过setText给TextView赋值
        textView9.setText(charSequence2);
        //设定一个点击的响应
        textView9.setMovementMethod(LinkMovementMethod.getInstance());



    }
}
