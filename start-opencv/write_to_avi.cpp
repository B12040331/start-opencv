#include"stdafx.h"
int write_to_avi(char* input,char* output) {
	//����豸����֮�󣬿���ͨ������cvWriteFrame()��֡����Ƶ��д���ļ�
	//д������󣬵���cvReleaseVideoWriter()���ͷ���Դ

	// convert a video to grayscale
	//argv[1]: input video file
	// argv[2]: name of new output file

	CvCapture* capture = 0;
	capture = cvCreateFileCapture(input);
	if (!capture) {
		return -1;
	}
	IplImage* bgr_frame = cvQueryFrame(capture);//Init the video read
	double fps = cvGetCaptureProperty(
		capture,
		CV_CAP_PROP_FPS);
	CvSize size = cvSize(
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH),
		(int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT)
		);
	CvVideoWriter* writer = cvCreateVideoWriter(
		output,
		CV_FOURCC('M', 'J', 'P', 'G'),
		fps,
		size);

	IplImage* logpolar_frame = cvCreateImage(
		size,
		IPL_DEPTH_8U,
		3
		);
	while ((bgr_frame = cvQueryFrame(capture)) != NULL)
	{
		cvLogPolar(bgr_frame,
			logpolar_frame,
			cvPoint2D32f(bgr_frame->width/2,bgr_frame->height / 2),
			40, 
			CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
		// cvLogPolar���һ������flagsĬ��ֵΪ9������������ö�ٳ�����Ӹպ�Ϊ9

		cvWriteFrame(writer, logpolar_frame);

	}

	cvReleaseVideoWriter(&writer);
	cvReleaseImage(&logpolar_frame);
	cvReleaseCapture(&capture);
	return 0;

}

/*��һ����Ƶ�ļ���ͨ��cvQueryFrame()����������Ƶ��Ȼ��ʹ��cvGetCaptureProperty()�������Ƶ���ĸ�����Ҫ���ԡ�
����Ƶͼ��ת��Ϊ�����������ʽ����ת�����ͼ����֡д����Ƶ�ļ���ֱ�����������

�ڵ��ú���cvCreateVideoWriter()ʱ����һ������������ָ���½���Ƶ�ļ������ƣ��ڶ�����������Ƶѹ���ı����ʽ��

���۲�ȡ���ֱ�����ʽ�����뱣֤�Լ��ĵ����������ֱ��������
OpenCV�ú�CV_FOURCC()��ָ�������ʽ�������4���ַ����������ɱ��������4�ֱ�ǡ�
Motion JPEG�����ʽ�����ֱ�Ǿ���MJPG��

����������������ָ�����ŵ�֡�ʺ���Ƶͼ��Ĵ�С��
*/