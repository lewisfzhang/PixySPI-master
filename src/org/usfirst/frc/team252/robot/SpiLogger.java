package org.usfirst.frc.team252.robot;

import edu.wpi.first.wpilibj.SPI;

public class SpiLogger {
	private SPI spi;
	
	public SpiLogger()
	{
		spi = new SPI(SPI.Port.kOnboardCS0);
		spi.setMSBFirst();
		spi.setClockActiveHigh();
		spi.setSampleDataOnRising();
		spi.setChipSelectActiveLow();
		spi.setClockRate(500000);
	}
	
	public void startLogging()
	{
		
//		for (int i=0;i<10;i++)
//		{
//			byte[] send = new byte[] {(byte) (i==0 ? 0x5a:0x5b)};
//			byte[] d = new byte[send.length];
//			spi.transaction(send, d, d.length);
//			System.out.println("Rx byte ("+i+"): " + Integer.toString(Byte.toUnsignedInt(d[0]), 16));
//		}
		byte[] send = new byte[] {0x5a,0x00};
		byte[] d = new byte[send.length];
		//spi.transaction(send, d, d.length);
		spi.write(send, send.length);
		spi.read(false, d, d.length);
		
		for (byte b: d)
		{
			System.out.println("Rx byte: " + Integer.toString(Byte.toUnsignedInt(b), 16));
		}
//		System.out.println("Rx byte: " + Integer.toString(Byte.toUnsignedInt(d[0]), 16));
//		System.out.println("Rx byte: " + Integer.toString(Byte.toUnsignedInt(d[1]), 16));
	}
}
