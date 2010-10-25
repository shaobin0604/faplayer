package org.stagex.player;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

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
		Log.d("faplayer", file);
		if (mPlayer.open(file) != 0) {
			Log.d("faplayer", "Failed to open file!");
			Toast.makeText(this, "Failed to open file!", Toast.LENGTH_SHORT)
					.show();
		}
		mPlayer.setVideoMode(NativePlayer.VIDEO_MODE_FIT);
		if (mPlayer.play(0.0, 0, 0, -1) != 0) {
			Log.d("faplayer", "Failed to play file!");
			Toast.makeText(this, "Failed to play file!", Toast.LENGTH_SHORT)
					.show();
		}
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		mPlayer.close();
	}

	@Override
	public void onStart() {
		super.onStart();
		// Bundle bundle = getIntent().getExtras();
		// String file = bundle.getString("file");
		// Log.d("faplayer", file);
		// if (mPlayer.open(file) != 0) {
		// Log.d("faplayer", "Failed to open file!");
		// Toast.makeText(this, "Failed to open file!", Toast.LENGTH_SHORT)
		// .show();
		// }
		// mPlayer.setVideoMode(NativePlayer.VIDEO_MODE_FIT);
		// if (mPlayer.play(0.0, 0, 0, -1) != 0) {
		// Log.d("faplayer", "Failed to play file!");
		// Toast.makeText(this, "Failed to play file!", Toast.LENGTH_SHORT)
		// .show();
		// }
	}

	@Override
	public void onStop() {
		super.onStop();
		// mPlayer.close();
	}
}
