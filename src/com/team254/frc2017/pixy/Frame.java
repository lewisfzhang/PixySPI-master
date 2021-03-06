package com.team254.frc2017.pixy;

import java.util.List;

public class Frame {
	
	public static class Block {
		public static enum SyncType {
			NORMAL, COLOR_CODE
		}
		SyncType sync;
		public int checksum, signature, centerX, centerY, width, height;
		@Override
		public String toString() {
			return ":Block { signature: "+signature+", center: ("+centerX+", "+centerY+"), size: "+width+"x"+height+" }";
		}
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
	
	@Override
	public String toString() {
		String str = "Frame{ blockCount: "+blocks.size()+", id: "+ id;
		for (Frame.Block b : blocks) {
			str += "\n    "+b;
		}
		str += "\n}";
		return str;
	}
	
	protected List<Frame.Block> blocks;
	protected int id;
	
}
