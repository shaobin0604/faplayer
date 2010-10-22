package org.stagex.player;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Vector;

import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class FileListAdapter extends BaseAdapter implements
		OnItemClickListener, OnKeyListener, FilenameFilter {

	private Context mContext;
	private File mFile;
	private Vector<File> mFileList;
	private String mCurrentPath;

	public static String getExternalStoragePath() {
		boolean exists = Environment.getExternalStorageState().equals(
				android.os.Environment.MEDIA_MOUNTED);
		if (exists)
			return Environment.getExternalStorageDirectory().getAbsolutePath();
		else
			return "/";
	}

	public FileListAdapter(Context context, String path) {
		mContext = context;
		setDirectory(path);
	}

	private void setDirectory(String path) {
		mFile = new File(path);
		if (mFile == null)
			return;
		File[] list = mFile.listFiles(this);
		if ((list == null || list.length == 0)
				&& (!path.equals(getExternalStoragePath()))) {
			return;
		}
		mFileList = new Vector<File>();
		for (File f : list) {
			if (f.isDirectory()) {
				mFileList.add(f);
			}
		}
		for (File f : list) {
			if (f.isFile() && !f.isHidden()) {
				mFileList.add(f);
			}
		}
		mCurrentPath = mFile.getAbsolutePath();
		if (mFileList.size() > 0)
			notifyDataSetChanged();
	}

	@Override
	public int getCount() {
		int count = mFileList.size();
		return count;
	}

	@Override
	public Object getItem(int position) {
		Object obj = mFileList.get(position);
		return obj;
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View view;
		if (convertView == null) {
			LayoutInflater inflater = LayoutInflater.from(mContext);
			view = inflater.inflate(R.layout.fileitem, parent, false);
		} else
			view = convertView;
		File file = mFileList.get(position);
		TextView tvFileName = (TextView) view.findViewById(R.id.filename);
		tvFileName.setText(file.getName());

		return view;
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		File file = mFileList.get(position);
		if (file.isDirectory())
			setDirectory(file.getAbsolutePath());
		if (file.isFile()) {
			Intent intent = new Intent(mContext, PlayerActivity.class);
			intent.putExtra("file", file.getAbsolutePath());
			mContext.startActivity(intent);
		}
	}

	@Override
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			if (!mCurrentPath.equals(getExternalStoragePath())) {
				setDirectory(mFile.getParentFile().getAbsolutePath());
				return true;
			} else
				return false;
		}
		return false;
	}

	@Override
	public boolean accept(File dir, String filename) {
		File file = new File(dir.getAbsolutePath() + "/" + filename);
		if (file.isDirectory())
			return true;
		String name = filename.toLowerCase();
		String exts = "3gp#amv#ape#asf#avi#flac#flv#mkv#mov#mp3#mp4#mpeg#mpg#rm#rmvb#tta#wav#wma#wmv";
		String[] temp = exts.split("#");
		for (String s : temp) {
			if (name.endsWith(s))
				return true;
		}
		return false;
	}

}
