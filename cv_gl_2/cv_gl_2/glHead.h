#include <GL\glut.h>
#include <GL\GL.h>
#include <opencv2\opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Labeling.h"
#include "GLMetaseq.h"

using namespace cv;

BackgroundSubtractorGMG backGroundSubtractor;
// �J�����f���̕��ƍ���
int w = 640;
int h = 480;
int i = 0;
int th = 20;
int flg = 0;
int frame = 0;
double x, y;
double d;
double s;
char trackName[6][10];
int t[6] = { 0 };
int lim[6] = { 360, 360, 255, 255, 255, 255 };
Mat im, tmp, tmp2, gray, bg, diff, dst, output;					// �摜�I�u�W�F�N�g�̍쐬
Mat mask, hsv;
LabelingBS labeling;				// ���x�����O�֘A�̕ϐ�
RegionInfoBS *ri;
short *mask2;//���x�����O�o�͐�

Mat hand = Mat::zeros(h, w, CV_8UC3), mask_h, mask_h2 = Mat::zeros(h, w, CV_8UC3);

VideoCapture cap(0);				// �J�����̃L���v�`��

MQO_MODEL model;

int sc[2][3] = { { 100, 130, 130 }, { 110, 255, 250 } };


GLfloat lightpos[] = { 0.0, 250.0, 0.0, 1.0 };
GLfloat lightDiff[] = {1.0,1.0,1.0};
GLfloat lightAmb[] = { 1.0, 1.0, 1.0 };
GLfloat lightSpe[] = {0.1,0.1,0.1};
int WIDTH = 640, HEIGHT = 480;
float mx = 0.0;

Mat glRead = Mat::zeros(h, w, CV_8UC3);;
int ch = 3;

float distance, twist, elevation, azimuth;
float aspect = (float)w / (float)h;
#define AA 10
void cvInit();
void cvFunc();
void cvRect();
void cvHand();
void cvImage();

void glInit(int, char **);
void glLightInit();
void glFunc();
void setupView_();
void restoreView_();

void modelInit();