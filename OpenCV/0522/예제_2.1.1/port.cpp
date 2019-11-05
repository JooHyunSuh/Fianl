#include <stdio.h>
#include "Serial.h"
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <highgui.h>
#include <cv.h>

using namespace cv;
using namespace std;

void load_cascade(CascadeClassifier& cascade, string fname)
{
	String  path = "C:/opencv/sources/data/haarcascades/";   // 검출기 폴더
	String  full_name = path + fname;

	CV_Assert(cascade.load(full_name));               // 분류기 로드 및 예외처리
}

Mat preprocessing(Mat image)
{
	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);   // 명암도 영상변환
	equalizeHist(gray, gray);               // 히스토그램 평활화
	return gray;
}
//위치작성
void put_string(Mat &frame, string text, Point pt, int value) {
	text += to_string(value);
	Point shade = pt + Point(2, 2);
	int font = FONT_HERSHEY_SIMPLEX;
	putText(frame, text, pt, font, 0.7, Scalar(120, 200, 90), 2);  //작성문자
}
Point2d calc_center(Rect obj)                     // 사각형 중심 계산   
{
	Point2d c = (Point2d)obj.size() / 2.0;
	Point2d center = (Point2d)obj.tl() + c;
	return center;
}
int main()
{
	CascadeClassifier face_cascade, eyes_cascade;
	load_cascade(face_cascade, "haarcascade_frontalface_alt2.xml");   // 정면 얼굴 검출기
	//load_cascade(eyes_cascade, "haarcascade_eye.xml");         // 눈 검출기   

	VideoCapture capture(0);

	if (!capture.isOpened()) {
		cout << "카메라가 연결되지 않았습니다." << endl;
		exit(1);
	}

	CSerial serial;

	// STEP 1. SerialPort Connect
	if (!serial.Open())
	{
		printf("connect faliled");
		return -1;
	}
	else
		printf("connect successed\n");

	// 데이터 전송
	char *at = "AT+CONA810871B7E0E";

	if (!serial.SendData(at, 18)) {
		printf("send fail\n");
		return -1;
	}
	else
		printf("send successed\n");

	serial.ReadDataWaiting();
	Sleep(1000);

	//데이터 리드 

	void *buffer = 0;
	int curT = 0, oldT = 0; //, dispType =0;
							//while (1)
							//{
	curT = GetTickCount();

	if (curT - oldT > 10)
	{
		char buffer[256];
		int nBytesRead = serial.ReadData(buffer, sizeof(buffer));

		if (nBytesRead > 0)
		{
			for (int i = 0; i < nBytesRead; ++i)
			{
				printf("%c", buffer[i]);
			}
			printf("\n");
		}

		oldT = curT;
	}
	char *at3 = "Q\n";

	if (!serial.SendData(at3, 2)) {
		printf("send fail\n");
		return -1;
	}
	else
		printf("send successed2\n");


	bool check = true;
	HOGDescriptor hog;   //hog 기술자와 검출기를 기본 매개 변수로 만듦
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	clock_t begin = clock();
	clock_t end;

	int ju = 0;
	double result = 0;
	double result2 = 0;
	double final;

	for (;;) {
		Mat mat_frame;
		Mat dst;//grayscale
		//Mat dst1=dst+100;//밝기
	
		capture >> mat_frame;
		if (mat_frame.empty())
			continue;
		
		cvtColor(mat_frame, dst, CV_RGB2GRAY);

		Scalar red(0, 0, 255);
		Point pt1(0, 300), pt2(1000, 300), pt3(0, 200), pt4(1000, 200);

		//중간지점을 나타내는 선긋기
		//line(mat_frame, pt3, pt4, red, 4);
		//line(mat_frame, pt1, pt2, red, 4);

		vector<Rect> found, found_filtered;
		hog.detectMultiScale(mat_frame, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);

		//haar cascade
		vector<Rect> faces, eyes;
		vector<Point2d> eyes_center;

		Mat  gray = preprocessing(mat_frame);   // 전처리
		face_cascade.detectMultiScale(gray, faces, 1.1, 2, 0, Size(100, 100));   // 얼굴 검출
	
		for (int i = 0; i < faces.size(); i++)           // 얼굴 사각형 검출되면
		{   			
			Rect f = faces[i];//얼굴 사각형 
			rectangle(mat_frame, faces[i], Scalar(255, 255, 0), 3);   // 얼굴 검출 사각형 그리기
			
			f.x += cvRound(f.height*0.07);
			f.height = cvRound(f.height*0.8);
			int x = f.x + f.height;
			
			int arrayface[100];//100명으로 초기화
			for (int i = 0; i < 100; i++) {
				arrayface[i] = x;

				//시간 카운트
				if (arrayface[i]>= 200 && arrayface[i]<= 300) //중간 위치일 경우
				{
					if (check) {
						//가장 먼저 해당영역에 도착했을 경우의 위치 값
						end = clock();
						result = (double)(begin - end) / CLOCKS_PER_SEC;
						//cout << "수행시간(초) : " << result << endl;
						check = false;
						double first_clock = fabs(result);
						cout << "초반부 경과(초) : " << first_clock << endl;
					}
					begin = clock();

					//최종적으로 사람이 나갔을 때의 시간 
					result2 = (double)(begin - end) / CLOCKS_PER_SEC;
					cout << "수행시간(초) : " << result2 << endl;
					break;
					//result2값이 바로 중간 위치에 있는 초
				}
				begin = clock();
				end = clock();
			}

			if (result2 > 3 && ju <50) {
				cout << ju << endl;
				char *at2 = "F\n";
				//char *at0 = "\n";

				ju++;
				if (ju == 2) {
					if (!serial.SendData(at2, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(1)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;
				}
				else if (ju == 100) {
					char *at5 = "F\n";
					if (!serial.SendData(at5, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(2)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;
				}
				//delay 걸어주기
			}
		}
		
		size_t i;
		for (i = 0; i<found.size(); i++)
		{
			Rect r = found[i];
			size_t j;
			//더 큰 탐지 내부에 작은 탐지를 추가하지 말아야 함
			for (j = 0; j<found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;
			if (j == found.size())
				found_filtered.push_back(r);
		}

		for (i = 0; i<found_filtered.size(); i++)
		{
			Rect r = found_filtered[i];

			r.x += cvRound(r.width*0.1);   //cvRound: 부동 소수점 숫자를 가장 가까운 정수로 반올림
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);

			//rectangle(mat_frame, r.tl(), r.br(), Scalar(255, 0, 0), 3);   //tl: 오른쪽 상단 br: 오른쪽 하단
			
			int y = r.y + r.height;   //보행자의 발이 특정 위치에 도달했을 때 시간 측정시작
			int array[100];//100명으로 초기화
			for (int i = 0; i < 100; i++) {
				array[i]= y;

				//보행자 위치 출력
				//put_string(mat_frame, "(x): ", Point(10, 40), array[i][0]);
				put_string(mat_frame, "(y): ", Point(10, 60), array[i]);

				//시간 카운트
				if (array[i] >= 200 && array[i] <= 300) //중간 위치일 경우
				{
					if (check) {
						//가장 먼저 해당영역에 도착했을 경우의 위치 값
						end = clock();
						result = (double)(begin - end) / CLOCKS_PER_SEC;
						//cout << "수행시간(초) : " << result << endl;
						check = false;
						double first_clock = fabs(result);
						cout << "초반부 경과(초) : " << first_clock << endl;
					}
					begin = clock();

					//최종적으로 사람이 나갔을 때의 시간 
					result2 = (double)(begin - end) / CLOCKS_PER_SEC;
					cout << "수행시간(초) : " << result2 << endl;
					break;
					//result2값이 바로 중간 위치에 있는 초, 최대 2번까지만 시간을 늘려줄것
				}
				begin = clock();
				end = clock();
			}

			if (result2 > 3 && ju <50) {
				cout << ju << endl;
				char *at2 = "F\n";
				//char *at0 = "\n";

				ju++;
				if (ju == 2) {
					if (!serial.SendData(at2, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(1)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;

				}
				else if (ju == 100) {
					char *at5 = "F\n";
					if (!serial.SendData(at5, 2)) {
						printf("send fail\n");
						return -1;
					}
					else {
						printf("send successed(2)\n");
					}

					serial.ReadDataWaiting();
					//Sleep(8000);
					break;
				}

				//delay 걸어주기
			}
		}
			
		cvtColor(mat_frame, dst, CV_RGB2GRAY);
		Mat dst1 = dst + 100;//밝기

		imshow("Webcam1", dst1);

		imshow("source", mat_frame);

		if (waitKey(30) >= 0) {
			break;
		}

	}
	return 0;
}