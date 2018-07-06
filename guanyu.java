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
 * Created by Administrator on 2018/7/1.
 */
public class guanyu extends ActionBarActivity  {

    private TextView textView3,textView4,textView6;


    Button fanhui2;
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.guanyu);
        //通过Id获得两个TextView控件
        textView3=(TextView)findViewById(R.id.textView3);
        textView4=(TextView)findViewById(R.id.textView4);
        textView6=(TextView)findViewById(R.id.textView6);

        //设置需要显示的字符串
        String html;
        html="<big><a href='https://github.com/ChinaChenYirong'>项目源码</a></big>";
        //使用Html.fromHtml,把含HTML标签的字符串转换成可显示的文本样式
        CharSequence charSequence= Html.fromHtml(html);
        //通过setText给TextView赋值
        textView3.setText(charSequence);
        //设定一个点击的响应
        textView3.setMovementMethod(LinkMovementMethod.getInstance());

        String text="项目作者:陈艺荣、周泽鑫、何晨晖、吴子莹、陈靖康\n";
        text+="项目URL:https://github.com/ChinaChenYirong\n";
        text+="项目email:eecyryou@mail.scut.edu.cn\n";
        text+="项目电话：+86 15768188251";
        //因为textView4中有autoLink=”all“的属性设定，所以会自动识别对应的连接，点击出发对应的Android程序
        textView4.setText(text);

        //设置需要显示的字符串
        String html2;
        html2="<big><a href='https://github.com/ChinaChenYirong'>智能锁(Smart Lock)</a></big>";
        //使用Html.fromHtml,把含HTML标签的字符串转换成可显示的文本样式
        CharSequence charSequence2= Html.fromHtml(html2);
        //通过setText给TextView赋值
        textView6.setText(charSequence2);
        //设定一个点击的响应
        textView6.setMovementMethod(LinkMovementMethod.getInstance());



    }
}


