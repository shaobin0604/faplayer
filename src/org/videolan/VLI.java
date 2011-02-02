package org.videolan;

public interface VLI {
	public final static int EVENT_SURFACE_CREATED = 1001;
	public final static int EVENT_SURFACE_CHANGED = 1002;
	public final static int EVENT_SURFACE_DESTROYED = 1003;

	public final static int EVENT_INPUT_STATE = 0;
	public final static int EVENT_INPUT_DEAD = 1;
	public final static int EVENT_INPUT_POSITION = 4;
	public final static int EVENT_INPUT_LENGTH = 5;

	public final static int EVENT_INPUT_STATE_INIT = 0;
	public final static int EVENT_INPUT_STATE_OPEN = 1;
	public final static int EVENT_INPUT_STATE_PLAY = 2;
	public final static int EVENT_INPUT_STATE_PAUSE = 3;
	public final static int EVENT_INPUT_STATE_END = 4;
	public final static int EVENT_INPUT_STATE_ERROR = 5;

	public void onStateChange(final String module, final String name, final String value);

}
