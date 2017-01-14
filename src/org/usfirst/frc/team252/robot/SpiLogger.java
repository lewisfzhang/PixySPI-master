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
	}
	
	public void startLogging()
	{
		for (int i=0;i<1000;i++)
		{
			byte[] d = new byte[1];
			spi.read(false, d, 1);
			System.out.println("Read byte " + d[0]);
		}
	}
}
