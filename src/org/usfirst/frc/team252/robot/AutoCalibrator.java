package org.usfirst.frc.team252.robot;

public class AutoCalibrator {
	AutoCalibrator(Frame frame) {
		for (int i = frame.centerX-(frame.width/2); i <= frame.centerX+frame.width; i++) {
			for (int j = frame.centerY-(frame.height/2); j <= frame.centerY+frame.height; j++) {
				correctPixel(i, j);
			}
		}
	}
	private int correctPixel(int x, int y) {
		int xCorrect = 0;
		return xCorrect;
	}
}
