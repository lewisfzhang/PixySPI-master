package org.usfirst.frc.team252.robot;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.wpilibj.SPI;

public class PixyCam {
	
	public PixyCam() {
		SPI spi = new SPI(SPI.Port.kOnboardCS0);
		spi.setMSBFirst();
		spi.setClockActiveHigh();
		spi.setSampleDataOnRising();
		spi.setChipSelectActiveLow();
//		spi.setClockRate(50000);
		pspi = new PeekableSPI(spi);
	}
	
	public Frame.Block parseBlock() {
		System.out.println("parseBlock");
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
		int lastByte = 0x00;
		for (int n=0; true; n++) {
			int curByte = pspi.readByte();
//			System.out.println("curByte = "+curByte);
			if (lastByte==0xaa && curByte==0x55) break;
			lastByte = curByte;
//			if (n>=1000) System.out.println("FORCE EXIT LOOP");
		}
		
		// check if there's another sync word
		if (pspi.peekWord() == 0xaa55) {
			pspi.readWord(); // skip over it
			// handle frame boundary.....
		}
		
		// read the block data
		block.checksum = pspi.readWord();
		block.signature = pspi.readWord();
		block.centerX = pspi.readWord();
		block.centerY = pspi.readWord();
		block.width = pspi.readWord();
		block.height = pspi.readWord();
		int chk = block.signature+block.centerX+block.centerY+block.width+block.height;
		if (block.checksum != chk) {
			System.out.println("BLOCK HAD AN INVALID CHECKSUM ("+Integer.toHexString(block.checksum)+", should be "+Integer.toHexString(chk)+")");
		}
//		for (int n=0; n<6; n++) {
//			System.out.println("block byte: "+Integer.toHexString(pspi.readWord()));
//		}
		PeekableSPI.visualizeBytes(pspi.byteStream);
		pspi.byteStream.clear();
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
		
		
		System.out.println("getFrame");
		List<Frame.Block> blocks = new ArrayList<>();
		while (true) { // don't judge, it werks
			Frame.Block b = parseBlock();
			System.out.println("parseBlock done");
			if (b == null) break;
//			blocks.add(b);
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
