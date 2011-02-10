package org.videolan;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Iterator;

import android.util.Log;

public class VLM {

	private static VLM mInstance;

	private VLI mCallbackHandler = null;

	private Thread mVLMThread = null;

	private ByteBuffer mSendBuffer = ByteBuffer.allocate(512);
	private ByteBuffer mRecvBuffer = ByteBuffer.allocate(512);
	private Selector mSelector;
	private SocketChannel mClientChannel = null;

	private int mLineOffset = 0;
	private int mLineLength = 0;
	private byte[] mLineBuffer = new byte[512];

	protected VLM() {
	}

	protected void triggerCallback(String line) {
		if (line == null || line.length() == 0 || mCallbackHandler == null)
			return;
		// TODO: quote
		String[] temp = line.split("\\s+");
		if (temp.length != 3)
			return;
		mCallbackHandler.onStateChange(temp[0], temp[1], temp[2]);
	}

	protected boolean wantRead() {
		return true;
	}

	protected boolean wantWrite() {
		synchronized (mSendBuffer) {
			return (mSendBuffer.position() > 0);
		}
	}

	protected void doRead() throws IOException {
		synchronized (mRecvBuffer) {
			mRecvBuffer.clear();
			int read = mClientChannel.read(mRecvBuffer);
			if (read == 0)
				return;
			mRecvBuffer.flip();
			while (mRecvBuffer.position() != mRecvBuffer.limit()) {
				byte ch = mRecvBuffer.get();
				mLineBuffer[(mLineOffset + mLineLength++) % mLineBuffer.length] = ch;
				if ((ch == '\n' || ch == '\r' || ch == '\0')
						|| (mLineLength == mLineBuffer.length)) {
					if ((ch == '\n' || ch == '\r' || ch == '\0'))
						mLineLength -= 1;
					String line = null;
					try {
						if (mLineOffset + mLineLength <= mLineBuffer.length)
							line = new String(mLineBuffer, mLineOffset,
									mLineLength, "utf-8");
						else {
							byte[] temp = new byte[mLineLength];
							int part1 = mLineBuffer.length - mLineOffset;
							int part2 = (mLineOffset + mLineLength)
									% mLineBuffer.length;
							System.arraycopy(mLineBuffer, mLineOffset, temp, 0,
									part1);
							System.arraycopy(mLineBuffer, 0, temp, part1, part2);
							line = new String(temp, 0, mLineLength, "utf-8");
						}
					} catch (UnsupportedEncodingException e) {
					}
					if (line != null) {
						// Log.d("faplayer-java", line);
						triggerCallback(line);
					}
					mLineOffset += mLineLength;
					mLineOffset %= mLineBuffer.length;
					mLineLength = 0;
				}
			}
		}
	}

	protected void doWrite() throws IOException {
		synchronized (mSendBuffer) {
			int position = mSendBuffer.position();
			mSendBuffer.flip();
			int written = 0;
			written = mClientChannel.write(mSendBuffer);
			if (written < position) {
				mSendBuffer.position(position - written);
			}
			mSendBuffer.compact();
		}
	}

	protected void writeBytes(String str) {
		if (str == null || str.length() == 0)
			return;
		if (!str.endsWith("\n"))
			str += "\n";
		byte[] data = null;
		try {
			data = str.getBytes("utf-8");
		} catch (UnsupportedEncodingException e) {
		}
		writeBytes(data);
	}

	protected void writeBytes(byte[] data) {
		if (data != null && data.length > 0) {
			synchronized (mSendBuffer) {
				mSendBuffer.put(data, 0,
						mSendBuffer.remaining() >= data.length ? data.length
								: mSendBuffer.remaining());
			}
		}
		if (mSelector != null)
			mSelector.wakeup();
	}

	public static VLM getInstance() {
		if (mInstance == null)
			mInstance = new VLM();
		return mInstance;
	}

	public void create(final String[] args) {
		if (args == null || args.length != 2 || mVLMThread != null)
			return;
		mVLMThread = new Thread(new Runnable() {
			@Override
			public void run() {
				for (;;) {
					mLineOffset = 0;
					mLineLength = 0;
					mRecvBuffer.clear();
					mSendBuffer.clear();
					try {
						mSelector = Selector.open();
						mClientChannel = SocketChannel
								.open(new InetSocketAddress(args[0], Integer
										.parseInt(args[1])));
						mClientChannel.configureBlocking(false);
						mClientChannel.register(mSelector, SelectionKey.OP_READ
								| SelectionKey.OP_WRITE);
						while (true) {
							mClientChannel
									.register(
											mSelector,
											(wantRead() ? SelectionKey.OP_READ
													: 0)
													| (wantWrite() ? SelectionKey.OP_WRITE
															: 0));
							// TODO: when the connection hangs up unexpectedly,
							// figure it out
							int n = mSelector.select();
							if (n < 0)
								break;
							Iterator<SelectionKey> it = mSelector
									.selectedKeys().iterator();
							while (it.hasNext()) {
								SelectionKey key = (SelectionKey) it.next();
								it.remove();
								if (key.isReadable()) {
									doRead();
								}
								if (key.isWritable()) {
									doWrite();
								}
							}
						}
					} catch (IOException e) {
						try {
							if (mClientChannel != null)
								mClientChannel.close();
							if (mSelector != null)
								mSelector.close();
						} catch (IOException ex) {
						}
						try {
							Thread.sleep(100);
						} catch (InterruptedException ex) {
						}
					}
				}
			}
		});
		mVLMThread.start();
	}

	public void destroy() {
		// 乱暴じゃなイカ？
		if (mVLMThread.isAlive()) {
			mVLMThread.interrupt();
			mVLMThread = null;
		}
	}

	public void setCallbackHandler(VLI handler) {
		mCallbackHandler = handler;
	}

	public void open(String file) {
		String line;
		line = String.format("open \"%s\"", file);
		writeBytes(line);
	}

	public void close() {
		String line;
		line = String.format("close");
		writeBytes(line);
	}

	public void play() {
		String line;
		line = String.format("play");
		writeBytes(line);
	}

	public void pause() {
		String line;
		line = String.format("pause");
		writeBytes(line);
	}

	public void stop() {
		String line;
		line = String.format("stop");
		writeBytes(line);
	}
}
