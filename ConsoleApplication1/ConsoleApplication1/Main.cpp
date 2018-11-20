#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

int redThre = 49;//��ɫ������ֵ
int saturationTh = 7;//���Ͷ���ֵ
#define threshold_diff1 25 //֡���ֵ

using namespace cv;
using namespace std;


//��֡��ַ� ��̬���
Mat frameDiff(Mat prevFrame, Mat nextFrame){
	Mat diffFrame1, diffFrame2, output;
	output.create(prevFrame.size(), CV_8UC1);
	absdiff(nextFrame, prevFrame, diffFrame1);
	for (int i = 0; i < diffFrame1.rows; i++){
		for (int j = 0; j < diffFrame1.cols; j++){
			if (abs(diffFrame1.at<uchar>(i, j)) >= threshold_diff1)
				diffFrame1.at<uchar>(i, j) = 255;
			else
				diffFrame1.at<uchar>(i, j) = 0;
		}
	}
	bitwise_and(diffFrame1, diffFrame1, output);
	medianBlur(output, output, 5);//��ֵ�˲�
	//dilate(output, output, Mat(5, 5, CV_8UC1));//��̬ѧ����8
	//erode(output, output, Mat(5, 5, CV_8UC1));
	return output;
}


//�Ҷȴ���
Mat setGray(Mat frame){
	cvtColor(frame, frame, CV_BGR2GRAY);
	Sobel(frame, frame, CV_8U, 1, 0, 3, 0.4, 128);//��Ե�������
	return frame;
}


//��ȡ��Ƶһ֡ͼ��
Mat getFrame(VideoCapture video){
	Mat frame;
	video >> frame;
	return frame;
}


//RGB & HSI��ɫ���
Mat CheckColor(Mat inImg, Mat grayFrame){
	Mat fireImg;
	fireImg.create(inImg.size(), CV_8UC1);
	Mat multiRGB[3];
	split(inImg, multiRGB);//��ԭͼ���ֳ�R��G��B����ͨ��
	for (int i = 0; i < inImg.rows; i++){
		for (int j = 0; j < inImg.cols; j++){
			float B, G, R;
			B = multiRGB[0].at<uchar>(i, j);
			G = multiRGB[1].at<uchar>(i, j);
			R = multiRGB[2].at<uchar>(i, j);

			float maxValue = max(max(B, G), R);
			float minValue = min(min(B, G), R);
			//����HSI�е�S
			double S = (1 - 3.0*minValue / (R + G + B));
			//��ɫ����о�
			if (!(R > redThre&&R >= G&&G >= B&&S > ((255 - R)*saturationTh / redThre)) && (grayFrame.at<uchar>(i, j) == 255)){
				grayFrame.at<uchar>(i, j) = 0;
			}
		}
	}
	//dilate(grayFrame, grayFrame, Mat(5, 5, CV_8UC1));
	imshow("fire", grayFrame);
	return grayFrame;
}


//������
int main(int argc, unsigned char* argv[])
{
	VideoCapture capture1("test1.avi");//��ȡ��Ƶ��Դ
	VideoCapture capture2("test1.avi");
	Mat prevFrame, nextFrame;
	prevFrame = getFrame(capture1);//��ȡ��һ֡ͼ��
	prevFrame = setGray(prevFrame);
	nextFrame = getFrame(capture1);//��ȡ�ڶ�֡ͼ��
	nextFrame = setGray(nextFrame);
	while (1){
		Mat frame;
		capture2 >> frame;
		if (frame.empty())
			break;
		imshow("org", frame);
		CheckColor(frame, frameDiff(prevFrame, nextFrame));
		//imshow("dyn", frameDiff(prevFrame,nextFrame));

		prevFrame = nextFrame;
		nextFrame = getFrame(capture1);
		if (nextFrame.empty())
			break;
		nextFrame = setGray(nextFrame);
		waitKey(1);
	}
	return 0;
}
