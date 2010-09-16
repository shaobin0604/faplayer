package org.luka.player;

import android.view.Surface;

class NativePlayer8 implements NativePlayer {

	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("player-8");
	}
	
	public native int open(String file);

	public native void close();

	public native int play(double start, int ast);

	public native void pause();

	public native void resume();

	public native int seek(double time);
	
	public native int setVideoMode(int mode);

	public native int getVideoWidth();

	public native int getVideoHeight();

	public native int getDuration();
	
	public native double getCurrentTime();

	public native boolean isPlaying();

	public native int attach(Surface surface);

	public native void detach();

}
