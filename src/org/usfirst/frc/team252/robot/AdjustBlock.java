package org.usfirst.frc.team252.robot;

import org.usfirst.frc.team252.robot.Frame.Block;

public class AdjustBlock {
	public Block AdjustBlock(Block block) {
		block.centerX = undistortX(block.centerX);
		block.centerY = undistortY(block.centerY);
		return block;
	}

	private int undistortY(int y) {
		int yCorrect = 0;
		return yCorrect;
	}

	private int undistortX(int x) {
		int xCorrect = 0;
		return xCorrect;
	}
}
