package org.usfirst.frc.team252.robot;

import java.awt.Point;
import java.util.ArrayList;
import java.util.List;

import org.usfirst.frc.team252.robot.Frame.Block;
import edu.wpi.first.wpilibj.SPI;

public class PixyCam {
    public static double fx, fy, cx, cy, k1, k2, k3;
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
		//get Camera Matrix and Distortion Coefficients
		 
		Frame.Block block = new Frame.Block();
		
		// Wait for sync
		while (pspi.readWord(pspi.data[0], pspi.data[1]) != 0xaa55) {}
		// check if there's another sync word
		
		// read the block data
		block.checksum = pspi.readWord(pspi.data[2], pspi.data[3]);
		block.signature = pspi.readWord(pspi.data[4], pspi.data[5]);
		block.centerX = pspi.readWord(pspi.data[6], pspi.data[7]);
		block.centerY = pspi.readWord(pspi.data[8], pspi.data[9]);
		block.width = pspi.readWord(pspi.data[10], pspi.data[11]);
		block.height = pspi.readWord(pspi.data[12], pspi.data[13]);
		Point center = transformCoordinates(block.centerX, block.centerY);
		double chk = block.signature+block.centerX+block.centerY+block.width+block.height;
		if (block.checksum != chk) {
			System.out.println("BLOCK HAD AN INVALID CHECKSUM ("+Integer.toHexString(block.checksum)+", should be "+Double.toHexString(chk)+")");
			return null;
		}
        block.centerX = center.x;
        block.centerY = center.y;
        undistortFourCorners(block);
		return block;
	}
	
	public Frame getFrame() {
		List<Frame.Block> blocks = new ArrayList<>();
		while (true) { // don't judge, it works
			Frame.Block b = parseBlock();
			if (b == null) break;
			blocks.add(b);
			
			
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		return new Frame(blocks, frameCount++);
	}
	private void undistortFourCorners(Block block) {
        // based on the inverted plane of a camera, you can calculate the four side x and y values of the rectangular block
        double leftSideX = block.centerX-(block.width/2);
        double rightSideX = block.centerX+(block.width/2);
        double topSideY = block.centerY-(block.height/2);
        double bottomSideY = block.centerY+(block.height/2);

        // Undistort the two opposite points of the rectangular block
        Point bottomLeft = transformCoordinates(leftSideX, bottomSideY);
        Point topRight = transformCoordinates(rightSideX, topSideY);

        // Calculate the width and height based on opposite points of rectangular block
        block.width = topRight.x - bottomLeft.x;
        block.height = bottomLeft.y - topRight.y;
    }
    private static Point transformCoordinates(double xDistorted, double yDistorted) {
        // put in terms of cx and cy being the origin and normalize
        double xDistNormalized = (xDistorted - cx)/400;
        double yDistNormalized = (yDistorted - cy)/400;
        // apply undistortion
        double rDistorted = Math.hypot(xDistNormalized, yDistNormalized);
        double rUndistorted = radiusFuncInv(rDistorted);
        double xUndistorted = xDistNormalized*rUndistorted/rDistorted;
        double yUndistorted = yDistNormalized*rUndistorted/rDistorted;
        // convert back to pixel space and return
        Point undistort = new Point((int) (Math.round(xUndistorted*400 + cx)), (int) (Math.round(yUndistorted*400 + cy)));
        return undistort;
    }
    private static double radiusFuncInv(double rDistorted) { // Function for Undistortion
        double rSquared = rDistorted*rDistorted;
        double rUndistorted = rDistorted;
        rDistorted *= rSquared;
        rUndistorted -= k1*rDistorted;
        rDistorted *= rSquared;
        rUndistorted += (3*k1*k1 - k2)*rDistorted;
        rDistorted *= rSquared;
        rUndistorted += (-12*k1*k1*k1 + 8*k1*k2 - k3)*rDistorted;
        return rUndistorted;
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
