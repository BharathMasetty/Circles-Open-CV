#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace cv;
using namespace std;

class Circle {
	public:
	
	Circle(cv::Mat &image) : image_(image), kNumGridSize(20), kDisplaySize(600), squareSide(2.0), kSpaceBetweenSquares(20.0),
		startx(0), starty(0), endx(0), endy(0), start_draw(false), leastSquaresEvaluated(false){

		drawGrid();
	}

	// Main process frame calling all functions
        void processFrame(const int &event, const int &x, const int &y);

	void  updateButton(const int &state);

	cv::Mat image_;

	private:
	// Function to draw the 20*20 Grid
	void drawGrid();

	// Function to draw the circle
	void drawCircle(const Point &center, const int &r_min, const int &r_max, Mat &current_view);

	// Function to draw rectangles from vector of points
	void drawRectangles(const std::vector<Point> &activePoints, Mat &current_view);

	void leastSquareFitCircle(Mat &current_view);


	// centers of square grid points
	std::vector<Point> gridPoints;

	std::vector<Point> contourPoints;
	
	// Size of the grid
	uint8_t kNumGridSize;
	
	//display image size
	uint32_t kDisplaySize;

	// side of squares in the grid
	float squareSide;

	// spacing between squares in the grid
	float kSpaceBetweenSquares;
	
	const std::string kwindowName = "circle";

	int startx, starty, endx, endy;
	bool start_draw;

	Point ls_center;
	float ls_radius;
	bool leastSquaresEvaluated;


	// Generate button limits
	int bxMin = 400;
	int bxMax = 600;
	int byMin = 500;
	int byMax = 600;
};	






	

	
