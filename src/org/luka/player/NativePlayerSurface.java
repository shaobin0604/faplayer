package org.luka.player;

import android.content.Context;
import android.os.Build;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class NativePlayerSurface extends SurfaceView implements
		SurfaceHolder.Callback {

	private NativePlayer mPlayer;
	private SurfaceHolder mSurfaceHolder;

	public NativePlayerSurface(Context context) {
		super(context);
		int version = Build.VERSION.SDK_INT;
		if (version >= 4 && version < 8) {
			mPlayer = new NativePlayer4();
		} else if (version == 8) {
			mPlayer = new NativePlayer8();
		}
		mSurfaceHolder = getHolder();
		mSurfaceHolder.addCallback(this);
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

	public int open(String file) {
		return mPlayer.open(file);
	}

	public void close() {
		mPlayer.close();
	}

	public int play(int start, int ast, int vst, int sst) {
		return mPlayer.play(start, ast, vst, sst);
	}

	public int setVideoMode(int mode) {
		return mPlayer.setVideoMode(mode);
	}
}
