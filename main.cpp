#include <ctime>
#include <iostream>
#include <vector>
#include <numeric>
#include <math.h>
#include "E101.h"
//#include "camera.h"

using namespace std;

/*initialization mess*/
void openGate();
int startMoving();
int setPath(int, int);
double distFromCenter();
void quadrant3();
double quadrant4();
void adjustMotors(double);
void slowReverse();
int currentQuadrant = 1;
int quad2_runCount = 1;
bool red_true(int, int, int);
bool green_true(int, int,int);
bool blue_true(int, int, int);
void takeUturn(int, int);
void takeUturn_Q4(int, int);
bool tIntersection;
bool fourWayIntersection;
bool lineOnTop;
bool lineOnLeft;
bool lineOnRight;
bool deadEnd;
bool goStraight;
double quad3_Instruction;
int mainCount = 0;
int current_Cylinder = 0;

/*main loop*/
int main(){
	openGate();   //un-comment for real use
	//currentQuadrant = 2; //comment out for real use
	cout<<currentQuadrant<<endl;
    int err = init(0);
    cout<<"After init() error="<<err<<endl;
    int totred = 0; // combined value of all red values of all pixels
    int totgreen = 0; //ditto for green
    int totblue = 0; // ditto for blue
    float redness = 0; // ratio of redness to greyness
    int n_red_pixels = 0;
    int red = 0;
    int green = 0;
    int blue = 0;
    int total_redpix =0;
    open_screen_stream();
    setPath(0, 0);
    while(true){
		mainCount++;
        take_picture();
        n_red_pixels = 0;
        total_redpix = n_red_pixels;
        red = 0;
	    green = 0;
	    blue = 0;
        totred = 0;
        redness = 0;
        totgreen = 0;
        totblue = 0;
        int right = 0;
	    int left = 32; // set left on the leftmost side of the cylinder
        sleep1(100);

        for ( int row = 0 ; row <240 ; row=row+1){
			for (int col=0; col<320; col=col+1){
			red = (int)get_pixel(row,col,0);
			green = (int)get_pixel(row,col, 1);
			blue = (int)get_pixel(row,col,2);
			
			if ((red>2*green) && (red>2*blue)){
				n_red_pixels++;
				//set_pixel(row,col,255,0,0);
				if (col < left) {
				left = col;
				} 
				if (col > right) {
				right = col;
				}
			}
			
			totred = totred + (int)get_pixel(row,col,0);
			totgreen = totgreen + (int)get_pixel(row, col, 1);
			totblue = totblue + (int)get_pixel(row,col, 2);
			 
		}
    
		} 

		 

		if(currentQuadrant == 2){
			set_motors(3,65);
			hardware_exchange();
			cout<<"running quadrant 2"<<endl;
			double error = distFromCenter();
			adjustMotors(error);
		} else if(currentQuadrant == 3){
			set_motors(3,63);
			hardware_exchange();
			cout<<"running quadrant 3"<<endl;
			double error = distFromCenter();
			quadrant3();
			adjustMotors(error);
		} else 	if(currentQuadrant == 4){
		set_motors(3,30);
		hardware_exchange();
		double q4_Instruction = quadrant4();
		adjustMotors(q4_Instruction);
		
		
				
		}
        int countClock_Quadrants;
        for(int i = 0; i < 320; i++){
			if(red_true((int)get_pixel(120,i,0),(int)get_pixel(120,i,1),(int)get_pixel(120,i,2))){
				//cout<<"maincount: "<<mainCount<<"countclock: "<<countClock_Quadrants<<"clculated clock: "<<mainCount-countClock_Quadrants<<endl;
				if(currentQuadrant == 2){
				//cout<<"found red line q2"<<endl;
				currentQuadrant++;
				countClock_Quadrants = mainCount;
				}
				if(mainCount-countClock_Quadrants > 10 && currentQuadrant == 3){
				//cout<<"found red line q3"<<endl;
				currentQuadrant++;
				}
				//cout<<"currentquad"<< currentQuadrant<<endl;
            }
		}
	}
    return 0;
}

/*opens the gate*/
void openGate(){
    char ip_addr[15] = "130.195.6.196";
    char message[] = "Please";
    char password[24];
    connect_to_server(ip_addr, 1024);
    send_to_server(message);
    receive_from_server(password);
    send_to_server(password);
	cout<<"The gate has been opened!"<<endl;
    cout<<"Welcome to Jurassic Park!!!"<<endl;
    currentQuadrant++;

    }

/*Method adjusts the motors based on the error and changes method based on current quadrant*/
void adjustMotors(double errorV){
	if(currentQuadrant == 2 || currentQuadrant == 3){
		if(errorV != 1000000){
		double vLeft;
		double vRight;
		vRight = 44 + (0.05 * errorV);
		vLeft = 52 + (0.05 * errorV);
		
		setPath(vRight,vLeft);
	} else { 
		slowReverse();
	}
	}
	if(currentQuadrant == 3){
		double vLeft;
		double vRight;
		if (lineOnTop && !lineOnLeft && !lineOnRight){
			cout<<"q3 straight"<<endl;
			vRight = 44 + (0.05 * errorV);
			vLeft = 52 + (0.05 * errorV);
			setPath(vRight,vLeft);
		} else if(lineOnTop && lineOnLeft && lineOnRight){
			cout<<"q3 4way intersection"<<endl;
			sleep1(100);
			vRight = 30;
			vLeft = 48;
			setPath(vRight,vLeft);
		} else if(!lineOnTop && lineOnLeft && lineOnRight){
			sleep1(100);
			cout<<"q3 right tIntersection"<<endl;
			vRight = 48;
			vLeft = 65;
			setPath(vRight,vLeft);
		} else if(!lineOnTop && !lineOnLeft && !lineOnRight){
			cout<<"uturn time"<<endl;
			vRight = 60;
			vLeft = 60;
			takeUturn(vRight, vLeft);
		}
	}
	if(currentQuadrant == 4){
		if(errorV != 1000000 && errorV != 2000000){
			
			double vLeft;
			double vRight;
			vRight = 44 + (0.05 * errorV);
			vLeft = 52 + (0.05 * errorV);
			setPath(vRight,vLeft);
		} else if(errorV == 2000000) { 
			cout<<"error 20000000"<<endl;
			slowReverse();
			//slowReverse();
			//slowReverse();


			current_Cylinder++;
			//turn left
		} else if(current_Cylinder == 1) {

			double vLeft;
			double vRight;
			vRight = 44;
			vLeft = 44;
			takeUturn_Q4(vRight, vLeft);
		} else if(current_Cylinder == 2) {
			double vLeft;
			double vRight;
			vRight = 52;
			vLeft = 52;
			takeUturn_Q4(vRight, vLeft);
		} else if(current_Cylinder == 3) {
			double vLeft;
			double vRight;
			vRight = 44;
			vLeft = 44;
			takeUturn_Q4(vRight, vLeft);
		} else if(current_Cylinder == 4) { //red  ball
			double vLeft;
			double vRight;
			vRight = 44;
			vLeft = 44;
			takeUturn_Q4(vRight, vLeft);
		}
		cout<<"currentcyl"<<current_Cylinder<<endl;

	}

}

/*method takes a uturn until it sees a black line in the middle of the camera frame*/
void takeUturn(int vRight, int vLeft){
	set_motors(vRight, vLeft);
	while(get_pixel(120, 160, 3) > 100) {
		take_picture();
		cout<<"processing loop-d-loop"<<endl;
		setPath(vRight, vLeft);
    }
}

void takeUturn_Q4(int vRight, int vLeft){
	set_motors(vRight, vLeft);
	if(current_Cylinder == 0){
	while(!(red_true(get_pixel(120, 160, 0), get_pixel(120, 160, 1), get_pixel(120, 160, 2)))) {
		take_picture();
		cout<<"processing loop-d-loopred"<<endl;
		setPath(vRight, vLeft);
    }
}
if(current_Cylinder == 1){
	while(!(green_true(get_pixel(120, 160, 0), get_pixel(120, 160, 1), get_pixel(120, 160, 2)))) {
		take_picture();
		cout<<"processing loop-d-loopgreen"<<endl;
		setPath(vRight, vLeft);
    }
}
if(current_Cylinder == 2){
	while(!(blue_true(get_pixel(120, 160, 0), get_pixel(120, 160, 1), get_pixel(120, 160, 2)))) {
		take_picture();
		cout<<"processing loop-d-loopblue"<<endl;
		setPath(vRight, vLeft);
    }  
if(current_Cylinder == 3){
	while(!(red_true(get_pixel(120, 160, 0), get_pixel(120, 160, 1), get_pixel(120, 160, 2)))) {
		take_picture();
		cout<<"processing loop-d-loopblue"<<endl;
		setPath(vRight, vLeft);
    }
if(current_Cylinder == 4){ //red ball
	while(!(red_true(get_pixel(120, 160, 0), get_pixel(120, 160, 1), get_pixel(120, 160, 2)))) {
		take_picture();
		cout<<"processing loop-d-loopblue"<<endl;
		setPath(vRight, vLeft);
    }
}
}
}
}


/*slow reverse method reverses robot to recalculate, used for Q2*/
void slowReverse(){
	setPath(52, 44);
}

/*sets motor to passed speed and does hardware exchange*/
int setPath(int rightMotor, int leftMotor){
    set_motors(1, rightMotor);
    set_motors(5, leftMotor);
    hardware_exchange();
    return 1;
}

/*calculates error by calculating distance from center*/
double distFromCenter(){
    int numBlackPixels = 0;
    int totalBlack = 0;
    int blackMap[320];
    double error = 0;
    take_picture();
    for(int i = 0; i < 320; i++){
        int isB;
            if(get_pixel(120, i, 3) > 100){
				isB = 0;
            } else {
				isB = 1;
			}
			blackMap[i] = isB;
    }
    for(int i = 0; i < 320; i++){
		if(blackMap[i] > 0){
			numBlackPixels++;
			totalBlack = totalBlack + i;
		}
	}
	if(numBlackPixels > 0){
		double blackLineLocation = totalBlack/numBlackPixels;
		error = blackLineLocation - 160;
		return error;
	} else {
		return 1000000;
}
if(quad2_runCount <= 1){
	currentQuadrant++;
}
quad2_runCount++;
}

void quadrant3() { //sets booleans if there is a line on top, and or left / right to true
	fourWayIntersection = false;
	tIntersection = false;
	lineOnTop = false;
	lineOnLeft = false;
	lineOnRight = false;
	deadEnd = false;
	goStraight = false;
    int numBlackPixelsLeft = 0;
    int numBlackPixelsRight = 0;

    int numBlackPixelsTop = 0;
    take_picture();
    for (int i = 0; i < 240; i++) {
        if (get_pixel(i, 0, 3) > 100) {
        } else {
            numBlackPixelsLeft++;
        }
    }

    for (int i = 0; i < 320; i++) {
        if (get_pixel(0, i, 3) > 100) {
        } else {
            numBlackPixelsTop++;
        }
    }

    for (int i = 0; i < 240; i++) {
        if (get_pixel(i, 240, 3) > 100) {
        } else {
            numBlackPixelsRight++;
        }
    }

    if(numBlackPixelsTop > 10){
	    lineOnTop = true;
	}
	if(numBlackPixelsRight > 10){
	    lineOnRight = true;
	}
	if(numBlackPixelsLeft > 10){
	    lineOnLeft = true;
	}
}

double quadrant4(){
	if(current_Cylinder == 0){ //red cylinder
    int numRedPixels = 0;
    int totalRed = 0;
    int redMap[320];
    double error = 0;
    take_picture();
    cout<<"iterate"<<current_Cylinder<<endl;
    for(int i = 0; i < 320; i++){
        int isR;
            if(!(red_true(get_pixel(120, i, 0), get_pixel(120, i, 1), get_pixel(120, i, 2)))){
				isR = 0;
            } else {
				isR = 1;
			}
			redMap[i] = isR;
    }
    for(int i = 0; i < 320; i++){
		if(redMap[i] > 0){
			numRedPixels++;
			totalRed = totalRed + i;
		}
	}
	cout<<"red pix"<<numRedPixels<<endl;
	if(numRedPixels > 0 && numRedPixels < 200){
		double redLineLocation = totalRed/numRedPixels;
		error = redLineLocation - 160;
		cout<<"error"<<error<<endl;
		return error;
	} else if(numRedPixels > 200){
		return 2000000;
	}
	else {
		return 1000000;
}
}
if(current_Cylinder == 1){ //green cylinder
	cout<<"processing green cylinder"<<endl;
    int numRedPixels = 0;
    int totalRed = 0;
    int redMap[320];
    double error = 0;
    take_picture();
    for(int i = 0; i < 320; i++){
        int isR;
            if(!(green_true(get_pixel(120, i, 0), get_pixel(120, i, 1), get_pixel(120, i, 2)))){
				isR = 0;
            } else {
				isR = 1;
			}
			redMap[i] = isR;
    }
    for(int i = 0; i < 320; i++){
		if(redMap[i] > 0){
			numRedPixels++;
			totalRed = totalRed + i;
		}
	}
	cout<<"red pix"<<numRedPixels<<endl;
	if(numRedPixels > 0 && numRedPixels < 260){
		double redLineLocation = totalRed/numRedPixels;
		error = redLineLocation - 160;
		cout<<"error"<<error<<endl;
		return error;
	} else if(numRedPixels > 260){
		return 2000000;
	}
	else {
		return 1000000;
}
}
if(current_Cylinder == 2){ //blue cylinder
    int numRedPixels = 0;
    int totalRed = 0;
    int redMap[320];
    double error = 0;
    take_picture();
    for(int i = 0; i < 320; i++){
        int isR;
            if(!(blue_true(get_pixel(120, i, 0), get_pixel(120, i, 1), get_pixel(120, i, 2)))){
				isR = 0;
            } else {
				isR = 1;
			}
			redMap[i] = isR;
    }
    for(int i = 0; i < 320; i++){
		if(redMap[i] > 0){
			numRedPixels++;
			totalRed = totalRed + i;
		}
	}
	cout<<"red pix"<<numRedPixels<<endl;
	if(numRedPixels > 0 && numRedPixels < 200){
		double redLineLocation = totalRed/numRedPixels;
		error = redLineLocation - 160;
		cout<<"error"<<error<<endl;
		return error;
	} else if(numRedPixels > 200){
		return 2000000;
	}
	else {
		return 1000000;

}
}
if(current_Cylinder == 3){ //red ball
    int numRedPixels = 0;
    int totalRed = 0;
    int redMap[320];
    double error = 0;
    take_picture();
    for(int i = 0; i < 320; i++){
        int isR;
            if(!(red_true(get_pixel(120, i, 0), get_pixel(120, i, 1), get_pixel(120, i, 2)))){
				isR = 0;
            } else {
				isR = 1;
			}
			redMap[i] = isR;
    }
    for(int i = 0; i < 320; i++){
		if(redMap[i] > 0){
			numRedPixels++;
			totalRed = totalRed + i;
		}
	}
	cout<<"red pix"<<numRedPixels<<endl;
	if(numRedPixels > 0 && numRedPixels < 240){
		double redLineLocation = totalRed/numRedPixels;
		error = redLineLocation - 160;
		cout<<"error"<<error<<endl;
		return error;
	} else if(numRedPixels > 240){
		return 2000000;
	}
	else {
		return 1000000;

}
return 1000000; 
}
return 1000000; 
}

/*function calculates if the pixel the robot is looking at is red or not*/
bool red_true(int totred, int totgreen, int totblue){
return(((totred > (2*totblue)) && (totred > (2*totgreen)))); 
}

bool green_true(int totred, int totgreen, int totblue){
return(((totgreen > (1.1*totblue)) && (totgreen > (1.1*totred)))); 
}

bool blue_true(int totred, int totgreen, int totblue){
return(((totblue > (1.1*totred)) && (totblue > (1.1*totgreen)))); 
}
