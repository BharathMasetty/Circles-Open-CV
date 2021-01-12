#include "drawCircle.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <numeric>


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
	
	
	rectangle(image_, Point(400,500), Point(600,600), Scalar(0, 85, 204), -1);
	putText(image_, "Generate", Point(450,550), FONT_HERSHEY_SIMPLEX,1, Scalar(255, 255,255),2);
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

	Point contactPoint(x,y);
	bool buttonPress = false;
	if (event == EVENT_LBUTTONDOWN){
		
		if(x>bxMin && x < bxMax){
			if(y>byMin && y<byMax){
				buttonPress= true;
			}
		}

		if (!buttonPress) {
			float distance = std::numeric_limits<float>::infinity();
			Point closestPoint(1000, 1000);
			for (const Point &point : gridPoints){
				float distanceToPoint = norm(point - contactPoint);
				if (distanceToPoint < distance) {
					closestPoint = point;
					distance = distanceToPoint;
				}
			}
			contourPoints.push_back(closestPoint);
			leastSquaresEvaluated = false;
			start_draw = false;
			buttonPress = false;
		}		
	}
		
	Mat current_view;
       	image_.copyTo(current_view);
	drawRectangles(contourPoints, current_view);	
		
	if (buttonPress){
		if (contourPoints.size() >= 3)
			start_draw = true;
	}
			
	if (start_draw && !leastSquaresEvaluated) {
			// Least squares fit
			leastSquareFitCircle(current_view);
			leastSquaresEvaluated = true;
		}

	if (leastSquaresEvaluated)		
		circle(current_view, ls_center, ls_radius, Scalar(255,0,0), 1, 8);
		
	imshow(window_name, current_view);


}

/**
 * Iterative least squares solution for fitting the circle
 * Reference: https://www.cse.iitb.ac.in/~cs749/spr2017/handouts/eberly_least_square_fitting.pdf
 */
void Circle::leastSquareFitCircle(Mat &current_view) {
	

	// Initial estimates for center, taken as the mean of all the data points.
	// Cost function in this case is summation of all norms from the center to all the data points.
	Point Center = accumulate(contourPoints.begin(), contourPoints.end(), Point(0,0))/(float)contourPoints.size();
	float radius;

	for (uint i=0; i< 1e6; i++) {
	
		float Cost = 0;
		std::vector<float> Norms;
		float La_ = 0;
		float Lb_ = 0;
		float a = Center.x;
		float b = Center.y;
		int m = contourPoints.size();
		float x_ = 0;
		float y_ = 0;
		
		for (const Point &point: contourPoints) {
			float value = norm(Center-point);
			Norms.push_back(value);
			Cost += value;
			La_ += (a - point.x)/value;
			Lb_ += (b - point.y)/value;
			x_ += point.x;
			y_ += point.y;
		}

		// This radius equation comes from equating the partial derivative of cost function the  wrt radius to zero. 
		radius = Cost/(float)m;

		La_ = La_/(float)m;
		Lb_ = Lb_/(float)m;
		x_ = x_/(float)m;
		y_ = y_/(float)m;
		
		// a_1 and b_1 are estimates of the x and y coordinates of the center for the next iteration. 
		// They come from equating the partial derivatives of the cost function wrt to a and b to zero respectively.
		float a_1 = x_ + radius*La_;
	        float b_1 = y_ + radius*Lb_;
		
		// Checking the convergance condition
		if (abs(a_1 - a)<1e-3 || abs(b_1 - b) <1e-3)
			break;

		// setting the new center for next iteration
		Center = Point(a_1,b_1);

	}	
	
	float Cost = 0;
	for (const Point &point: contourPoints) {
                        Cost += norm(point-Center);
                }
        radius = Cost/((float)contourPoints.size());

	// Final solutions of the iterative least squares process
	ls_center = Center;
	ls_radius = radius;

	circle(current_view, Center, (int)radius, Scalar(255,0,0),1,8);
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
