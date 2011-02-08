package org.stagex;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;

import org.stagex.helper.SystemUtility;
import org.videolan.VLC;
import org.videolan.VLM;

import android.app.Application;
import android.content.res.AssetManager;

public class Danmaku extends Application {

	protected boolean initialize() {
		// prepare, orz
		String root = super.getCacheDir().getAbsolutePath();
		try {
			ArrayList<String> list = new ArrayList<String>();
			AssetManager asset = getAssets();
			InputStream is = asset.open("index.txt");
			InputStreamReader ir = new InputStreamReader(is);
			BufferedReader br = new BufferedReader(ir);
			try {
				while (br.ready()) {
					String line = br.readLine();
					list.add(line);
				}
			} finally {
				br.close();
				ir.close();
				is.close();
			}
			for (String file : list) {
				String path = String.format("%s/%s", root, file);
				String parent = path.substring(0, path.lastIndexOf('/'));
				File test = new File(parent);
				if (!test.isDirectory()) {
					test.mkdirs();
					if (!test.isDirectory())
						return false;
				}
				test = new File(path);
				// if (test.exists())
				// continue;
				byte[] buffer = new byte[32768];
				is = asset.open(file);
				OutputStream os = new FileOutputStream(path);
				try {
					while (true) {
						int rc = is.read(buffer);
						if (rc <= 0)
							break;
						os.write(buffer, 0, rc);
					}
				} finally {
					os.close();
					is.close();
				}
			}
		} catch (IOException e) {
			return false;
		}
		// start VLC
		// attention: SO_MAX is 96
		String libd = String.format("%s/lib", root);
		String datd = String.format("%s/share", root);
		int code = SystemUtility.getSDKVersionCode();
		if (code == 6 || code == 7)
			code = 5;
		String aout = String.format("aout_android-%d", code);
		// on my x10i freex10 2.2.1 it's using legacy libui.so
		File test = new File("/system/lib/libsurfaceflinger_client.so");
		String vout = String
				.format("vout_android-%d", test.exists() ? code : 5);
		VLC.getInstance().create(
				new String[] { "--no-ignore-config", "--no-plugins-cache",
						"--no-drop-late-frames", "--verbose", "2",
						"--plugin-path", libd, "--data-path", datd, "--intf",
						"asrc", "--aout", aout, "--vout", vout });
		// start VLM
		VLM.getInstance().create(new String[] { "127.0.0.1", "21178" });

		return true;
	}

	protected void finalize() {
		// stop VLM
		VLM.getInstance().destroy();
		// stop VLC
		VLC.getInstance().destroy();
	}

	@Override
	public void onCreate() {
		super.onCreate();

		initialize();

	}

	@Override
	public void onTerminate() {
		super.onTerminate();

		finalize();

	}

}
