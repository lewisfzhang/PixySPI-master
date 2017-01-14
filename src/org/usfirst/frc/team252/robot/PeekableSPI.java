package org.usfirst.frc.team252.robot;

import edu.wpi.first.wpilibj.SPI;

public class PeekableSPI {
	
	public PeekableSPI(SPI spi) {
		this.spi = spi;
		getNextWord();
	}
	
	public void writeWord(int word) {
		byte[] buf = new byte[] {(byte)(word), (byte)(word>>8)};
		spi.write(buf, buf.length);
	}
	
	public void writeByte(int b) {
		//byte[] buf = new byte[] {(byte)b};
		//spi.transaction(buf, new byte[buf.length], buf.length);
		spi.write(new byte[] { (byte)b }, 1);		
	}
	
	public int readWord() {
		int word = nextWord;
		getNextWord();
		return word;
	}
	
	public int peekWord() {
		return nextWord;
	}
	
	private void getNextWord() {
		//spi.transaction(new byte[buf.length], buf, buf.length);
		byte[] buf = readSyncedBytes(2);
		// this is big endian (MSB, LSB)
		nextWord = makeWord(buf[1], buf[0]);
	}
	
	private static int makeWord(byte lsb, byte msb) {
		return Byte.toUnsignedInt(lsb) | Byte.toUnsignedInt(msb) << 8;
	}
	
	private byte[] readSyncedBytes(int len)
	{
		byte[] r = new byte[len];
		for (int i=0;i<len;i++)
		{
			r[i] = readSyncedByte();
		}
		return r;
	}
	
	private byte readSyncedByte()
	{
		// send sync byte
		spi.write(new byte[] {PIXY_SYNC_BYTE }, 1);
		// read data
		byte[] buf = new byte[1];
		spi.read(false, buf, buf.length);
		return buf[0];
	}
	
	private SPI spi;
	private int nextWord;
	
	private final int PIXY_SYNC_BYTE = 0x5a;
	private final int PIXY_SYNC_BYTE_DATA = 0x5b;
	
}
