#include"stdafx.h"

//��ImageROI������ĳ��Χ������
int roi_add(const char* filename, int x, int y, int width, int height, int add)
{
	IplImage* src;
	if(src = cvLoadImage(filename, 1))
	{
		IplImage* origin;
		origin = cvLoadImage(filename);
		cvSetImageROI(src, cvRect(x, y, width, height));
		cvAddS(src, cvScalar(add), src);
		cvResetImageROI(src);
		cvNamedWindow("ROI_Add", 1);
		cvNamedWindow("Origin", 1);
		cvShowImage("ROI_Add", src);
		cvShowImage("Origin", origin);
		cvWaitKey();
	}
	return 0;
}

/*ROIҪ����Щ������widthStep�������á�
��Щʱ���ڴ�������У����ڲ������������úͱ���һ��ͼ��Ķ����������ڻ״̬��
����ROIֻ�ܴ��д����ұ��벻�ϵ����ú����á�*/
int width_step_add(IplImage* interest_img, CvRect interest_rect)
{
	// Assuming IplImage* interest_img;
	
	// and CvRect interest_rect;
	
	// Use widthStep to get a rigion of interest
	// <Ailternate method>

	IplImage* sub_img = cvCreateImageHeader(
		cvSize(
			interest_rect.width,
			interest_rect.height
			),
		interest_img->depth,
		interest_img->nChannels
		);

	sub_img->origin = interest_img->origin;

	sub_img->widthStep = interest_img->widthStep;

	sub_img->imageData = interest_img->imageData +
		interest_rect.y * interest_img->widthStep +
		interest_rect.x * interest_img->nChannels;

	cvAddS(sub_img, cvScalar(1), sub_img);
	cvReleaseImageHeader(&sub_img);

	/*�����ģ�壬
	�ڴ���ʾ���У�cvAddS()����������ĸ�����Ĭ��ֵΪ�գ�
	const CvArr* mask=NULL��
	����һ��8λ��ͨ�����飬����Ѳ������Ƶ�������״�ķ�0���ص���������
	���ROI�����������ģ��仯�����̽��ᱻ������ROI������Ľ�������
	*/

	return 0;

}

/*alphablend
	src2��alpha�ں�ROI��(0,0)��ʼ��src1��ROI��(x,y)��ʼ
	*/
int alphablend(char* src1, char* src2, int x, int y, int width, int height, double alpha, double beta)
{
	IplImage *img1, *img2;
	if (((img1 = cvLoadImage(src1)))&&(img2 = cvLoadImage(src2)))
	{
		cvSetImageROI(img1, cvRect(x,y,width, height));

		cvSetImageROI(img2, cvRect(0,0, width, height));

		cvAddWeighted(img1, alpha, img2, beta, 0.0, img2);

		cvResetImageROI(img1);


		cvNamedWindow("Alpha_blend", 1);
		cvShowImage("Alpha_blend", img1);
		cvWaitKey();
		
	}
	return 0;
	
}