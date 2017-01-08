package org.usfirst.frc.team252.robot;

import java.util.List;

public class Frame {
	
	public static class Block {
		public static enum SyncType {
			NORMAL, COLOR_CODE
		}
		SyncType sync;
		int checksum, signature, centerX, centerY, width, height;
	}
	
	Frame(List<Frame.Block> blocks, int id) {
		this.blocks = blocks;
		this.id = id;
	}
	
	public List<Frame.Block> getBlocks() {
		return blocks;
	}
	
	public int getID() {
		return id;
	}
	
	protected List<Frame.Block> blocks;
	protected int id;
	
}
