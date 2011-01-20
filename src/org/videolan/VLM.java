package org.videolan;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.Socket;
import java.util.ArrayList;

import android.util.Log;

public class VLM {

	private static VLM mInstance;

	private Socket mClientSocket;
	private InputStream mClientIn;
	private OutputStream mClientOut;
	private Thread mBoot;

	protected VLM() {
	}

	protected String readBytes() {
		int length = 0;
		byte[] buffer = new byte[4096];
		String str = new String();
		try {
			length = mClientIn.read(buffer);
			str = new String(buffer, 0, length, "utf-8");
		} catch (IOException e) {
		}
		return str;
	}

	protected void writeBytes(String str) {
		if (str == null || str.length() == 0)
			return;
		// Log.d("faplayer", str);
		try {
			byte[] raw = str.getBytes("utf-8");
			mClientOut.write(raw);
			mClientOut.flush();
		} catch (UnsupportedEncodingException e) {
		} catch (IOException e) {
		}
	}

	public static VLM getInstance() {
		if (mInstance == null)
			mInstance = new VLM();
		return mInstance;
	}

	public void create(final String[] args) {
		if (args == null || args.length != 2)
			return;
		if (mClientSocket != null && mClientSocket.isConnected())
			return;
		mBoot = new Thread(new Runnable() {
			@Override
			public void run() {
				for (;;) {
					try {
						mClientSocket = new Socket(args[0],
								Integer.parseInt(args[1]));
						mClientSocket.setKeepAlive(true);
						mClientSocket.setTcpNoDelay(true);
						mClientIn = mClientSocket.getInputStream();
						mClientOut = mClientSocket.getOutputStream();
						writeBytes("help\n");
						Log.d("faplayer", readBytes());
						break;
					} catch (IOException e) {
						try {
							Thread.sleep(100);
						} catch (InterruptedException ex) {
						}
					}
				}
			}
		});
		mBoot.start();
	}

	public void destroy() {
		if (mClientSocket != null) {
			try {
				writeBytes("quit\n");
				try {
					mClientSocket.close();
				} finally {
					mClientSocket = null;
				}
			} catch (IOException e) {
			}
			mClientIn = null;
			mClientOut = null;
		}
	}

	public void play(int index) {
		String line;
		line = String.format("goto %d\n", index);
		writeBytes(line);
		line = "play\n";
		writeBytes(line);
	}
	
	public void stop() {
		String line;
		line = "stop\n";
		writeBytes(line);
	}

	public void setPlayList(ArrayList<String> list) {
		String line;
		writeBytes("clear\n");
		for (String file : list) {
			line = String.format("enqueue %s\n", file);
			writeBytes(line);
		}
		line = "playlist\n";
		Log.d("faplayer", readBytes());
	}
}
