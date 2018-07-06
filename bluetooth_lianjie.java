package com.example.administrator.new_dianzixitongzonghesheji;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;

import android.provider.Settings;
import android.support.v7.app.ActionBarActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.administrator.new_dianzixitongzonghesheji.BlueToothDeviceAdapter;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.PublicKey;
import java.util.Set;
import java.util.UUID;

import com.example.administrator.new_dianzixitongzonghesheji.R;


/**
 * Created by 56305 on 2018/6/28.
 */
public class bluetooth_lianjie extends ActionBarActivity implements View.OnClickListener   {

    public  MainActivity a;
    private BluetoothAdapter bTAdatper;
    private ListView listView;
    private BlueToothDeviceAdapter adapter;
    String androidId = null;
    static String androidId_data;
    Toast tst;
    Context context;
    TextView text_state;
    private TextView text_msg;
    boolean kaimenorguanmen=true;
    boolean switch_bluetooth=true;
    private final int BUFFER_SIZE = 1024;
    byte[] buffer = new byte[BUFFER_SIZE];
    int bytes;


    private static final String NAME = "BT_DEMO";
    private static final UUID BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    boolean lianjie_state=false;
    ConnectThread connectThread;
    private ListenerThread listenerThread;
    Button btn_kaimen,btn_openBT;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.bluetooth_lianjie);
        initView();
        bTAdatper = BluetoothAdapter.getDefaultAdapter();
        initReceiver();
        listenerThread = new ListenerThread();
        listenerThread.start();
        btn_kaimen=(Button)findViewById(R.id.btn_kaimen);
        btn_kaimen.setOnClickListener(this);
        btn_openBT=(Button)findViewById(R.id.btn_openBT);
        btn_openBT.setOnClickListener(this);
        androidId_data="7659483c763e8649";
        if (!bTAdatper.isEnabled())
        {btn_openBT.setText("打开蓝牙");
            switch_bluetooth=true;
        }
        else
        {btn_openBT.setText("关闭蓝牙");
            switch_bluetooth=false;
        }
    }

    private void initView() {
       // findViewById(R.id.btn_openBT).setOnClickListener(this);
        findViewById(R.id.btn_search).setOnClickListener(this);
        findViewById(R.id.btn_send).setOnClickListener(this);
        findViewById(R.id.btn_dengji).setOnClickListener(this);
       // findViewById(R.id.btn_kaimen).setOnClickListener(this);
        findViewById(R.id.btn_tianjia).setOnClickListener(this);
        findViewById(R.id.btn_shanchu).setOnClickListener(this);
        text_state = (TextView) findViewById(R.id.text_state);
        text_msg = (TextView) findViewById(R.id.text_msg);
        androidId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
        listView = (ListView) findViewById(R.id.listView);
        adapter = new BlueToothDeviceAdapter(getApplicationContext(), R.layout.device_list);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                if (bTAdatper.isDiscovering()) {
                    bTAdatper.cancelDiscovery();
                }
                BluetoothDevice device = (BluetoothDevice) adapter.getItem(position);
                //连接设备
                connectDevice(device);
            }
        });
    }

    private void initReceiver() {
        //注册广播
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        registerReceiver(mReceiver, filter);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_openBT:
               if(switch_bluetooth)
               { openBlueTooth();
                   btn_openBT.setText("关闭蓝牙");
                   switch_bluetooth=false;
               }
                else
               {
                   bTAdatper.disable();
                   btn_openBT.setText("打开蓝牙");
                   Toast.makeText(getApplicationContext(), "蓝牙已关闭", Toast.LENGTH_LONG);
                   switch_bluetooth=true;
               }
                break;
            case R.id.btn_search:
                searchDevices();
                break;
            case R.id.btn_send:
                if (connectThread != null) {
                    connectThread.sendMsg("111");

                }
                break;
            case R.id.fanhui:
            {  Intent intent =new Intent(bluetooth_lianjie.this,com.example.administrator.new_dianzixitongzonghesheji.MainActivity.class);
                startActivity(intent);
                //  finish();
            }
            break;
            case R.id.btn_dengji: {
                if( lianjie_state) {

                    tst = Toast.makeText(bluetooth_lianjie.this, androidId, Toast.LENGTH_SHORT);
                    tst.show();
                    //  CopyToClipboard(context,androidId);
                    ClipboardManager cm = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                    // 创建普通字符型ClipData
                    ClipData mClipData = ClipData.newPlainText("本机Android_ID是", androidId);
                    cm.setPrimaryClip(mClipData);
                    connectThread.sendMsg(androidId);

                }
                else
                    Toast.makeText(bluetooth_lianjie.this, "请先连接蓝牙！", Toast.LENGTH_SHORT).show();
            }
            break;
            case R.id.btn_kaimen:
            {
                if( lianjie_state) {
                    if (kaimenorguanmen)

                    {
                        if (androidId.equals("7659483c763e8649")) {   //root用户id
                            connectThread.sendMsg("R" + androidId + "1");
                        } else {
                            connectThread.sendMsg("C" + androidId + "1");
                        }
                        kaimenorguanmen = false;
                        btn_kaimen.setText("关门");
                    } else {
                        if (androidId.equals("7659483c763e8649")) {
                            connectThread.sendMsg("R" + androidId + "0");
                        } else {
                            connectThread.sendMsg("C" + androidId + "0");
                        }

                        kaimenorguanmen = true;
                        btn_kaimen.setText("开门");

                    }
                }
                else
                    Toast.makeText(bluetooth_lianjie.this, "请先连接蓝牙！", Toast.LENGTH_SHORT).show();
            }
            break;
            case R.id.btn_tianjia: {

                if(!androidId.equals("7659483c763e8649"))
                {
                    Toast.makeText(bluetooth_lianjie.this, "抱歉！您不是root用户，无法使用此功能", Toast.LENGTH_SHORT).show();
                }
                else if(text_msg.getText().toString()!=null)
                {    if((returnResultMultiple(text_msg.getText().toString()).length()==16)||(returnResultMultiple(text_msg.getText().toString()).length()==15))
                {
                    connectThread.sendMsg( androidId + "T"+returnResultMultiple(text_msg.getText().toString()));
                    Toast.makeText(bluetooth_lianjie.this, "添加权限成功", Toast.LENGTH_SHORT).show();
                }
                else//添加
                {
                    Toast.makeText(bluetooth_lianjie.this, "请录入正确的要添加的身份码", Toast.LENGTH_SHORT).show();

                }
                }
                else  {
                    Toast.makeText(bluetooth_lianjie.this, "请录入要添加的身份码", Toast.LENGTH_SHORT).show();

                }
                //   finish();


            }
        break;
        case R.id.btn_shanchu: {
                if(!androidId.equals("7659483c763e8649"))
                    {
                        Toast.makeText(bluetooth_lianjie.this, "抱歉！您不是root用户，无法使用此功能", Toast.LENGTH_SHORT).show();
                    }
                    else  if(text_msg.getText().toString()!=null)
                    {
                        if((returnResultMultiple(text_msg.getText().toString()).length()==16)||(returnResultMultiple(text_msg.getText().toString()).length()==15))
                        {
                            connectThread.sendMsg( androidId + "S"+returnResultMultiple(text_msg.getText().toString()));
                            Toast.makeText(bluetooth_lianjie.this, "删除权限成功", Toast.LENGTH_SHORT).show();
                        }
                        else
                        {
                            Toast.makeText(bluetooth_lianjie.this, "请录入正确的要删除的身份码", Toast.LENGTH_SHORT).show();

                        }
                        //删除
                    }
                    //   finish();
                    else  {
                        Toast.makeText(bluetooth_lianjie.this, "请录入要添加的身份码", Toast.LENGTH_SHORT).show();

                    }
            }
            break;
        }
    }


    /**
     * 开启蓝牙
     */
    private void openBlueTooth() {
        if (bTAdatper == null) {
            Toast.makeText(this, "当前设备不支持蓝牙功能", Toast.LENGTH_SHORT).show();
        }
        if (!bTAdatper.isEnabled()) {
           /* Intent i = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivity(i);*/
            bTAdatper.enable();
        }
        else
            Toast.makeText(bluetooth_lianjie.this, "蓝牙已打开", Toast.LENGTH_SHORT).show();
        //开启被其它蓝牙设备发现的功能
        if (bTAdatper.getScanMode() != BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            Intent i = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            //设置为一直开启
            i.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 0);
            startActivity(i);
        }
    }

    /**
     * 搜索蓝牙设备
     */
    private void searchDevices() {
        if (!bTAdatper.isEnabled()) {
           /* Intent i = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivity(i);*/
            Toast.makeText(bluetooth_lianjie.this, "蓝牙未打开，请打开蓝牙", Toast.LENGTH_SHORT).show();
        }
        if (bTAdatper.isDiscovering()) {
            bTAdatper.cancelDiscovery();
        }
        getBoundedDevices();
        bTAdatper.startDiscovery();
    }

    /**
     * 获取已经配对过的设备
     */
    private void getBoundedDevices() {
        //获取已经配对过的设备
        Set<BluetoothDevice> pairedDevices = bTAdatper.getBondedDevices();
        //将其添加到设备列表中
        if (pairedDevices.size() > 0) {
            adapter.clear();
            for (BluetoothDevice device : pairedDevices) {
                adapter.add(device);
            }
        }
    }

    /**
     * 连接蓝牙设备
     */
    private void connectDevice(BluetoothDevice device) {

        text_state.setText(getResources().getString(R.string.connecting));

        try {
            //创建Socket
            BluetoothSocket socket = device.createRfcommSocketToServiceRecord(BT_UUID);
            //启动连接线程
            connectThread = new ConnectThread(socket, true);
            connectThread.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        //取消搜索
        if (bTAdatper != null && bTAdatper.isDiscovering()) {
            bTAdatper.cancelDiscovery();
        }
        //注销BroadcastReceiver，防止资源泄露
        unregisterReceiver(mReceiver);
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                //避免重复添加已经绑定过的设备
                if (device.getBondState() != BluetoothDevice.BOND_BONDED) {
                    adapter.add(device);
                    adapter.notifyDataSetChanged();
                }
            } else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                Toast.makeText(bluetooth_lianjie.this, "开始搜索", Toast.LENGTH_SHORT).show();
            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                Toast.makeText(bluetooth_lianjie.this, "搜索完毕", Toast.LENGTH_SHORT).show();
            }
        }
    };

    /**
     * 连接线程
     */
    public class ConnectThread extends Thread {

        private BluetoothSocket socket;
        private boolean activeConnect;
        InputStream inputStream;
        OutputStream outputStream;

        public ConnectThread(BluetoothSocket socket, boolean connect) {
            this.socket = socket;
            this.activeConnect = connect;
        }

        @Override
        public void run() {
            try {
                //如果是自动连接 则调用连接方法
                if (activeConnect) {
                    socket.connect();
                }
                text_state.post(new Runnable() {
                    @Override
                    public void run() {
                        text_state.setText(getResources().getString(R.string.connect_success));
                        lianjie_state=true;
                    }
                });
                inputStream = socket.getInputStream();
                outputStream = socket.getOutputStream();

                //byte[] buffer = new byte[BUFFER_SIZE];
                //int bytes;
                while (true) {
                    //读取数据
                    bytes = inputStream.read(buffer);
                    if (bytes > 0) {
                        final byte[] data = new byte[bytes];
                        System.arraycopy(buffer, 0, data, 0, bytes);

                        text_msg.post(new Runnable() {
                            @Override
                            public void run() {

                                text_msg.setText(getResources().getString(R.string.get_msg)+new String(data));
                                androidId_data=new String(data);   //转换
                            }
                        });
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
                text_state.post(new Runnable() {
                    @Override
                    public void run() {
                        text_state.setText(getResources().getString(R.string.connect_error));
                        lianjie_state=false;
                    }
                });
            }
        }

        /**
         * 发送数据
         *
         * @param msg
         */
        public void sendMsg(final String msg) {

            byte[] bytes = msg.getBytes();
            if (outputStream != null) {
                try {
                    //发送数据
                    outputStream.write(bytes);
                    text_msg.post(new Runnable() {
                        @Override
                        public void run() {
                            text_msg.setText(getResources().getString(R.string.send_msgs)+msg);
                        }
                    });
                } catch (IOException e) {
                    e.printStackTrace();
                    text_msg.post(new Runnable() {
                        @Override
                        public void run() {
                            text_msg.setText(getResources().getString(R.string.send_msg_error)+msg);
                        }
                    });
                }
            }
        }
    }

    /**
     * 监听线程
     */
    private class ListenerThread extends Thread {

        private BluetoothServerSocket serverSocket;
        private BluetoothSocket socket;

        @Override
        public void run() {
            try {
                serverSocket = bTAdatper.listenUsingRfcommWithServiceRecord(
                        NAME, BT_UUID);
                while (true) {
                    //线程阻塞，等待别的设备连接
                    socket = serverSocket.accept();
                    text_state.post(new Runnable() {
                        @Override
                        public void run() {
                            text_state.setText(getResources().getString(R.string.connecting));
                        }
                    });
                    connectThread = new ConnectThread(socket, false);
                    connectThread.start();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    private static String returnResultMultiple(String str) {
        String string = "";
        if (str.equals("")) {
            return "";
        }
        for (int i = 0; i < str.length(); i++) {
            char ch = str.charAt(i);
            if ((ch>=65&&ch<=122)||(ch>=48&&ch<=57)){
                string = string + ch;
            }
        }
        return string;
    }

}
