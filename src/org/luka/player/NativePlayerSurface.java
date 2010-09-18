package org.luka.player;

import android.content.Context;
import android.os.Build;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class NativePlayerSurface extends SurfaceView implements
		SurfaceHolder.Callback {

	private NativePlayer mPlayer = null;

	private SurfaceHolder mSurfaceHolder;

	public NativePlayerSurface(Context context) {
		super(context);
		mSurfaceHolder = getHolder();
		mSurfaceHolder.addCallback(this);
		if (mPlayer == null) {
			int version = Build.VERSION.SDK_INT;
			if (version >= 4 && version < 8) {
				mPlayer = new NativePlayer4();
			} else if (version == 8) {
				mPlayer = new NativePlayer8();
			}
		}
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		mPlayer.attach(holder.getSurface());
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		mPlayer.detach();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
	}

	// ----------------

    public int open(String file) {
        return mPlayer.open(file);
    }

	public void close() {
		mPlayer.close();
	}

	public int play(double start, int ast) {
		return mPlayer.play(start, ast);
	}

	public void pause() {
		mPlayer.pause();
	}

	public void resume() {
		mPlayer.resume();
	}

	public int seek(double time) {
		return mPlayer.seek(time);
	}

	public double getCurrentTime() {
		return mPlayer.getCurrentTime();
	}

	public int setVideoMode(int mode) {
		return mPlayer.setVideoMode(mode);
	}

	public boolean isPlaying() {
		return mPlayer.isPlaying();
	}
}
