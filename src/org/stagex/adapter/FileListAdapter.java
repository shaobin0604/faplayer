package org.stagex.adapter;

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import org.stagex.activity.PlayerActivity;
import org.stagex.helper.SystemUtility;
import org.stagex.R;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
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
	private String mCurrentPath;
	private File mCurrentRoot;
	private ArrayList<String> mTop = new ArrayList<String>();
	private ArrayList<String> mBottom = new ArrayList<String>();
	private ArrayList<String> mList = new ArrayList<String>();

	public FileListAdapter(Context context, String path) {
		mContext = context;
		setDirectory(path);
	}

	private void setDirectory(String path) {
		mCurrentRoot = new File(path);
		if (mCurrentRoot == null)
			return;
		File[] list = mCurrentRoot.listFiles(this);
		if ((list == null || list.length == 0)
				&& (!path.equals(SystemUtility.getExternalStoragePath()))) {
			return;
		}
		mTop.clear();
		mBottom.clear();
		mList.clear();
		for (File f : list) {
			if (f.isDirectory()) {
				mTop.add(f.getName());
			}
			if (f.isFile()) {
				mBottom.add(f.getName());
			}
		}
		Comparator<String> comparator = new Comparator<String>() {
			@Override
			public int compare(String n1, String n2) {
				return n1.compareToIgnoreCase(n2);
			}
		};
		Collections.sort(mTop, comparator);
		Collections.sort(mBottom, comparator);
		mList.addAll(mTop);
		mList.addAll(mBottom);
		mCurrentPath = mCurrentRoot.getAbsolutePath();
		if (mList.size() > 0)
			notifyDataSetChanged();
	}

	@Override
	public int getCount() {
		int count = mList.size();
		return count;
	}

	@Override
	public Object getItem(int position) {
		Object obj = mList.get(position);
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
		String name = mList.get(position);
		TextView tvFileName = (TextView) view.findViewById(R.id.filename);
		tvFileName.setText(name);

		return view;
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		File file = new File(String.format("%s/%s", mCurrentPath,
				mList.get(position)));
		if (file.isDirectory())
			setDirectory(file.getAbsolutePath());
		if (file.isFile()) {
			Intent intent = new Intent(mContext, PlayerActivity.class);
			ArrayList<String> list = new ArrayList<String>();
			for (String name : mBottom) {
				String path = String.format("%s/%s", mCurrentPath, name);
				list.add(path);
			}
			Bundle bundle = new Bundle();
			bundle.putStringArrayList("list", list);
			bundle.putInt("index", position - mTop.size());
			intent.putExtra("playlist", bundle);
			mContext.startActivity(intent);
		}
	}

	@Override
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			if (!mCurrentPath.equals(SystemUtility.getExternalStoragePath())) {
				setDirectory(mCurrentRoot.getParentFile().getAbsolutePath());
				return true;
			} else
				return false;
		}
		return false;
	}

	@Override
	public boolean accept(File dir, String filename) {
		if (filename.startsWith("."))
			return false;
		File file = new File(String.format("%s/%s", dir.getAbsolutePath(),
				filename));
		if (file.isHidden())
			return false;
		if (file.isDirectory())
			return true;
		String name = filename.toLowerCase();
		int dot = name.lastIndexOf('.');
		if (dot < 0 || dot == name.length() - 1)
			return false;
		String ext = name.substring(dot + 1).toLowerCase();
		String list = "3gp#amv#ape#asf#avi#flac#flv#mkv#mov#mp3#mp4#mpeg#mpg#rm#rmvb#tta#wav#wma#wmv";
		if (list.indexOf(ext) < 0)
			return false;
		return true;
	}
}
