#include "glHead.h"
#define CH 5
#define DIST 1500

using namespace cv;

void track1(int *value, void *ptr);
void display();
void idle();

//void keyboard();

void display(){
	cvFunc();
	glFunc();
	cvImage();
}

void idle(){
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	if (!cap.isOpened()) return -1;		// キャプチャ失敗時のエラー処理
	cvInit();
	glInit(argc, argv);
	//glLightInit();
	modelInit();

	//glutMainLoop();
	while (1){
		display();
	}
	mqoDeleteModel(model);
	mqoCleanup();
	return 0;
}

void  cvInit(){

	strcmp(trackName[0], "H_L");
	strcmp(trackName[1], "H_H");
	strcmp(trackName[2], "S_L");
	strcmp(trackName[3], "S_H");
	strcmp(trackName[4], "V_L");
	strcmp(trackName[5], "V_H");
	mask2 = new short[w*h];

	t[0] = sc[0][0];
	t[1] = sc[1][0];
	t[2] = sc[0][1];
	t[3] = sc[1][1];
	t[4] = sc[0][2];
	t[5] = sc[1][2];
	/*
	namedWindow("test", CV_WINDOW_AUTOSIZE);
	//createTrackbar("H_U", "test", &t1, 360);
	//createTrackbar("H_H", "test", &t2, 360);
	//createTrackbar("test3", "test", &t3, 360);
	for (int i = 0; i < 6; i++)
		createTrackbar(trackName[i], "test", &t[i], lim[i]);
	resizeWindow("test", 300, 0);
	*/
}

void cvFunc(){

	cap >> im;								// カメラから画像を取得
	imshow("im", im);
	cvRect();
	cvHand();
}

void cvRect(){
	int c = cvWaitKey(1);
	if (c == 'w'){
		sc[0][1] += CH;
	}
	else if (c == 's'){
		sc[0][1] -= CH;
	}
	else if (c == 'e'){
		sc[1][1] += CH;
	}
	else if (c == 'd'){
		sc[1][1] -= CH;
	}
	if (sc[0][0] > 360)
		sc[0][0] = 360;
	if (sc[1][0] < 0)
		sc[1][0] = 0;
	tmp = im.clone();
	cvtColor(tmp, hsv, CV_BGR2HSV);			// 画像をRGBからHSVに変換
	inRange(hsv, Scalar(t[0], t[2], t[4]), Scalar(t[1], t[3], t[5]), mask);	// 色検出でマスク画像の作成
	dilate(mask, mask, Mat(), Point(-1, -1), 9);
	erode(mask, mask, Mat(), Point(-1, -1), 9);
	// 白領域が無い場合のエラー処理
	rectangle(mask, Point(0, 0), Point(1, 1), Scalar(255), -1);

	//ラベリング処理
	labeling.Exec((uchar *)mask.data, mask2, w, h, true, 15);
	//最大の領域を四角で囲む
	ri = labeling.GetResultRegionInfo(0);
	int x1, y1, x2, y2;
	ri->GetMin(x1, y1);
	ri->GetMax(x2, y2);
	rectangle(im, Point(x1, y1), Point(x2, y2), Scalar(255, 0, 0), 3);
	s = (x2 - x1)*(y2 - y1);
	x = (x1 + x2) / 2.0;
	y = (y1 + y2) / 2.0;
	//ss += s;
	frame++;
	d = sqrt(20000.0 / s) * 1500;
	printf("d = %8.2lf\n", d);
	imshow("im", im);
	imshow("mask", mask);
}

void cvHand(){
	//hand = im.clone();
	medianBlur(im, hand, 21);
	cvtColor(hand, mask_h, CV_BGR2HSV);
	for (int i = 0; i < h; i++){
		for (int j = 0; j < w; j+=2){
			int b = mask_h.step*i + (j * 3);
			if (((mask_h.data[b] >= 0 && mask_h.data[b] <= 30)) && mask_h.data[b + 1] >= 50 && mask_h.data[b + 2] >= 50) //HSVでの検出
			{
				mask_h2.data[b] = 255;
				b = mask_h.step*i + ( (j+1) * 3);
				mask_h2.data[b] = 255;
			}

		}
	}
	//imshow("mask", mask_h2);
	medianBlur(mask_h2, mask_h2, 21);
}

void cvImage(){
	hand = im.clone();
	for (int i = 0; i < h; i++){
		for (int j = 0; j < w; j++){
			int a = im.step*i + (j * 3);
			if (glRead.data[a] != glRead.data[0]){
				for (int k = 0; k < 3; k++){
					im.data[a + k] = glRead.data[a + k];
				}
			}
			if (mask_h2.data[a] != 0){
				for (int k = 0; k < 3; k++){
					im.data[a + k] = hand.data[a + k];
				}
			}

		}
	}
	mask_h2 = Scalar(0, 0, 0);
	imshow("test", im);
}

//----------------------------------

void resetview(void)
{
	distance = 5.0;
	twist = 0.0;
	elevation = 0.0;
	azimuth = 0.0;
}
void polarview(void)
{
	glTranslatef(0.0, 0.0, -distance);
	glRotatef(-twist, 0.0, 0.0, 1.0);
	glRotatef(-elevation, 1.0, 0.0, 0.0);
	glRotatef(-azimuth, 0.0, 1.0, 0.0);
}

void glInit(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitWindowSize(640, 480);
	glutCreateWindow("端っこに隠しておいて");
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glClearColor(0.0, 0.0, 0.0, 1.0);


	resetview();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspect, 1.0, 200.0);
	glMatrixMode(GL_MODELVIEW);
	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glLightInit();
}

void glLightInit(){
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpe);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	lightpos[3] = -50;
	glLightfv(GL_LIGHT1, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiff);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpe);

	lightpos[3] = 50;
	glLightfv(GL_LIGHT2, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiff);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpe);
}

void glFunc(){
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -20.0);
	polarview();
	x /= double(w) / 2.0;
	y /= double(h) / 2.0;
	x--;
	y--;
	x *= AA*aspect;
	y *= -AA;
	//printf("x=%3.2lf,y=%3.2lf\n", x, y);
	glTranslatef(x,y-(AA-2.0) , 0.0);
	//glTranslatef(x, y, 0.0);
	glScalef(1.0, 0.8, 1.0);
	mqoCallModel(model);
	glPopMatrix();
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE, glRead.data);
	flip(glRead, glRead, 0);
	//glClear(GL_COLOR_BUFFER_BIT);

	//imshow("glRead", glRead);

	glFlush();
}

void setupView_()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, w, h, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void restoreView_()
{
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//----------------------------------

void modelInit(){
	mqoInit();
	model = mqoCreateModel("doll.mqo",0.07);
}
