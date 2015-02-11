package com.smart.smartsip;

public class SmartSipCallInfo 
{
	private String name;
	private int duration;
	private int state;
	
	
	
	public String getName() {
		return name;
	}



	public void setName(String name) {
		this.name = name;
	}



	public int getDuration() {
		return duration;
	}



	public void setDuration(int duration) {
		this.duration = duration;
	}



	public int getState() {
		return state;
	}



	public void setState(int state) {
		this.state = state;
	}



	public SmartSipCallInfo(String name, int duration, int state)
	{
		this.name = name;
		this.duration = duration;
		this.state = state;
	}

}
