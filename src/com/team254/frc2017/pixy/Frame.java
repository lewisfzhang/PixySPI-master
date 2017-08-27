package com.team254.frc2017.pixy;

import java.util.List;

import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.imgproc.Imgproc;

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
		
		public void undistort() {
			MatOfPoint2f src = new MatOfPoint2f();
			MatOfPoint2f dst = new MatOfPoint2f();
			Mat cameraMatrix = new Mat();
			Mat distCoeffs = new Mat();
			
			src.fromArray(new Point(centerX, centerY), new Point(centerX - width / 2, centerY - height / 2), new Point(centerX + width / 2, centerY + height / 2));
			
			for (int i = 0; i < PixyConstants.cameraMatrix.length; i++) {
				for (int j = 0; j < PixyConstants.cameraMatrix[i].length; j++) {
					cameraMatrix.put(i, j, PixyConstants.cameraMatrix[i][j]);
				}
			}
			
			for (int i = 0; i < PixyConstants.distCoeffs.length; i++) {
				for (int j = 0; j < PixyConstants.distCoeffs[i].length; j++) {
					distCoeffs.put(i, j, PixyConstants.distCoeffs[i][j]);
				}
			}
			
			Imgproc.undistortPoints(src, dst, cameraMatrix, distCoeffs);
		}
	}
	
	Frame(List<Frame.Block> blocks, int id) {
		this.blocks = blocks;
		this.id = id;
	}
	
	public List<Frame.Block> getBlocks() {
		return blocks;
	}
	
	public void undistortFrame() {
		for (Frame.Block block : blocks) {
			block.undistort();
		}
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
