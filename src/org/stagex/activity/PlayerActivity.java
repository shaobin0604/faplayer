package org.stagex.activity;

import java.util.ArrayList;

import org.stagex.R;
import org.videolan.VLC;
import org.videolan.VLI;
import org.videolan.VLM;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.TextView;

public class PlayerActivity extends Activity implements VLI {

	private SurfaceView mSurfaceView;
	private TextView mTextViewTime;
	private SeekBar mSeekBar;
	private TextView mTextViewLength;
	private ImageButton mImageButtonPrev;
	private ImageButton mImageButtonBackward;
	private ImageButton mImageButtonStop;
	private ImageButton mImageButtonPlay;
	private ImageButton mImageButtonForward;
	private ImageButton mImageButtonNext;

	private SurfaceHolder mSurfaceHolder;

	private ArrayList<String> mPlayList = null;
	private int mCurrentIndex = -1;

	private int mCurrentState = -1;
	private int mCurrentTime = -1;
	private int mCurrentLength = -1;

	private int mDisplayWidth = -1;
	private int mDisplayHeight = -1;
	private int mVideoWidth = -1;
	private int mVideoHeight = -1;

	private Handler mEventHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case VLI.EVENT_SURFACE_CREATED: {
				break;
			}
			case VLI.EVENT_SURFACE_CHANGED: {
				Surface surface = (Surface) msg.obj;
				VLC.attachVideoOutput(surface);
				mDisplayWidth = msg.arg1;
				mDisplayHeight = msg.arg2;
				if (mPlayList != null && mCurrentIndex >= 0
						&& mCurrentIndex < mPlayList.size()) {
					VLM.getInstance().open(mPlayList.get(mCurrentIndex));
				}
				break;
			}
			case VLI.EVENT_SURFACE_DESTROYED: {
				VLC.detachVideoOutput();
				break;
			}
			case VLI.EVENT_INPUT_STATE: {
				int state = msg.arg1;
				switch (state) {
				case VLI.EVENT_INPUT_STATE_PLAY: {
					mImageButtonPlay.setImageResource(R.drawable.pause);
					mImageButtonPlay.invalidate();
					break;
				}
				case VLI.EVENT_INPUT_STATE_PAUSE: {
					mImageButtonPlay.setImageResource(R.drawable.play);
					mImageButtonPlay.invalidate();
					break;
				}
				case VLI.EVENT_INPUT_STATE_END: {
					//mImageButtonPlay.
					mImageButtonPlay.setImageResource(R.drawable.play);
					mImageButtonPlay.invalidate();
					break;
				}
				case VLI.EVENT_INPUT_STATE_ERROR: {
					break;
				}
				default:
					break;
				}
				mCurrentState = msg.arg1;
				break;
			}
			case VLI.EVENT_INPUT_DEAD: {
				break;
			}
			case VLI.EVENT_INPUT_POSITION: {
				int val = msg.arg1;
				if (val != mCurrentTime) {
					int hour = val / 3600;
					val -= hour * 3600;
					int minute = val / 60;
					val -= minute * 60;
					int second = val;
					String time = String.format("%02d:%02d:%02d", hour, minute,
							second);
					mTextViewTime.setText(time);
					mCurrentTime = val;
					if (mCurrentLength > 0) {
						mSeekBar.setProgress(mCurrentTime);
					}
				}
				break;
			}
			case VLI.EVENT_INPUT_LENGTH: {
				int val = msg.arg1;
				int hour = val / 3600;
				val -= hour * 3600;
				int minute = val / 60;
				val -= minute * 60;
				int second = val;
				String time = String.format("%02d:%02d:%02d", hour, minute,
						second);
				mTextViewLength.setText(time);
				mCurrentLength = msg.arg1;
				if (mCurrentLength > 0) {
					mSeekBar.setMax(mCurrentLength);
				}
				break;
			}
			case VLI.EVENT_INPUT_VOUT: {
				mVideoWidth = msg.arg1;
				mVideoHeight = msg.arg2;
				break;
			}
			default:
				break;
			}
		}
	};

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.player);

		mSurfaceView = (SurfaceView) findViewById(R.id.video);
		mSurfaceHolder = mSurfaceView.getHolder();
		mSurfaceHolder.addCallback(new SurfaceHolder.Callback() {

			@Override
			public void surfaceDestroyed(SurfaceHolder holder) {
				mEventHandler.sendEmptyMessage(VLI.EVENT_SURFACE_DESTROYED);
			}

			@Override
			public void surfaceCreated(SurfaceHolder holder) {
				Message msg = new Message();
				msg.what = VLI.EVENT_SURFACE_CREATED;
				mEventHandler.sendMessage(msg);
			}

			@Override
			public void surfaceChanged(SurfaceHolder holder, int format,
					int width, int height) {
				Message msg = new Message();
				msg.what = VLI.EVENT_SURFACE_CHANGED;
				msg.arg1 = width;
				msg.arg2 = height;
				msg.obj = holder.getSurface();
				mEventHandler.sendMessage(msg);
			}
		});
		mTextViewTime = (TextView) findViewById(R.id.time);
		mSeekBar = (SeekBar) findViewById(R.id.seekbar);
		mTextViewLength = (TextView) findViewById(R.id.length);
		mImageButtonPrev = (ImageButton) findViewById(R.id.prev);
		mImageButtonBackward = (ImageButton) findViewById(R.id.backward);
		mImageButtonStop = (ImageButton) findViewById(R.id.stop);
		mImageButtonPlay = (ImageButton) findViewById(R.id.play);
		mImageButtonForward = (ImageButton) findViewById(R.id.forward);
		mImageButtonNext = (ImageButton) findViewById(R.id.next);

		VLM.getInstance().setCallbackHandler(this);

		Bundle bundle = getIntent().getExtras().getBundle("playlist");
		mPlayList = bundle.getStringArrayList("list");
		mCurrentIndex = bundle.getInt("index");

	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		
		VLM.getInstance().close();
		
	}

	@Override
	public void onStart() {
		super.onStart();
		
		VLM.getInstance().play();
	}

	@Override
	public void onStop() {
		super.onStop();

		VLM.getInstance().pause();

	}

	public void onStateChange(final String module, final String name,
			final String value) {
		if (module.compareTo("input") == 0) {
			if (name.compareTo("time") == 0) {
				Message msg = new Message();
				msg.what = VLI.EVENT_INPUT_POSITION;
				msg.arg1 = Integer.parseInt(value);
				mEventHandler.sendMessage(msg);
			} else if (name.compareTo("state") == 0) {
				Message msg = new Message();
				msg.what = VLI.EVENT_INPUT_STATE;
				msg.arg1 = Integer.parseInt(value);
				mEventHandler.sendMessage(msg);
			} else if (name.compareTo("length") == 0) {
				Message msg = new Message();
				msg.what = VLI.EVENT_INPUT_LENGTH;
				msg.arg1 = Integer.parseInt(value);
				mEventHandler.sendMessage(msg);
			}
		} else if (module.compareTo("video") == 0) {
			if (name.compareTo("size") == 0) {
				String[] temp = value.split("x");
				if (temp.length == 2) {
					int width = Integer.parseInt(temp[0]);
					int height = Integer.parseInt(temp[1]);
					Message msg = new Message();
					msg.what = VLI.EVENT_INPUT_VOUT;
					msg.arg1 = width;
					msg.arg2 = height;
					mEventHandler.sendMessage(msg);
				}
			}
		}
	}
}
