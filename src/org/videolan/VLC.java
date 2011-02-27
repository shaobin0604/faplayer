package org.videolan;

import org.stagex.helper.SystemUtility;

import android.view.Surface;

public class VLC {

	static {
		System.loadLibrary("vlccore");
		int arch = SystemUtility.getArmArchitecture();
		String ffmpeg = String.format("ffmpeg-%d", arch);
		System.loadLibrary(ffmpeg);
	}

	private static VLC mInstance = null;

	private int mHandle = 0;

	private Thread mVLCMain;

	protected VLC() {
	}
	
	public static native int setenv(String key, String val, boolean overwrite);

	private native void run(String[] args);

	public static native void attachVideoOutput(Surface surface);

	public static native void detachVideoOutput();

	public static VLC getInstance() {
		if (mInstance == null)
			mInstance = new VLC();
		return mInstance;
	}

	public void create(final String[] args) {
		if (mVLCMain != null && mVLCMain.isAlive())
			return;
		mVLCMain = new Thread(new Runnable() {
			@Override
			public void run() {
				VLC.this.run(args);
			}
		});
		mVLCMain.start();
	}

	public void destroy() {
		// ...
	}
}
