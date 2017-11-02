package net.armtronix.wifiarduino.ssr;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.Socket;
import java.net.URL;
import java.net.URLEncoder;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicHttpResponse;
import org.apache.http.message.BasicNameValuePair;
import org.json.JSONObject;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.speech.RecognizerIntent;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.Toast;
import android.widget.ToggleButton;

public class Client extends Activity {
	 private static final String TAG = "Client";
	private Socket socket;
    public SeekBar seekBar;
	private static int SERVERPORT_MAIN = 0;
	private static String SERVER_MAIN = "";
	private String serverinputdata = null;
	private String serveroutputdata = null;
	private String serverinputdata_parsed = null;
	private String temp_send=null;
	public EditText et;
	private Handler mHandler;
	public static final int VOICE_RECOGNITION_REQUEST_CODE = 1234;
	@Override
	public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);		
		
		
		Intent intent = getIntent();
		SERVERPORT_MAIN = Integer.valueOf(intent.getStringExtra("SERVERPORT"));
	    SERVER_MAIN = intent.getStringExtra("SERVER_IP");
		//new Thread(new ClientThread()).start();
		//mHandler = new Handler();
        //mHandler.post(mUpdate);
        // Creating HTTP client
	    mHandler = new Handler();
        mHandler.post(mUpdate);

        et =(EditText) findViewById(R.id.EditText01);
        
        
        ImageButton speakButton = (ImageButton) findViewById(R.id.button_speak);
        speakButton.setOnClickListener(new OnClickListener()
        {
        	public void onClick(View view) 
        	{
        		//startVoiceRecognitionActivity();
        		temp_send="/gpio?state_sw=1";
        		new HTTPdemo().execute("");
        	}
        	
        });
        
		Button send=(Button) findViewById(R.id.myButton);
        send.setOnClickListener(new OnClickListener()
        {
    	public void onClick(View view) 
    	{
    		try {
    			
    			PrintWriter out = new PrintWriter(new BufferedWriter(
    					new OutputStreamWriter(socket.getOutputStream())),
    					true);
    			//et=(EditText) findViewById(R.id.EditText01);
    			serveroutputdata = et.getText().toString();
    			out.println(serveroutputdata);
    			et.setText("");

    		} catch (UnknownHostException e) {
    			e.printStackTrace();
    		} catch (IOException e) {
    			e.printStackTrace();
    		} catch (Exception e) {
    			e.printStackTrace();
    		}
    	 }
    	});
        
     
		ImageButton statusbutton=(ImageButton) findViewById(R.id.button_status);
        statusbutton.setOnClickListener(new OnClickListener()
        {
    	public void onClick(View view) 
    	{
    		temp_send="/gpio?state_sw=0";
    		new HTTPdemo().execute("");

    	 }
        });
        
      final ToggleButton onoffbutton=(ToggleButton) findViewById(R.id.onoffbutton);
        onoffbutton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @SuppressLint("NewApi")
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
            {
            	String temp="";
            	if(isChecked)
            	{
            	 temp_send="/gpio?state_sw=1";
            	 new HTTPdemo().execute("");
            	 //onoffbutton.updateMovieFavorite(movieObj.getId().intValue(), 1);
            	 //onoffbutton.setText("ON");
            	}
            	else
            	{
            		temp_send="/gpio?state_sw=0";
            		new HTTPdemo().execute("");
                 //onoffbutton.setText("OFF");
                 //onoffbutton.updateMovieFavorite(movieObj.getId().intValue(), 1);
            	}
            		
            	
    			}
            	}
            );
        
        final ToggleButton sensorbutton=(ToggleButton) findViewById(R.id.sensorbutton);
        sensorbutton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @SuppressLint("NewApi")
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
            {
            	String temp="";
            	if(isChecked)
            	{
            		temp_send="/gpio?state_led=1";
            		new HTTPdemo().execute("");

            	}
            	else
            	{
            		temp_send="/gpio?state_led=0";
            		new HTTPdemo().execute("");

            	}
            		
            	
    			}
            	}
            );
        
        seekBar = (SeekBar) findViewById(R.id.seekBar1);
        seekBar.setOnSeekBarChangeListener(

                new OnSeekBarChangeListener() {
                    int progress = 0;
                    @Override
               public void onProgressChanged(SeekBar seekBar, 

               int progresValue, boolean fromUser) {
               progress = progresValue;
               //String temp="br "+(String.valueOf(progress))+"\n\r";
               temp_send="/gpio?state_dimmer="+(String.valueOf(progress*10));
       		   new HTTPdemo().execute("");
               
			
			
   			
               }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {
                    	
                    }
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {
                      // Display the value in textview
                     
                    }}

                );
	}
	
	public void informationMenu() 
	{
	    startActivity(new Intent("android.intent.action.INFOSCREEN"));
	}


	private String SendDataFromAndroidDevice() {
	    String result = "";
        String u="http://"+SERVER_MAIN+":"+SERVERPORT_MAIN+temp_send;
	    try {
	        HttpClient httpclient = new DefaultHttpClient();
	        HttpGet getMethod = new HttpGet(u);
	        //HttpGet getMethod2 = new HttpGet("http://"+SERVER_MAIN+"/xml");
	        BufferedReader in = null;
	        BasicHttpResponse httpResponse = (BasicHttpResponse) httpclient
	                .execute(getMethod);
	        //httpResponse = (BasicHttpResponse) httpclient.execute(getMethod2);
	        
	        in = new BufferedReader(new InputStreamReader(httpResponse
	                .getEntity().getContent()));

	        StringBuffer sb = new StringBuffer("");
	        String line = "";
	        while ((line = in.readLine()) != null) {
	            sb.append(line);
	        }
	        in.close();
	        //serverinputdata=sb.toString(); 
	        //serverinputdata_parsed = serverinputdata.substring(30, 35);
	        result = sb.toString(); 
	        


	    } catch (Exception e) {
	        e.printStackTrace();
	    }
	    return result;
	}
	
	private String Get_data_from_Device() {
	    String result = "";
        String u="http://"+SERVER_MAIN+"/xml";
	    try {
	        HttpClient httpclient = new DefaultHttpClient();
	        HttpGet getMethod = new HttpGet(u);
	        //HttpGet getMethod2 = new HttpGet("http://"+SERVER_MAIN+"/xml");
	        BufferedReader in = null;
	        BasicHttpResponse httpResponse = (BasicHttpResponse) httpclient
	                .execute(getMethod);
	        
	        in = new BufferedReader(new InputStreamReader(httpResponse
	                .getEntity().getContent()));

	        StringBuffer sb = new StringBuffer("");
	        String line = "";
	        while ((line = in.readLine()) != null) {
	            sb.append(line);
	        }
	        in.close();
	        serverinputdata=sb.toString(); 
	        String temp_string_data = serverinputdata.substring(31, 33);
	        if(temp_string_data.equals("ON"))
	        {
	        	serverinputdata_parsed="Light is On";	
	        }
	        else if(temp_string_data.equals("OF")) 
	        {
	        	serverinputdata_parsed="Light is Off";
	        }
	        		
	        result = sb.toString(); 
	        


	    } catch (Exception e) {
	        e.printStackTrace();
	    }
	    return result;
	}	
	
	
	
	
private class HTTPdemo extends
    AsyncTask<String, Void, String> {

@Override
protected void onPreExecute() {}

@Override
protected String doInBackground(String... params) {
	
    String result = SendDataFromAndroidDevice();
    String result1 =Get_data_from_Device();
    //et.setText(sb.toString());///here i need to work
    return result;
}

@Override
protected void onProgressUpdate(Void... values) {}

@Override
protected void onPostExecute(String result) {

    if (result != null && !result.equals("")) {
        try {
            JSONObject resObject = new JSONObject(result);

            } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
}

private Runnable mUpdate = new Runnable() {
	   public void run() {
		   EditText dataget = (EditText) findViewById(R.id.Textget);
		   String result1 =Get_data_from_Device();
		   
			dataget.setText(serverinputdata_parsed);
			//dataget.setText("Light is Off");   
		   
		   //serveroutputdata_parsed = serverinputdata.substring(60, 90);
		   //dataget.setText(serverinputdata_parsed);
		   mHandler.postDelayed(this, 100);
	    }
};


	

}