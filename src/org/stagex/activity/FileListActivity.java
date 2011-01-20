package org.stagex.activity;

import org.stagex.adapter.FileListAdapter;
import org.stagex.helper.SystemUtility;
import org.stagex.R;

import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;
import android.widget.ListView;

public class FileListActivity extends Activity {
	/** Called when the activity is first created. */

	private ListView mFileList;
	private FileListAdapter mFileAdapter;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
		setContentView(R.layout.filelist);
		mFileAdapter = new FileListAdapter(this, SystemUtility.getExternalStoragePath());
		mFileList = (ListView) findViewById(R.id.filelist);
		mFileList.setAdapter(mFileAdapter);
		mFileList.setOnItemClickListener(mFileAdapter);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if(mFileAdapter.onKey(null, keyCode, event))
			return true;
		else
			return super.onKeyDown(keyCode, event);
	}
}
