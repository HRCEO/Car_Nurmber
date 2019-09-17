//��ȣ�� �ν��� ��ȣ ���� �� ���Ϸ� ����

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Point2f srcPts[4], dstPts[4];

int main(void)
{

	VideoCapture cap("car_number.mp4");
	if (!cap.isOpened())
	{
		cerr << "���� ���� ����!" << endl;
		return -1;
	}
	cout << "width: " << cvRound(cap.get(CAP_PROP_FRAME_WIDTH)) << endl;
	cout << "height: " << cvRound(cap.get(CAP_PROP_FRAME_HEIGHT)) << endl;

	double fps = cap.get(CAP_PROP_FPS);
	cout << "FPS: " << fps << endl;

	int delay = cvRound(1000 / fps);

	Mat frame, dst, thr, copymain, labels, stats, centroids;
	Mat kernel(5,25, CV_8UC1, Scalar(1));

	while (true)  // �������� �ƴ� ���� ĸó�̸� �װ��� ���İ��� ���� �����ִ� ������ó�� �ȴ�.
	{
		cap >> frame;
		if (frame.empty()) break;

		// ȭ�� �߶󳻱� -> ���� ���縦 �̿��Ͽ� ���
		copymain = frame(Rect(150,40,490,300));  		// ���� ��ǥ x, ������ǥ y , ���� ����, ���α���

		cvtColor(copymain, dst, COLOR_BGR2GRAY);
		Mat sobel, bin;
		Sobel(dst, dst, CV_8U, 1, 0);
		blur(dst, dst, Size(3, 3));
		erode(dst, dst, Mat());

		// ���� ����ȭ
		threshold(dst, thr, 0, 255, THRESH_BINARY| THRESH_OTSU);

		// ���� ��ȣ�� ����
		int oldp3=0, oldp23 = 0;
		Mat morph_CLOSE;
		morphologyEx(thr, morph_CLOSE, MORPH_CLOSE, kernel);// ��� , ���� ���� , �� �� ����

		int count = connectedComponentsWithStats(morph_CLOSE, labels, stats, centroids,4);
		for (int i = 1; i < count; i++)
		{
			int* p = stats.ptr<int>(i);
			{
				if ((oldp3 != p[3])&&(oldp23!=(p[2] / p[3])))
				{
					if ((p[3] < 40) && (p[2] / p[3] > 1) && (p[4] < 2000) && (p[4] > 1500))
					{
						oldp3 = p[3];
						oldp23 = p[2] / p[3];
						rectangle(copymain, Rect(p[0], p[1], p[2], p[3]), Scalar(0, 255, 0), 1);
						srcPts[0] = Point2f(p[0], p[1]);
						srcPts[1] = Point2f((p[0] + p[2]), p[1]);
						srcPts[2] = Point2f((p[0] + p[2]), (p[1] + p[3]));
						srcPts[3] = Point2f(p[0], (p[1] + p[3]));
						// ��ȣ�� ����
						int w = p[2], h = p[3];
						dstPts[0] = Point2f(0, 0);
						dstPts[1] = Point2f(w - 1, 0);
						dstPts[2] = Point2f(w - 1, h - 1);
						dstPts[3] = Point2f(0, h - 1);

						Mat pers = getPerspectiveTransform(srcPts, dstPts);
						Mat Numeber;
						warpPerspective(copymain, Numeber, pers, Size(w, h));
						imshow("Numeber", Numeber);
					}
				}
				
			}
		}
		imshow("frame", frame);
		imshow("morph_CLOSE", morph_CLOSE); //����ȭ ����

		if (waitKey(delay) == 27) break;
		// ������ 1000ms �������� /fps �� ������ �־��� ������ ������ �ӵ��� �ɼ� �־���.  
		//int delay = cvRound(1000 / fps);
	}
}