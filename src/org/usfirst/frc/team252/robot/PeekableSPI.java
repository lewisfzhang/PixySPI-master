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
		byte[] buf = new byte[] {(byte)b};
		spi.write(buf, buf.length);
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
		byte[] buf = new byte[2];
		spi.read(false, buf, buf.length);
		nextWord = makeWord(buf[0], buf[1]);
	}
	
	private static int makeWord(byte lsb, byte msb) {
		return Byte.toUnsignedInt(lsb) | Byte.toUnsignedInt(msb)<<8;
	}
	
	private SPI spi;
	private int nextWord;
	
}
