package org.usfirst.frc.team252.robot;

import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;

import javax.imageio.ImageIO;

import org.usfirst.frc.team252.robot.Frame.Block;

public class AdjustBlock {
	public Block AdjustBlock(Block block) {
		BufferedImage img = new BufferedImage(block.width, block.height, 1); // 1 is RGB
		img = rawToImage(block);
		img = undistortImage(img);
		block = imageToRaw(img);
		return block;
	}

	private BufferedImage rawToImage(Block block) {
		// TODO Auto-generated method stub
		return null;
	}
	
	private BufferedImage undistortImage(BufferedImage img) {
		// TODO Auto-generated method stub
		return null;
	}
		
	private Block imageToRaw(BufferedImage img) {
		// TODO Auto-generated method stub
		return null;
	}
}
