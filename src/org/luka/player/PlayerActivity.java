package org.luka.player;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

public class PlayerActivity extends Activity {

	private NativePlayerSurface mPlayer;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		mPlayer = new NativePlayerSurface(this);
		setContentView(mPlayer);
		Bundle bundle = getIntent().getExtras();
		String file = bundle.getString("file");
		mPlayer.open(file);
		mPlayer.setVideoMode(NativePlayer.VIDEO_MODE_FIT);
		mPlayer.play(0, 0);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		mPlayer.close();
	}
}
