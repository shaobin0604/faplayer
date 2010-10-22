package org.stagex.player;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class InputActivity extends Activity {

	private EditText mEdit;
	private Button mButton;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.test);
		mEdit = (EditText) findViewById(R.id.uri);
		mEdit.setText("rtsp://gaf:GAF123@81.99.219.168/axis-media/media.amp?videocodec=h264&streamprofile=Mobile");
		mButton = (Button) findViewById(R.id.play);
		mButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if (mEdit.getText().toString().length() > 0) {
					Intent intent = new Intent(InputActivity.this, PlayerActivity.class);
					intent.putExtra("file", mEdit.getText().toString());
					startActivity(intent);
				}
			}

		});
	}

}
