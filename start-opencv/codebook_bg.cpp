#include"stdafx.h"





// int update_codebook(uchar *p, codeBook &c, unsigned *cbBounds,int numChannels)
// Updates the codebook entry with a new data point
//
// p	pointer to a YUV pixel
// c	codebook for this pixel
// cbBounds learning bounds for codebook(Rule of thumb:10)
// numChannels	number of color channels we're learning

// NOTES:
// cvBounds must be of length equal to numChannels

// RETURN:
// codebook index

int update_codebook(uchar *p, codeBook &c, unsigned *cbBounds, int numChannels) {

	unsigned int high[3], low[3], n;
	for (n = 0; n < numChannels; n++) {
		high[n] = *(p + n) + *(cbBounds + n);
		if (high[n] > 255)high[n] = 255;
		low[n] = *(p + n) - *(cbBounds + n);
		if (low[n] < 0)low[n] = 0;
	}

	int matchChannel;
	int i;
	//See if this fits an existing codeword

	for (i = 0; i < c.numEntries; i++) {
		matchChannel = 0;
		for (n = 0; n < numChannels; n++) {
			if ((c.cb[i]->learnLow[n] <= *(p + n)) &&
				//found an entry for this channel
				(*(p + n) <= c.cb[i]->learnHigh[n]))
			{
				matchChannel++;
			}
		}


		if (matchChannel == numChannels)//if an entry was found
		{
			c.cb[i]->t_last_update = c.t;
			//adjust this codeword for the first channel
			for (n = 0; n < numChannels; n++) {
				if (c.cb[i]->max[n] < *(p + n)) {
					c.cb[i]->max[n] = *(p + n);
				}
				else if (c.cb[i]->min[n]>*(p + n)) {
					c.cb[i]->min[n] = *(p + n);
				}
			}
			break;
		}

		//overhead to track potential stale entries
		for (int s = 0; s < c.numEntries; s++) {
			//track which codebook entries are going stale:
			int negRun = c.t - c.cb[s]->t_last_update;
			if (c.cb[s]->stale < negRun)c.cb[s]->stale = negRun;
		}

		//enter a new codeword if needed
		if (i == c.numEntries)//if no existing codeword found, make one
		{
			code_element **foo = new code_element*[c.numEntries + 1];
			for (int ii = 0; ii < c.numEntries; ii++) {
				foo[ii] = c.cb[ii];
			}

			foo[c.numEntries] = new code_element;
			if (c.numEntries) delete[] c.cb;
			c.cb = foo;
			for (n = 0; n < numChannels; n++) {
				c.cb[c.numEntries]->learnHigh[n] = high[n];
				c.cb[c.numEntries]->learnLow[n] = low[n];
				c.cb[c.numEntries]->max[n] = *(p + n);
				c.cb[c.numEntries]->min[n] = *(p + n);
			}
			c.cb[c.numEntries]->t_last_update = c.t;
			c.cb[c.numEntries]->stale = 0;
			c.numEntries += 1;
		}

		//slowly adjust learning bounds
		for (n = 0; n < numChannels; n++) {
			if (c.cb[i]->learnHigh[n] < high[n])c.cb[i]->learnHigh[n] += 1;
			if (c.cb[i]->learnLow[n] > low[n])c.cb[i]->learnLow[n] -= 1;

		}
		
	}
	return i;
}


//ѧϰ���ƶ�ǰ��Ŀ��ı���

//int clear_stale_entries(codeBook &c)
//During learning, after you've learned for some period of time,
// periodically call this to clear out stale codebook entries

//c		codebook to clean up
// RETURN
// number of entries cleared

int clear_stale_entries(codeBook &c){
	int staleThresh = c.t >> 1;//������ʱ���һ�룬Ϊ����ֵ
	int *keep = new int[c.numEntries];
	int keepCnt = 0;
	// see which codebook entries are too stale
	for (int i = 0; i < c.numEntries; i++) {
		if (c.cb[i]->stale>staleThresh)
		{
			keep[i] = 0;//mark for destruction
		}
		else
		{
			keep[i] = 1;//mark to keep
			keepCnt += 1;
		}
	}

	// keep only the good
	c.t = 0;//full reset on stale tracking
	code_element **foo = new code_element *[keepCnt];
	int k = 0;
	for (int ii = 0; ii < c.numEntries; ii++) {
		if (keep[ii]) {
			foo[k] = c.cb[ii];
			//we have to refresh these entries for next clearStale
			foo[k]->t_last_update = 0;
			k++;
		}
	}
	//clean up
	delete[] keep;
	delete[] c.cb;
	c.cb = foo;
	int numCleared = c.numEntries - keepCnt;
	c.numEntries = keepCnt;
	return numCleared;
}


//������֣�Ѱ��ǰ��Ŀ��
//uchar background_diff(uchar *p,codeBook &c,int minMod,int maxMod)
//
//Given a pixel and a codebook, determine if the pixel is covered by the codebook
//
//p	pixel pointer(YUV interleaved)
//c Codebook reference
//numChannels	number of channels we are testing
//maxMod	Add this (possibly negative) number onto max level when determining if new pixel is foreground
//minMod	Subract this (possibly negative) number from min level when determining if new pixel is foreground
//NOTES:
//minMode and maxMod must have length numChannels,
//e.g. 3channels=>minMod[3],maxMod[3]. There is one min and one max threshold per channel.
//
//RETURN:
// 0=>background,255=>foreground

uchar background_diff(
	uchar* p,
	codeBook& c,
	int numChannels,
	int* minMod,
	int* maxMod
	) {
	int matchChannel;
	int i;
	//see if this fits an existing codeword
	for (i = 0; i < c.numEntries; i++) {
		matchChannel = 0;
		for (int n = 0; n < numChannels; n++) {
			if ((c.cb[i]->min[n] - minMod[n] <= *(p + n)) &&
				(*(p + n) <= c.cb[i]->max[n] + maxMod[n])) {
				matchChannel++;//found an entry for this channel
			}
			else break;
		}
		if (matchChannel == numChannels) {
			break;//found an entry that matched all channels
		}

	}
	if (i >= c.numEntries)return 255;
	return 0;
}
//
//ʹ��codebook�����ָ����ͨ���������£�
//1. ʹ�ú���update_codebook()�ڼ���򼸷���ѵ��һ�������ı���ģ�ͣ�
//2. ���ú���clear_stale_entries()���stale������
//3. ������ֵminMod��maxMod����֪ǰ���ﵽ��õķָ
//4. ����һ�����߼���ĳ���ģ�ͣ�
//5. ͨ������background_diff()ʹ��ѵ���õ�ģ�ͽ�ǰ���ӱ����зָ������
//6. ���ڸ���ѧϰ�ı������أ�
//7. ��һ��Ƶ�ʽ���������£��ú���clear_stale_entries()��������stale��codebook������


//ͨ��codebook��Ч���ܺã�ѵ���������ٶȽϿ죬���ǲ��ܺܺô���ͬģʽ�Ĺ⡣

// # ����ǰ���������ͨ����
//����ʹ����ͨ�ɷַ���������ԭʼ�ָ�ͼ��ķ�����
//���ַ����ķ�ʽ��������������ģͼ��Ȼ��������̬ѧ������������С��������0���������á��ա������ؽ����ڡ�����������ʧ�ı�Ե���֡�
//Ȼ������ҵ����㹻�󡱴��ڵĲ�����������ѡ���ͳ����ЩƬ�Σ��ٻָ�����������
//
//ͷ�ļ��������£�
//void find_connected_components(IplImage *mask, int poly1_hull0, float perimScale, int *num, CvRect *bbs, CvPoint *centers) 
//This cleans up the foreground segmentation mask derived from calls to backgroundDiff
//
//mask		Is a grayscale (8-bit depth) "raw" mask image that will be clean up
//
//Optional parameters:
//poly1_hull0		If set, approximate connected COMPONENT by default Polygon, or else convex hull(0)
//perimScale		Len = Image(width + height) / perimScale.If contour len < this, delete that contour(default:4)
//num				Maximum number of rectangles and / or centers to return; on return will contain number filled (default:NULL)
//bbs				Pointer to bounding box rectangle vector of length num.(default setting: NULL)
//centers			Pointer to contour centers vector of length num(default:NULL)
//
//void find_connected_components(
//	IplImage *mask,
//	int poly1_hull0 = 1,
//	float perimScale = 4,
//	int *num = NULL,
//	CvRect *bbs = NULL,
//	CvPoint *centers = NULL
//	);

//for connected componence:
//Approx.threshold - the bigger it is, the simpler is the boundary 

#define CVCONTOUR_APPROX_LEVEL 2

//how many iterations of erosion and/or dilation there should be 

#define CVCLOSE_ITR 1

void find_connected_components(
IplImage *mask,
int poly1_hull0,
float perimScale,
int *num,
CvRect *bbs,
CvPoint *centers){
	static CvMemStorage* mem_storage = NULL;
	static CvSeq* contours = NULL;

	// clean up ram mask
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_OPEN, CVCLOSE_ITR);
	cvMorphologyEx(mask, mask, 0, 0, CV_MOP_CLOSE, CVCLOSE_ITR);

	//���������Ѿ�����ģͼ��������ˣ��ҵ������е�����
	// find contours around only bigger regions
	if(mem_storage == NULL) {
		mem_storage = cvCreateMemStorage(0);
	}
	else {
		cvClearMemStorage(mem_storage);
	}
	CvContourScanner scanner = cvStartFindContours(mask, mem_storage, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	//��һ��������С���������ö���λ�͹�����ʣ�µ����������Ӷ���CVCONTOUR_APPROX_LEVEL���á�
	CvSeq* c;
	int numCont = 0;
	while ((c = cvFindNextContour(scanner)) != NULL) {
		double len = cvContourPerimeter(c);
		//calculate perimeter len threshold:
		double q = (mask->height + mask->width) / perimScale;
		//Get rid of blob if its perimeter is too small:
		if (len < q) {
			cvSubstituteContour(scanner, NULL);
		}
		else {
			// Smooth its edges if its large enough
			CvSeq* c_new;
			if (poly1_hull0) {
				//Polygonal approximation
				c_new = cvApproxPoly(c, sizeof(CvContour), mem_storage, CV_POLY_APPROX_DP, CVCONTOUR_APPROX_LEVEL, 0);
			}
			else {
				//convex hull of the segmentation
				c_new = cvConvexHull2(c, mem_storage, CV_CLOCKWISE, 1);
				
				}

			cvSubstituteContour(scanner, c_new);
			numCont++;
			}
		}
	contours = cvEndFindContours(&scanner);


//��ǰ��Ĵ����У�CV_POLY_APPROX_DPʹ��Douglas - Peucher����㷨�����ã���CV_CLOCKWISE��Ĭ�ϵ�͹����������
//�ڽ��������Ƶ���ģͼ��֮ǰ������һЩ�򵥵Ļ�����ɫ��
// Just some convenience variables 
	const CvScalar CV_WHITE = CV_RGB(0xff, 0xff, 0xff);
	const CvScalar CV_BLACK = CV_RGB(0x00, 0x00, 0x00);

//����Ĵ����У����Ȱ���ģ��Ĳ����޳���Ȼ������ģͼ���ϻ����������������Ҫ����û��Ƿ����ֻ�������ͳ����Ϣ����Ӿ��κ����ģ���
// paint the found regions back into the image
cvZero(mask);
IplImage* maskTemp;
//calc center of mass and/or bounding rectangles
if (NULL != num){
	//User wants to collect statistics
	int N = *num, numFilled = 0, i = 0;
	CvMoments moments;
	double M00, M01, M10;
	maskTemp = cvCloneImage(mask);
	for (i = 0, c = contours; c != NULL; c = c->h_next, i++) {
		if (i < N) {
			//only process up to *num of them
			cvDrawContours(maskTemp, c, CV_WHITE, CV_WHITE, -1, CV_FILLED, 8);
			//find the center of each contour
			if (centers != NULL) {
				cvMoments(maskTemp, &moments, 1);
				M00 = cvGetSpatialMoment(&moments, 0, 0);
				M10 = cvGetSpatialMoment(&moments, 1, 0);
				M01 = cvGetSpatialMoment(&moments, 0, 1);
				centers[i].x = (int)(M10 / M00);
				centers[i].y = (int)(M01 / M00);

			}

			// bounding rectangles around blobs
			if (bbs != NULL) {
				bbs[i] = cvBoundingRect(c);
			}
			cvZero(maskTemp);
			numFilled++;
		}
		//draw filled contours into mask
		cvDrawContours(mask, c, CV_WHITE, CV_WHITE, -1, CV_FILLED, 8);
	}//end looping over contours
	*num = numFilled;
	cvReleaseImage(&maskTemp);
}
//else just draw processed contours into the mask
else {
	// the user doesn't want statistics, just draw the contours
	for (c = contours; c != NULL; c = c->h_next) {
		cvDrawContours(mask, c, CV_WHITE, CV_BLACK, -1, CV_FILLED, 8);
	}
}

//�����Ѿ��õ�һ�������õĳ��򣬿ɴ�ԭʼ������ģͼ�񴴽�����������ģͼ��
}