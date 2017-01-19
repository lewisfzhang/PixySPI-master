package org.usfirst.frc.team252.robot;

import java.util.*;

import edu.wpi.first.wpilibj.SPI;

public class PeekableSPI {
	
	public PeekableSPI(SPI spi) {
		this.spi = spi;
		getNextWord();
	}
	
	public byte getByte(byte data)
	{
		byte[] send = new byte[] {data};
		byte[] recv = new byte[send.length];
		spi.transaction(send, recv, recv.length);
		return recv[0];
	}
	
	public int getWord()
	{
		int data = 0;
		return makeWord(getByte((byte)0), getByte((byte)0));
//		int msb = Byte.toUnsignedInt(getByte((byte)0));
//		int lsb = Byte.toUnsignedInt(getByte((byte)0));
//		msb <<= 8;
//		data =  msb | lsb;
//		return data;
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
		if (buffer.isEmpty()) {
			nextWord = getWord();
		} else {
			int bufferedWord = buffer.get(buffer.size()-1);
			nextWord = bufferedWord;
			buffer.remove(buffer.size()-1);
		}
		++wordsRead;
//		System.out.println("Word: "+ Integer.toString(nextWord, 16));
	}
	
	private static int makeWord(byte lsb, byte msb) {
		return Byte.toUnsignedInt(lsb) | Byte.toUnsignedInt(msb) << 8;
	}
	
	public long getWordsRead() { return wordsRead; }
	
	public void pushBuffer(int word) {
		buffer.add(word);
	}
	
	private List<Integer> buffer = new ArrayList<>();
	private SPI spi;
	private int nextWord;
	private long wordsRead = 0;
	
	private final int PIXY_SYNC_BYTE = 0x5a;
	private final int PIXY_SYNC_BYTE_DATA = 0x5b;
	
}
