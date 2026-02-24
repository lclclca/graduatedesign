package ima2c.entity.corexml;

public class Channel {
	private String channelId;
	private String channelName;

	private ConnectPort source;
	private ConnectPort destination;

	public Channel() {
		super();
	}

	public Channel(String channelId, String channelName, ConnectPort source, ConnectPort destination) {
		super();
		this.channelId = channelId;
		this.channelName = channelName;
		this.source = source;
		this.destination = destination;
	}

	public String getChannelId() {
		return channelId;
	}

	public void setChannelId(String channelId) {
		this.channelId = channelId;
	}

	public String getChannelName() {
		return channelName;
	}

	public void setChannelName(String channelName) {
		this.channelName = channelName;
	}

	public ConnectPort getSource() {
		return source;
	}

	public void setSource(ConnectPort source) {
		this.source = source;
	}

	public ConnectPort getDestination() {
		return destination;
	}

	public void setDestination(ConnectPort destination) {
		this.destination = destination;
	}

	@Override
	public String toString() {
		return "Channel [channelId=" + channelId + ", channelName=" + channelName + ", source=" + source
				+ ", destination=" + destination + "]";
	}

}
