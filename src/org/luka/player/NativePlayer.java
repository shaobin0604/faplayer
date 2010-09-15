package org.luka.player;

import android.view.Surface;

interface NativePlayer {

	public static final int VIDEO_MODE_NONE = 0;
	public static final int VIDEO_MODE_FIT = 1;
	public static final int VIDEO_MODE_FILL = 2;
	
	public abstract int open(String file);

	public abstract void close();

	public abstract int play(int start, int ast, int vst, int sst);

	public abstract void pause();

	public abstract void resume();

	public abstract void reset();
	
	public abstract int setVideoMode(int mode);

	public abstract int getVideoWidth();

	public abstract int getVideoHeight();

	public abstract int getDuration();

	public abstract boolean isPlaying();

	public abstract int attach(Surface surface);

	public abstract void detach();

}
