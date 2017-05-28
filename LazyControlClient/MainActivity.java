package com.vladtuichev.lazycontrol;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import static com.vladtuichev.lazycontrol.LazyServer.LOG_TAG;

public class MainActivity extends AppCompatActivity
{
    private Button mButtonOpen  = null;
    private Button mButtonVolumePlus  = null;
    private Button mButtonVolumeMinus  = null;
    private Button mButtonPowerOff  = null;
    private Button mButtonClose = null;
    private LazyServer mServer = null;
    private EditText mEdit = null;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mButtonOpen = (Button) findViewById(R.id.button_open_connection);
        mButtonVolumePlus = (Button) findViewById(R.id.volume_plus);
        mButtonVolumeMinus = (Button) findViewById(R.id.volume_minus);
        mButtonPowerOff = (Button) findViewById(R.id.power_off);
        mButtonClose = (Button) findViewById(R.id.button_close_connection);
        mEdit   = (EditText)findViewById(R.id.IPfield);

        mButtonVolumePlus.setEnabled(false);
        mButtonVolumeMinus.setEnabled(false);
        mButtonPowerOff.setEnabled(false);
        mButtonClose.setEnabled(false);

        mButtonOpen.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                mServer = new LazyServer( mEdit.getText().toString());

                new Thread(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        try
                        {
                           mServer.openConnection();
                           runOnUiThread(new Runnable()
                           {
                               @Override
                               public void run() {
                                   mButtonVolumePlus.setEnabled(true);
                                   mButtonVolumeMinus.setEnabled(true);
                                   mButtonPowerOff.setEnabled(true);
                                   mButtonClose.setEnabled(true);
                                   mEdit.setEnabled(false);
                               }
                           });
                        }
                        catch (Exception e)
                        {
                            Log.e(LOG_TAG, e.getMessage());
                            mServer = null;
                        }
                    }
                }).start();
            }
        });

        mButtonVolumePlus.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v) {
                if (mServer == null)
                    Log.e(LOG_TAG, "Server is null");

                new Thread(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        try
                        {
                            mServer.sendData("volume_plus".getBytes());
                        }
                        catch (Exception e)
                        {
                            Log.e(LOG_TAG, e.getMessage());
                        }
                    }
                }).start();
            }
        });

        mButtonVolumeMinus.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v) {
                if (mServer == null)
                    Log.e(LOG_TAG, "Server is null");

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try
                        {
                            mServer.sendData("volume_minus".getBytes());
                        }
                        catch (Exception e)
                        {
                            Log.e(LOG_TAG, e.getMessage());
                        }
                    }
                }).start();
            }
        });

        mButtonPowerOff.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (mServer == null)
                    Log.e(LOG_TAG, "Server is null");

                new Thread(new Runnable()
                {
                    @Override
                    public void run() {
                        try
                        {
                            mServer.sendData("power_off".getBytes());
                        }
                        catch (Exception e)
                        {
                            Log.e(LOG_TAG, e.getMessage());
                        }
                    }
                }).start();
            }
        });

        mButtonClose.setOnClickListener(new View.OnClickListener()
        {

            @Override
            public void onClick(View v)
            {
                mServer.closeConnection();
                mEdit.setEnabled(true);
                mButtonVolumePlus.setEnabled(false);
                mButtonVolumeMinus.setEnabled(false);
                mButtonPowerOff.setEnabled(false);
                mButtonClose.setEnabled(false);
            }
        });

    }

}
