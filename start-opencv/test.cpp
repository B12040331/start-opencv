#include"stdafx.h"

using namespace cv;

int test()
{
	//����һ��ͼƬ
	Mat img = imread("pic.jpg");
	//����һ������
	cvNamedWindow("��������");
	//�ڴ�������ʾ��Ϸԭ��
	imshow("��������", img);
	//�ȴ�5000ms�󴰿��Զ��ر�
	waitKey(5000);
	return 0;
}