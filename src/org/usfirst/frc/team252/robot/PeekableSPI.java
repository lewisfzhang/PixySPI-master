package org.usfirst.frc.team252.robot;

import java.util.*;

import edu.wpi.first.wpilibj.SPI;

public class PeekableSPI {
	
	public PeekableSPI(SPI spi) {
		this.spi = spi;
		getNextWord();
	}
	
	public ArrayList<Integer> byteStream = new ArrayList<>();
	private byte getByte(byte data)
	{
		byte[] send = new byte[] {data};
		byte[] recv = new byte[send.length];
		spi.transaction(send, recv, recv.length);
		byteStream.add(Byte.toUnsignedInt(recv[0]));
		return recv[0];
	}
	
	private int getWord()
	{
		return makeWord(getByte((byte)0), getByte((byte)0));
	}
	
	public int readByte() {
		int b = peekByte();
		getNextByte();
		return b;
	}
	
	public int peekByte() {
		return nextWord >>> 8;
	}
	
	private void getNextByte() {
		int b = Byte.toUnsignedInt(getByte((byte)0));
		nextWord = ((nextWord&0xFF) << 8) | b;
	}
	
	public int readWord() {
		int word = peekWord();
		getNextWord();
		return word;
	}
	
	public int peekWord() {
		return nextWord;
	}
	
	private void getNextWord() {
		nextWord = getWord();
		++wordsRead;
//		System.out.println("Word: "+ Integer.toString(nextWord, 16));
	}
	
	private static int makeWord(byte msb, byte lsb) {
		return Byte.toUnsignedInt(lsb) | Byte.toUnsignedInt(msb) << 8;
	}
	
	public long getWordsRead() { return wordsRead; }
	
	private SPI spi;
	private int nextWord;
	private long wordsRead = 0;
	
	
	
	
	public static String hexByte(int b) {
		String hex = Integer.toHexString(b);
		if (hex.length() < 2) hex = "0"+hex;
		return hex;
	}
	public static void visualizeBytes(List<Integer> byteStream) {
		System.out.println(byteStream.size()+" bytes:");
		int numZeros = 0;
		for (int i : byteStream) {
			if (i == 0) {
				numZeros++;
			} else {
				if (numZeros > 5) System.out.println("\n---- "+numZeros+" ZEROS ----");
				else for (; numZeros > 0; numZeros--) System.out.print("00 ");
				numZeros = 0;
				System.out.print(hexByte(i)+" ");
			}
		}
		if (numZeros > 5) System.out.println("\n---- "+numZeros+" ZEROS ----");
		else for (; numZeros > 0; numZeros--) System.out.print("00 ");
		System.out.println();
	}
	
}
