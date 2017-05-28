package com.vladtuichev.lazycontrol;

import android.util.Log;
import java.io.IOException;
import java.net.Socket;

/**
 * Created by Vlad Tuichev on 21.05.2017.
 */

public class LazyClient
{
    public static final String LOG_TAG = "LazyControlApp";
    private String mServerName = "169.254.3.77";
    private int mServerPort = 8888;
    private Socket mSocket = null;

    public LazyClient(String ip)
    {
        mServerName = ip;
    }

    public void openConnection() throws Exception
    {
        closeConnection();

        try {
            mSocket = new Socket(mServerName,mServerPort);
            }
        catch (IOException e)
        {
            throw new Exception("Can't create socket: "+e.getMessage());
        }
    }

    public void closeConnection()
    {
        if (mSocket != null && !mSocket.isClosed())
        {
            try
            {
                mSocket.close();
            }
            catch (IOException e)
            {
                Log.e(LOG_TAG, "Can't close socket: " + e.getMessage());
            }
            finally {
                mSocket = null;
            }
        }
        mSocket = null;
    }

    public void sendData(byte[] data) throws Exception
    {
        if (mSocket == null || mSocket.isClosed())
            throw new Exception("Can't send data. Socket is null or closed");

        try
        {
            mSocket.getOutputStream().write(data);
            mSocket.getOutputStream().flush();
        } catch (IOException e)
        {
            throw new Exception("Can't send data: "+e.getMessage());
        }
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        closeConnection();
    }

}
