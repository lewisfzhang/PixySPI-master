package org.usfirst.frc.team252.robot;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.wpilibj.SPI;

public class PixyCamStuff {
	
	public PixyCamStuff() {
		SPI spi = new SPI(SPI.Port.kOnboardCS0);
		spi.setMSBFirst();
		spi.setClockActiveHigh();
		spi.setSampleDataOnRising();
		spi.setChipSelectActiveLow();
		pspi = new PeekableSPI(spi);
	}
	
	public Frame.Block parseBlock() {
		Frame.Block block = new Frame.Block();
		block.sync = pspi.readWord()==0xaa55? Frame.Block.SyncType.NORMAL : Frame.Block.SyncType.COLOR_CODE;
		if (pspi.peekWord()==0xaa55 || pspi.peekWord()==0xaa56) return null;
		block.checksum = pspi.readWord();
		block.signature = pspi.readWord();
		block.centerX = pspi.readWord();
		block.centerY = pspi.readWord();
		block.width = pspi.readWord();
		block.height = pspi.readWord();
		return block;
	}
	
	public Frame getFrame() {
		List<Frame.Block> blocks = new ArrayList<>();
		while (true) { // don't judge, it werks
			Frame.Block b = parseBlock();
			if (b == null) break;
			blocks.add(b);
		}
		
		return new Frame(blocks, frameCount++);
	}
	
	private PeekableSPI pspi;
	private int frameCount = 0;
	
}
