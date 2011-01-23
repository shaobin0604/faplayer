package org.stagex.activity;

import java.util.ArrayList;

import org.stagex.R;
import org.videolan.VLC;
import org.videolan.VLM;

import android.app.Activity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class PlayerActivity extends Activity {

	private SurfaceView mSurfaceView;
	private SurfaceHolder mSurfaceHolder;

	private ArrayList<String> mPlayList;
	private int mCurrentIndex;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.player);

		mSurfaceView = (SurfaceView) findViewById(R.id.video);
		mSurfaceHolder = mSurfaceView.getHolder();
		mSurfaceHolder.addCallback(new SurfaceHolder.Callback() {

			@Override
			public void surfaceDestroyed(SurfaceHolder holder) {
				VLC.detachVideoOutput();
			}

			@Override
			public void surfaceCreated(SurfaceHolder holder) {
				VLC.attachVideoOutput(holder.getSurface());
				VLM.getInstance().playMRL(mPlayList.get(mCurrentIndex));
			}

			@Override
			public void surfaceChanged(SurfaceHolder holder, int format,
					int width, int height) {
				VLC.attachVideoOutput(holder.getSurface());
			}
		});

		Bundle bundle = getIntent().getExtras().getBundle("playlist");
		mPlayList = bundle.getStringArrayList("list");
		mCurrentIndex = bundle.getInt("index");

	}

	@Override
	public void onDestroy() {
		super.onDestroy();
	}

	@Override
	public void onStart() {
		super.onStart();
	}

	@Override
	public void onStop() {
		super.onStop();

		VLM.getInstance().stop();

	}
}
