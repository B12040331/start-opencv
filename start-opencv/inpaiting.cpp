

////���ȣ�Ϊ��Ҫ�Ĳ�ͬ��ʱͼ���ͳ������ͼ�񴴽�ָ�롣���������ڸ���ͼ��Ĳ�ͬ���Ͷ��Ժ�ʹ�õ�ͼ��ָ������
//// global storage
////float, 3-channel images
//IplImage *IavgF, *IdiffF, *IprevF, *IhiF, *IlowF;
//IplImage *Iscratch, *Iscratch2;
////float, 1-channel images
//IplImage *Igray1, *Igray2, *Igray3;
//IplImage *Ilow1, *Ilow2, *Ilow3;
//IplImage *Ihi1, *Ihi2, *Ihi3;
////byte, 1-channel image
//IplImage *Imaskt;
////counts number of images learned for averaging later.
//float Icount;
////����������һ����������Ҫ��������ʱͼ������ڴ�
//// I is just a sample image for allocation  
//// (passed in for sizing)
//
//void AllocateImages(IplImage* I) {
//	CvSize sz = cvGetSize(I);
//	IavgF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IdiffF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IprevF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IhiF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	IlowF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	Ilow1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ilow2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ilow3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ihi1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ihi2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Ihi3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//
//	cvZero(IargF);
//	cvZero(IdiffF);
//	cvZero(IprevF);
//	cvZero(IlowF);
//	Icount = 0.00001;//protect against divide by zero
//
//	Iscratch = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	Iscratch2 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
//	Igray1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Igray2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Igray3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
//	Imaskt = cvCreateImage(sz, IPL_DEPTH_8U, 1);
//
//	cvZero(Iscratch);
//	cvZero(Iscratch2);
//
//}
