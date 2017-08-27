package com.team254.frc2017.pixy;

import java.util.List;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
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
			
			Mat trainingDataMat = new Mat(4, 2, CvType.CV_64FC1);
			for (int i = 0; i < 4; i++) {
//				trainingDataMat.put(i,0, trainingData[i]);
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
