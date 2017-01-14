package org.usfirst.frc.team252.robot;

import edu.wpi.first.wpilibj.SPI;

public class SpiLogger {
	private SPI spi;
	
	public SpiLogger()
	{
		spi = new SPI(SPI.Port.kOnboardCS0);
//		spi = new SPI(SPI.Port.kMXP);
		spi.setMSBFirst();
		spi.setClockActiveHigh();
		spi.setSampleDataOnRising();
		spi.setChipSelectActiveLow();
		spi.setClockRate(500000);
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
		int msb = Byte.toUnsignedInt(getByte((byte)0));
		int lsb = Byte.toUnsignedInt(getByte((byte)0));
		msb <<= 8;
		data =  msb | lsb;
		return data;
	}
	
	public void startLogging() throws Exception
	{
		
//		for (int i=0;i<10;i++)
//		{
//			byte[] send = new byte[] {(byte) (i==0 ? 0x5a:0x5b)};
//			byte[] d = new byte[send.length];
//			spi.transaction(send, d, d.length);
//			System.out.println("Rx byte ("+i+"): " + Integer.toString(Byte.toUnsignedInt(d[0]), 16));
//		}
//		byte[] send = new byte[] {0x5a,0x5a,0x5a,0x5a,0x5a,0x5a};
//		byte[] send = new byte[] {0x5a,0x00};
//		byte[] d = new byte[send.length];
//		spi.transaction(send, d, d.length);
		while (true) {
//			byte[] send = new byte[] {0x5a,0x00};
//			byte[] d = new byte[send.length];
//			spi.transaction(send, d, d.length);
			int data = getWord();
			System.out.println(Integer.toString(data, 16));
			Thread.sleep(10);
		}
//		spi.write(send, send.length);
//		spi.read(false, d, d.length);
		
//		for (byte b: d)
//		{
//			System.out.println("rx byte: " + Integer.toString(Byte.toUnsignedInt(b), 16));
//		}
//		System.out.println("Rx byte: " + Integer.toString(Byte.toUnsignedInt(d[0]), 16));
//		System.out.println("Rx byte: " + Integer.toString(Byte.toUnsignedInt(d[1]), 16));
	}
}
