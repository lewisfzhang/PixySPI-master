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
//		spi.setClockRate(50000);
		pspi = new PeekableSPI(spi);
	}
	
	public Frame.Block parseBlock() {
		Frame.Block block = new Frame.Block();
		// Wait for sync
//		while (pspi.peekWord() != 0xaa55) { pspi.readWord(); }
////		System.out.println("First sync: "+ Integer.toString(pspi.peekWord(),16));
//		int nextWd = pspi.readWord(); // eat the last word, get next
////		System.out.println("Second sync: "+Integer.toString(nextWd,16));
//		if (nextWd == 0xaa55) {
//			System.out.println("Double-sync: end");
//			return null;
//		}
		// Wait for sync
		while (pspi.peekWord() != 0xaa55) { pspi.readWord(); }
		// we have a 0xaa55
		pspi.pushBuffer(0xaa55); // push it back
		int nextWord = pspi.readWord();
		System.out.println("First sync: "+ Integer.toString(nextWord,16));
		block.sync = nextWord==0xaa55? Frame.Block.SyncType.NORMAL : Frame.Block.SyncType.COLOR_CODE;
		nextWord = pspi.readWord();
		System.out.println("?Second sync: "+ Integer.toString(nextWord,16));
		if (nextWord == 0xaa55 || nextWord == 0xaa56) {
			// Frame ended.
			//pspi.pushBuffer(0xaa55); // Push on a single sync
			System.out.println("Double-sync: end");
			return null;
		}
		block.checksum = pspi.readWord();
		block.signature = pspi.readWord();
		block.centerX = pspi.readWord();
		block.centerY = pspi.readWord();
		block.width = pspi.readWord();
		block.height = pspi.readWord();
		return block;
	}
	
	public Frame getFrame() {
//		int c=0;
//		while (true) {
//			System.out.println("W"+(++c)+": " + Integer.toString(pspi.readWord(), 16));
//			try {
//				Thread.sleep(500);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		}
//		System.out.println("Waiting for sync checksum");
//		while (pspi.peekWord() != 0xaa55) { pspi.readWord(); }
//		while (true) {
//			System.out.println("Waiting for sync checksum");
//			while (pspi.peekWord() != 0xaa55) { pspi.readWord(); }
//			System.out.println("Sync status: "+ (pspi.peekWord() == 0xaa55) + " Word Seq: " + pspi.getWordsRead());
//			System.out.println("Sync Word: " + Integer.toString(pspi.peekWord(), 16));
//			System.out.println("Checksum Word: " + Integer.toString(pspi.readWord(), 16));
//			System.out.println("Sig Word: " + Integer.toString(pspi.readWord(), 16));
//			System.out.println("Cx Word: " + Integer.toString(pspi.readWord(), 16));
//			System.out.println("Cy Word: " + Integer.toString(pspi.readWord(), 16));
//			System.out.println("W Word: " + Integer.toString(pspi.readWord(), 16));
//			System.out.println("H Word: " + Integer.toString(pspi.readWord(), 16));
//			System.out.println("=========BLOCK END=======");
//			try {
//				Thread.sleep(500);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		}
//		return null;
//		// Wait for sync (0x55aa)
//		System.out.println("Waiting for sync checksum");
//		while (pspi.peekWord() != 0xaa55) {pspi.readWord();}
		List<Frame.Block> blocks = new ArrayList<>();
		while (true) { // don't judge, it werks
			Frame.Block b = parseBlock();
			if (b == null) break;
			blocks.add(b);
			System.out.println("Block added: " + b);
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		System.out.println("Read frame.");
		
		return new Frame(blocks, frameCount++);
	}
	
//	public void setServoPosition(int pan, int tilt) {
//		pspi.writeWord(0xff00);
//		pspi.writeWord(pan);
//		pspi.writeWord(tilt);
//	}
//	
//	public void setBrightness(int brightness) {
//		pspi.writeWord(0xfe00);
//		pspi.writeByte(brightness);
//	}
//	
//	public void setLEDOptions(int r, int g, int b) {
//		pspi.writeWord(0xfd00);
//		pspi.writeByte(r);
//		pspi.writeByte(g);
//		pspi.writeByte(b);
//	}
	
	@Override
	public String toString() {
		return "{ frameCount: "+frameCount+" }";
	}
	
	private PeekableSPI pspi;
	private int frameCount = 0;
	
}
