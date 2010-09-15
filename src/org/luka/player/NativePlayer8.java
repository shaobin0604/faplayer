package org.luka.player;

import android.view.Surface;

class NativePlayer8 implements NativePlayer {

	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("player-8");
	}

	public static final int VIDEO_MODE_NONE = 0;
	public static final int VIDEO_MODE_FIT = 1;
	public static final int VIDEO_MODE_FILL = 2;
	
	public native int open(String file);

	public native void close();

	public native int play(int start, int ast, int vst, int sst);

	public native void pause();

	public native void resume();

	public native void reset();
	
	public native int setVideoMode(int mode);

	public native int getVideoWidth();

	public native int getVideoHeight();

	public native int getDuration();

	public native boolean isPlaying();

	public native int attach(Surface surface);

	public native void detach();

}
