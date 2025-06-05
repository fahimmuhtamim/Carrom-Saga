#include"iGraphics.h"
#include<stdlib.h>
#include<float.h>


#define pi 3.1415926536

int menu_state=0;
int player=1;
int k;
int score1=0,score2=0;
int selected=0;
int piecesOnBoard=19;
bool soundOn=false;
int circle_x=220;

typedef struct {
	double x;
	double y;
}Coordinate;

typedef struct{
	double d[2];
	double v[2];
	double m;
	double miu=0.17;
	double r;
	int state=1;
	int c=0;
	bool p;
}piece;

Coordinate r1[2];

const double pocket4[4][4]={{186,208,70,92},{507,527,70,92},{507,527,391,411},{186,208,391,411}};

const double ARROW[2][7] = {{50, 50, 70, 50, 50, 0, 0}, {4, 12, 0, -12, -4, -4, 4}};

piece pieces[20];

Coordinate b_coordinate[4];

const double min_v=0.1;
const double minD=25;
double Pmax=120;
double powerX=0,powerY=0;
bool sound=false;
char score[2][40];

char b[6][30]={"options\\11.jpg","options\\12.jpg","options\\13.jpg","options\\howto.bmp", "options\\14.jpg","options\\15.jpg"};
char boards[2][40]={"iGraphics_photos\\square_board1.bmp","iGraphics_photos\\square_board2.bmp"};

const double square_border[4]={186,70,527,411};

double distance(double x1, double y1, double x2, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	double d = sqrt((dx*dx) + (dy*dy));
	return d;
}

void velocity(){
	double vk = 0.12;
	double dx = powerX;
	double dy = powerY;
	double d = sqrt(dx*dx + dy*dy);
	if (d > Pmax) {
		dx = ((dx * Pmax) / (d));
		dy = ((dy * Pmax) / (d));
	}
	pieces[19].v[0] = dx * vk;
	pieces[19].v[1] = dy * vk;
}

void friction(piece* p, int i)
{
	double vx = p->v[0];
	double vy = p->v[1];
	double v = sqrt((vx*vx) + (vy*vy));
	double ratio[2];
	ratio[0] = abs(vx/v);
	ratio[1] = abs(vy/v);

	if (p->v[i]<0) p->v[i]+=ratio[i]*p->miu;
	else if (p->v[i] > 0) p->v[i]-=ratio[i]*p->miu;
}

void moveObj(){
	for (int i = 0; i <= 19; i++){
		for (int j = 0; j < 2; j++){
			if (abs(pieces[i].v[j])>=min_v){
				pieces[i].d[j]+=pieces[i].v[j];
				friction(&pieces[i], j);
			}
		}
	}
}

void setpieces(){
	pieces[0].d[0]=(716/2);
	pieces[0].d[1]=(476/2);
	for(int i=1; i<=3; i++) {
		pieces[i].d[0]=(pieces[0].d[0]+20*cos((pi/180)*(120*(i-1))));
		pieces[i].d[1]=(pieces[0].d[1]+20*sin((pi/180)*(120*(i-1))));
	}
	for(int i=4; i<=9; i++) {
		pieces[i].d[0]=(pieces[0].d[0]+40*cos((pi/180)*(60*(i-1))));
		pieces[i].d[1]=(pieces[0].d[1]+40*sin((pi/180)*(60*(i-4))));
	}
	for(int i=10; i<=12; i++) {
		pieces[i].d[0]=(pieces[0].d[0]+20*cos((pi/180)*(60+120*(i-10))));
		pieces[i].d[1]=(pieces[0].d[1]+20*sin((pi/180)*(60+120*(i-10))));
	}
	for(int i=13; i<=18; i++) {
		pieces[i].d[0]=(pieces[0].d[0]+(20*sqrt(3))*cos((pi/180)*(30+60*(i-13))));
		pieces[i].d[1]=(pieces[0].d[1]+(20*sqrt(3))*sin((pi/180)*(30+60*(i-13))));
	}
	pieces[19].d[0]=358;
	pieces[19].d[1]=107;
	pieces[19].r=14;
	pieces[19].m = 36;
	pieces[19].miu = 0.18; 
	for (int i = 0; i <= 18; i++)
	{
		pieces[i].r = 10;
		pieces[i].m = 30;
		pieces[i].miu = 0.17; 
	}
	for (int i = 0; i <= 19; i++)
	{
		pieces[i].v[0] = 0;
		pieces[i].v[1] = 0;
	}
}

int select_piece(int x, int y){
	for (int i = 0; i <= 19; i++)
	{
		double d=distance(x, y, pieces[i].d[0], pieces[i].d[1]);
		if (d<=pieces[i].r) return i;
	}
	return -1;
}

void border_collision(){
	if (menu_state==1){
		for (int i = 0; i <= 19; i++){
			double p[2];
			p[0] = pieces[i].d[0];
			p[1] = pieces[i].d[1];
			double r = pieces[i].r;			
			if (p[0] - r < square_border[0]){
				pieces[i].d[0] = square_border[0] + r;
				pieces[i].v[0] *= -1;
			}
			else if (p[0] + r > square_border[2]){
				pieces[i].d[0] = square_border[2] - r;
				pieces[i].v[0] *= -1;
			}
			if (p[1] - r < square_border[1]){
				pieces[i].d[1] = square_border[1] + r;
				pieces[i].v[1] *= -1;
			}
			else if (p[1] + r > square_border[3]){
				pieces[i].d[1] = square_border[3] - r;
				pieces[i].v[1] *= -1;
			}
		}
	}
}

void after_collision_position(piece* obj1, piece* obj2, double dd){
	piece p1 = *obj1;
	piece p2 = *obj2;

	double D = p1.r+p2.r;
	double dc = D-dd;
	double dx = p2.d[0] - p1.d[0];
	double dy = p2.d[1] - p1.d[1];

	dx = ((dx * dc) / (2 * dd)) +0.1;
	dy = ((dy * dc) / (2 * dd)) +0.1;

	(*obj1).d[0] -= dx;
	(*obj1).d[1] -= dy;
	(*obj2).d[0] += dx;
	(*obj2).d[1] += dy;
}

void after_collision_velocity(piece* obj1, piece* obj2){
	piece p1 = *obj1;
	piece p2 = *obj2;
	double vx1 = p1.v[0];
	double vy1 = p1.v[1];
	double vx2 = p2.v[0];
	double vy2 = p2.v[1];
	double m1 = p1.m;
	double m2 = p2.m;
	double dx = p2.d[0] - p1.d[0];
	double dy = p2.d[1] - p1.d[1];
	double vk1 = (vx1*dx+vy1*dy)/(dx*dx+dy*dy);
	double vk2 = (vx2*dx+vy2*dy)/(dx*dx+dy*dy);
	double v1[2];
	double v2[2];
	v1[0] = dx*vk1;
	v1[1] = dy*vk1;
	v2[0] = dx*vk2;
	v2[1] = dy*vk2;
	for (int i = 0; i < 2; i++){
		(*obj1).v[i]-=v1[i];
		(*obj2).v[i]-=v2[i];
		(*obj1).v[i]+=((m1-m2)*v1[i] + 2*m2*v2[i]) / (m1+m2);
		(*obj2).v[i]+=((m2-m1)*v2[i] + 2*m1*v1[i]) / (m1+m2);
	}
}

void pieceCollision()
{
	for (int i = 0; i <= 19; i++)
	{
		for (int j = i+1; j <= 19; j++)
		{
			int d = distance(pieces[i].d[0], pieces[i].d[1], pieces[j].d[0], pieces[j].d[1]);
			if (d < pieces[i].r + pieces[j].r && pieces[i].state==1 && pieces[j].state==1){
				sound=true;
				after_collision_position(&pieces[i], &pieces[j], d);
				after_collision_velocity(&pieces[i], &pieces[j]);
				if (soundOn && sound){
					PlaySound(L"iGraphics_photos\\collision.wav", NULL, SND_ASYNC);
					sound==false;
				}
				
			}
		}
	}
}

void changeplayer(){
	for(int i=0;i<=19;i++){
		if (pieces[i].state==0) continue;
		pieces[i].d[0]=726-pieces[i].d[0];
		pieces[i].d[1]=486-pieces[i].d[1];
	}
	player = (player == 1) ? 2 : 1;
}

bool isInPocketSpace(double* p)
{
	if (p[0]>=pocket4[0][0] && p[0]<=pocket4[0][1] && p[1]>=pocket4[0][2] && p[1]<=pocket4[0][3]) return true;
	else if (p[0]>=pocket4[1][0] && p[0]<=pocket4[1][1] && p[1]>=pocket4[1][2] && p[1]<=pocket4[1][3]) return true;
	else if (p[0]>=pocket4[2][0] && p[0]<=pocket4[2][1] && p[1]>=pocket4[2][2] && p[1]<=pocket4[2][3]) return true;
	else if (p[0]>=pocket4[3][0] && p[0]<=pocket4[3][1] && p[1]>=pocket4[3][2] && p[1]<=pocket4[3][3]) return true;
	else return false;
}

void isInPocket(){
	for(int i=0; i<=18; i++) {
		if  (pieces[i].state==0) continue;
		double v=sqrt(pieces[i].v[0]*pieces[i].v[0]+pieces[i].v[1]*pieces[i].v[1]);
		if (isInPocketSpace(pieces[i].d) && v<10) {
			pieces[i].d[0]=0;
			pieces[i].d[1]=0;
			pieces[i].v[0]=0;
			pieces[i].v[1]=0;
			pieces[i].state=0; 
			piecesOnBoard--;
			if (player==1){
				switch((i+8)/9) {
					case 0:
					score1+=20;
					break;
					case 1:
					score1+=10;
					break;
					case 2:
					score1+=5;
					break;
				}
			}
			if (player==2){
				switch((i+8)/9) {
					case 0:
					score2+=20;
					break;
					case 1:
					score2+=10;
					break;
					case 2:
					score2+=5;
					break;
				}
			}
		}
	}
}

void drawArrow(double* p){
	double dx = powerX;
	double dy = powerY;
	double d = sqrt(dx*dx + dy*dy);
	dx=dx/d;
	dy=dy/d;
	if (selected != 19 || d <= min_v) return;
	double newArrow[2][7];
	double x, y;
	double scale=((d/Pmax)*0.75)+0.25;
	if (scale > 1.2) scale = 1.2;
	
	for (int i = 0; i < 7; i++){
		x = ARROW[0][i];
		y = ARROW[1][i];
		newArrow[0][i] = (x*dx) - (y*dy);
		newArrow[1][i] = (x*dy) + (y*dx);
		newArrow[0][i] *= scale;
		newArrow[1][i] *= scale;
		newArrow[0][i] += p[0];
		newArrow[1][i] += p[1];
	}
	if (d<70) iSetColor(2, 201, 178);
	else if (d<120) iSetColor(2, 191, 15);
	else if (d<150) iSetColor(157, 240, 2);
	else if (d<170) iSetColor(240, 137, 2);
	else iSetColor(224, 10, 2);
	iFilledPolygon(newArrow[0], newArrow[1], 7);
}

void play2player(){

	if (pieces[0].state==1)iShowBMP2(pieces[0].d[0]-10,pieces[0].d[1]-10,"iGraphics_photos\\rsz_cropped_2.bmp",0xFFFFFF);
	for(int i=1;i<=9; i++) {
		if (pieces[i].state==1)iShowBMP2(pieces[i].d[0]-10,pieces[i].d[1]-10,"iGraphics_photos\\rsz_cropped_1.bmp",0xFFFFFF);
	}
	for(int i=10;i<19; i++) {
		if (pieces[i].state==1)iShowBMP2(pieces[i].d[0]-10,pieces[i].d[1]-10,"iGraphics_photos\\rsz_cropped.bmp",0xFFFFFF);
	}
	iSetColor(158, 2, 2);
	iFilledCircle(pieces[19].d[0],pieces[19].d[1],14);
	iSetColor(0,0,0);
	if (player==1)iText(150,460,"Player 1 plays",  GLUT_BITMAP_HELVETICA_18);
	if (player==2)iText(150,460,"Player 2 plays",  GLUT_BITMAP_HELVETICA_18);
	sprintf(score[0],"Player 1 score: %d",score1);
	sprintf(score[1],"Player 2 score: %d",score2);
	iText(150,6,score[0],GLUT_BITMAP_9_BY_15);
	iText(420,6,score[1],GLUT_BITMAP_9_BY_15);
}

void start_game(){
	setpieces();
	//iResumeTimer(0);
	//play2player();
}

void iDraw() {
	 
	
	iClear();
	if (menu_state == 0) {
		//Homemenu
		iShowBMP(0, 0, "iGraphics_photos\\carrom_board_arrow.bmp"); 
		for(int i=0; i<5; i++) {
			if (i==3) iShowBMP2(b_coordinate[i].x,b_coordinate[i].y, b[i],0x2B2B2B);
			else iShowBMP2(b_coordinate[i].x,b_coordinate[i].y, b[i],0);
		}
		iSetColor(43,43,43);
		iText(185,440,"WELCOME TO CARROM SAGA!",GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else if (menu_state==1){
		if (player==1){
			iShowBMP(120,0,boards[0]);
		}
		else iShowBMP(120,0,boards[1]);
		iShowBMP2(40,400, b[5],0);
		drawArrow(pieces[19].d);
		play2player();
		if (piecesOnBoard==0) menu_state=6;
		
	}
	
	else if (menu_state==2){
		
		iShowBMP(0, 0, "iGraphics_photos\\carrom_board_arrow.bmp"); 
		iShowBMP2(40,400, b[5],0);
		iSetColor(0,0,0);
		
		iText(45,250,"Friction", GLUT_BITMAP_HELVETICA_18);
		iSetColor(30,30,30);
		iFilledRectangle(120,254,200,2);
		iSetColor(255, 22, 5);
		iFilledCircle(circle_x,255,8);
		iSetColor(0,0,0);
		if(soundOn) {
			iShowBMP2(215,290,"options\\soundoff.bmp",0X2B2B2B);
			iText(45,310, "Collision Sound Off:", GLUT_BITMAP_HELVETICA_18);
		}
		else {
			iShowBMP2(215,290,"options\\soundon.bmp",0X2B2B2B);
			iText(45,310, "Collision Sound On:", GLUT_BITMAP_HELVETICA_18);
		}
	}
	else if (menu_state==3){
		iShowBMP(0,0,"iGraphics_photos\\credits.bmp");
		iShowBMP2(25,415, b[5],0);
	}
	else if (menu_state==4){
		iShowBMP(0,0,"iGraphics_photos\\how_to_play.jpg");
		iShowBMP2(25,415, b[5],0);
	}
	else if (menu_state==5){
		exit(0);
	}
	else if (menu_state==6) {
		iShowBMP2(40,400,b[4],0);
		if (score1>score2) {
			iShowBMP(0,97,"iGraphics_photos\\player1wins.bmp");
		}
		else iShowBMP(0,97,"iGraphics_photos\\player2wins.bmp");
	}

}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/


/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
	*/
void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (menu_state==0){
			score1=0;
			score2=0;
			if (mx>=50 && mx<=100 && my>=320 && my <= 370) {
				for(int i=0; i<=18; i++) pieces[i].state=1;
				piecesOnBoard=19;
				menu_state=1;
				start_game();
				
			}
			else if (mx>58 && mx<93 && my>248 && my < 286){
				menu_state=2;
			}
			else if (mx>58 && mx<93 && my>189 && my < 228){
				menu_state=3;
			}
			else if (mx>58 && mx<93 && my>137 && my < 168){
				menu_state=4;
			}
			else if (mx>58 && mx < 93 && my >68 && my < 108) {
				menu_state=5;
			}
			
		}
		else if (menu_state==1 ) {
			if (mx>=42 && mx<=85 && my>=402 && my<=450) menu_state=0;
			selected = select_piece(mx, my);
			if (my<=121 && my>=93) {
				if (mx>247 && mx<468)pieces[19].d[0]=mx;
				else if (mx<=247)pieces[19].d[0]=247;
				else pieces[19].d[0]=468;
				if (k==piecesOnBoard && pieces[19].d[1]!=107) changeplayer();
				pieces[19].d[1]=107;
				
			}

		}
		else if (menu_state==2) {
			if (mx>=42 && mx<=85 && my>=402 && my<=450) menu_state=0;
			if (mx>=215 && mx<=255 && my>=290 && my<=330 ){
				switch(soundOn){
					case true: 
					soundOn=false;
					break;
					case false:
					soundOn=true;
					break;
				}
			}
			if (mx>=120 && mx<=320 && my >=250 && my<=260){
				circle_x=mx;
				for(int i=0; i<19; i++){
					pieces[i].miu=0.14+(0.6*(mx-120))/200;
				}
			}
			
		}
		else if (menu_state==3){
			if (mx>=25 && mx<=70 && my>=415 && my<=465) menu_state=0;
		}
		else if (menu_state==4){
			if (mx>=25 && mx<=70 && my>=415 && my<=465) menu_state=0;
		}
		else if (menu_state==6) {
			if (mx>=42 && mx<=85 && my>=402 && my<=450) menu_state=0;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		if(menu_state==1){
			
			double d = sqrt(powerX*powerX + powerY*powerY);
			if (selected==19 && d > minD && menu_state==1 && my<400)
			{
				powerX -= minD * (powerX / d);
				powerY -= minD * (powerY / d);
				velocity();
			}
			powerX = 0;
			powerY = 0;
			k=piecesOnBoard;
			
		}
		else if (menu_state==0) {
			b_coordinate[4].x=50;
			b_coordinate[4].y=60;
			
		}
	}
}
void iMouseMove(int mx, int my) {
	//printf("x = %d, y= %d\n",mx,my);
	if (menu_state==1 && my< 300){
		
		powerX = pieces[19].d[0] - mx;
		powerY = pieces[19].d[1] - my;
	}
}

void iKeyboard(unsigned char key)
{

}

void iSpecialKeyboard(unsigned char key)
{

}

void screen(){
	moveObj();
	border_collision();
	pieceCollision();
	isInPocket();
}

int main() {
	for(int i=0; i<6; i++) {
		b_coordinate[i].x=50;
		if (i==0) b_coordinate[i].x-=5;
		b_coordinate[i].y=60*(5-i);
	}
	iSetTimer(15,screen);
	iInitialize(716, 476, "Carrom Saga");
	return 0;
}
