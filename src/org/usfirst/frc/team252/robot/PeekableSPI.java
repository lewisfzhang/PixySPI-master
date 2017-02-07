package org.usfirst.frc.team252.robot;

import edu.wpi.first.wpilibj.SPI;

public class PeekableSPI {
	byte[] data;
	public PeekableSPI(SPI spi) {
		this.spi = spi;
		data = getByte(this.spi);
	}
	
	private byte[] getByte(SPI spi) {
		byte[] dataReceived = new byte[14];
		spi.read(false, dataReceived, dataReceived.length);
		return dataReceived;
	}
	
	public int readWord(byte a, byte b) {
		int val = ((a & 0xff) << 8) | (b & 0xff);
		return val;
	}
	
	private SPI spi;
}
