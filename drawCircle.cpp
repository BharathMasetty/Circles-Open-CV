#include "drawCircle.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <iostream>


using namespace std;
using namespace cv;

Circle *c_ptr;
Mat image(600, 600, CV_8UC3, Scalar(255,255,255));
const string window_name = "circle";

/**
 * Method to draw grid of squares in the image
 */
void Circle::drawGrid(){
	
	float startPoint = ((float)kDisplaySize - ((float)kNumGridSize)*kSpaceBetweenSquares)*0.5;
	float currX = startPoint;
	float currY = startPoint;
	for (uint i=0; i<kNumGridSize; i++) {
		for (uint j=0; j<kNumGridSize; j++) {
			gridPoints.emplace_back(currX, currY);
			rectangle(image_, Point(currX-squareSide, currY-squareSide), Point(currX+squareSide, currY+squareSide), Scalar(150,150,150), -1);
			currY += kSpaceBetweenSquares;
		}
		currX += kSpaceBetweenSquares;
		currY = startPoint;
	}
	
	imshow(kwindowName, image_);
}

/*
 * Method to draw circles
 */
void Circle::drawCircle(const Point &center, const int &r_min, const int &r_max, Mat &current_view){
	
	// inner cirlce
	circle(current_view, center, r_min, Scalar(0,0,255), 1, 8);
	// outer circle
	circle(current_view, center, r_max, Scalar(0,0,255), 1, 8);

}

/*
 * Method to draw highlighted squares at choses points in the grid.
 */
void Circle::drawRectangles(const std::vector<Point> &activePoints, Mat &current_view) {
	for (const Point &point : activePoints) {
		rectangle(current_view, Point(point.x-squareSide, point.y-squareSide), Point(point.x+squareSide, point.y+squareSide), Scalar(255,0,0), -1);
	}
}

/**
 * main process frame governed by the mouse functionality
 */
void Circle::processFrame(const int &event, const int &x, const int &y) {
	if (event == EVENT_LBUTTONDOWN){
		if (!start_draw){
			startx = x;
			starty = y;
			start_draw = true;
		}else {
			endx = x;
			endy = y;
			start_draw = false;
		}
	}
	
	if ((event == EVENT_MOUSEMOVE) && start_draw) {
		
		// Draw the circle
		Mat current_view;
		image_.copyTo(current_view);

		int radius = max(abs(startx - x), abs(starty - y));
		Point Center(startx, starty);
		circle(current_view, Center, radius, Scalar(255,0,0), 1, 8);

		// get the centers
		std::vector<Point> activeCenters;
		int r_min =  radius;
                int r_max =  radius;

		for (const Point& point : gridPoints) {
			// TODO: Make this threshold a constant
			float distance = norm(point - Center);
			if (abs(distance - radius) < 10.0){
				activeCenters.push_back(point);
				if (distance < r_min)
					r_min = distance;
				if (distance > r_max)
					r_max = distance;
			}
		}
		
		// highlight the rectanges and bounding circles
		drawRectangles(activeCenters, current_view);
		drawCircle(Center, r_min, r_max, current_view);
		imshow(window_name, current_view);
		}

}

/**
 * Callback function for mouse operations
 */
void on_mouse(int event, int x, int y, int, void *) {
	c_ptr->processFrame(event, x, y);
}

/**
 * Main execution
 */
int main(int argc, const char** argv) {

	namedWindow(window_name, WINDOW_AUTOSIZE);
	c_ptr = new Circle(image);
	setMouseCallback(window_name, on_mouse);
	imshow(window_name, image);
 	int key_pressed = 0;

	
	do {
	   key_pressed = 0xFF & waitKey(0);
	   
	   imshow(window_name, image);
	} while(key_pressed != 27);

	destroyWindow(window_name);
}
