#include <stdio.h>
#include <easyx.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>//多媒体设备接口MCI //放在<graphics.h>下方
#pragma comment(lib, "winmm.lib")//加载静态库

enum SomeValues {//窗口大小
	WIDTH = 960,
	HEIGHT = 540,
	HOMURA_WIDTH = 120,
	HOMURA_HEIGHT = 66,
	WRPRGS_WIDTH = 200,
	WRPRGS_HEIGHT = 200,
	BULLET_NUM = 2//子弹数组长度。
		//由于现行代码会使数组里的所有子弹一起打出，故让子弹速度为5（很快），数组长度只留2（J和K）
};

struct Object
{
	int x;
	int y;
	bool alive;//是否存活
	int hp;
	short jk;//子弹类型 j1 k2
}homura, bullet[BULLET_NUM], wrprgs;

//图片
IMAGE bk;
IMAGE homura_img;
IMAGE bullet_img;
IMAGE wrprgs_img;
IMAGE victory;

//函数集中声明
void welcome();
void initGame();
void gameDraw();
void homuraMove(int speed);
void createBulletJ();
void createBulletK();
void bulletMoveJ();
void bulletMoveK();
void wrprgsMove(long long i);
void bulletHit();
void printHp(long long i);
void theEnd();

int main() {
	srand((unsigned)time(nullptr));
	initgraph(WIDTH, HEIGHT, EX_SHOWCONSOLE);
	welcome();
	getchar();
	getchar();
	initGame();
	mciSendString(_T("play ./瓦夜bgm.mp3 repeat"), 0, 0, 0);

	BeginBatchDraw();	//双缓冲绘图
	//main
	while (true) {
		long long i = 0;//防止瓦夜瞬间逼近晓美焰；防止控制台一直输出瓦夜血量
		gameDraw();
		FlushBatchDraw();

		homuraMove(8);
		bulletMoveJ();
		bulletMoveK();
		wrprgsMove(i);
		bulletHit();
		printHp(i);

		if (wrprgs.hp == 0) break;

		++i;
		if (i == 999999999999999999) i = 0;//防止瓦夜瞬间逼近晓美焰的遗留代码
	}
	EndBatchDraw();

	theEnd();

	getchar();
	return 0;
}

void welcome() {
	printf("  欢迎来玩《晓美焰大战魔女之夜》！\n\tv Alpha.1  2025.12.6.\n\tProudly Presented by Chipppress\n");
	printf("-----------------------------------------\n");
	printf("目标：控制晓美焰，击败魔女之夜\n");
	printf("1. 游戏包括 游戏画面窗口 和 控制台窗口\n");
	printf("2. 游戏时需要一直在控制台窗口键入，请确保输入法为英文，\n  或者直接大写锁定，像小时候玩4399那样\n");
	printf("  建议游戏前把控制台窗口调小，置于屏幕右下角\n");
	printf("3. WASD移动，J和K发射子弹\n");
	printf("4. 有BGM，请戴耳机（以免吓到室友）\n");
	printf("-----------------------------------------\n");
	printf("按两下Enter开始游戏！\n");
}

void initGame() {
	//homura
	homura.x = WIDTH / 7;
	homura.y = HEIGHT / 2;
	homura.alive = true;
	homura.hp = 100;

	//bullet
	for (short i = 0; i < BULLET_NUM; ++i) {
		bullet[i].alive = false;
		bullet[i].x = bullet[i].y = 0;
	}

	//瓦夜warupurugisu
	wrprgs.x = WIDTH - WIDTH / 4 - WRPRGS_WIDTH;
	wrprgs.y = HEIGHT / 2 - WRPRGS_HEIGHT;
	wrprgs.alive = true;
	wrprgs.hp = 1500;

	//加载图片
	loadimage(&bk, _T("./背景.jpg"));
	loadimage(&homura_img, _T("./晓美焰举枪.jpg"));
	//loadimage(&homura[1], _T("./晓美焰举枪伪掩码图.jpg"));
	loadimage(&wrprgs_img, _T("./瓦夜.jpg"));
	loadimage(&bullet_img, _T("./子弹.jpg"));
	loadimage(&victory, _T("./victory.jpg"));
}

void gameDraw() {
	putimage(0, 0, &bk);
	putimage(homura.x, homura.y, &homura_img /* ,NOTSRCERASE */);
		//putimage(50, 50, &homura[1], SRCINVERT);
	putimage(wrprgs.x, wrprgs.y, &wrprgs_img);

	//bullet
	for (short i = 0; i < BULLET_NUM; ++i) {
		if (bullet[i].alive) putimage(bullet[i].x, bullet[i].y, &bullet_img);
	}
}

void homuraMove(int speed) {
	if (_kbhit()) {
		char key = _getch();
		switch (key) {
		case 'w':
		case 'W':
			if (homura.y>0) homura.y -= speed;
			break;
		case 'a':
		case 'A':
			if (homura.x > 0) homura.x -= speed;
			break;
		case 's':
		case 'S':
			if (homura.y + HOMURA_HEIGHT < 540) homura.y += speed;
			break;
		case 'd':
		case 'D':
			if (homura.x + HOMURA_WIDTH < 960) homura.x += speed;
			break;

		case 'j':
		case 'J':
			createBulletJ();
			break;
		case 'k':
		case 'K':
			createBulletK();
			break;

		default: break;
		}
	}
}

void createBulletJ() {
	for (short i = 0; i < BULLET_NUM; ++i) {//循环语句先不删，免得忘了最初的设想是有一组子弹
		if (!bullet[0].alive) {
			bullet[0].alive = true;
			bullet[0].jk = 1;
			bullet[0].x = homura.x + HOMURA_WIDTH;
			bullet[0].y = homura.y;
		}
	}
}

void createBulletK() {
	for (short i = 0; i < BULLET_NUM; ++i) {
		if (!bullet[1].alive) {
			bullet[1].alive = true;
			bullet[1].jk = 2;
			bullet[1].x = homura.x;
			bullet[1].y = homura.y;
		}
	}
}

void bulletMoveJ() {
	if (bullet[0].alive && bullet[0].jk == 1) {
		bullet[0].x += 5;
		bullet[0].y -= 1;
	}//子弹速度在这调
	if (bullet[0].x > WIDTH) bullet[0].alive = false;
}

void bulletMoveK() {
	if (bullet[1].alive && bullet[1].jk == 2) {
		bullet[1].x -= 5;
		bullet[1].y -= 1;
	}//子弹速度在这调
	if (bullet[1].x < 0) bullet[1].alive = false;
		//现行代码需要子弹击中瓦夜或者飞出边界才会alive=false，而J和K两种子弹飞出边界分别在右、左
}

void wrprgsMove(long long i) {
	if (i % 99999999999 == 0) {
	//远离边界//改用传送
	if (wrprgs.x <= WRPRGS_WIDTH || wrprgs.x >= WIDTH - WRPRGS_WIDTH / 2) {
		wrprgs.x = (WIDTH - WRPRGS_WIDTH - 20) / (rand() % 4 + 1);
		wrprgs.y = HEIGHT/(rand()%4+1);
	}
	if (wrprgs.y <= -WRPRGS_HEIGHT / 2 || wrprgs.y >= HEIGHT - WRPRGS_HEIGHT / 2) {
		wrprgs.x = (WIDTH - WRPRGS_WIDTH - 20) / (rand() % 4 + 1);
		wrprgs.y = HEIGHT/ (rand() % 4 + 1);
	}

	short x = rand() % 100;
	if (x == 0 || x==8) {//慢逼近
		if (wrprgs.x > homura.x) wrprgs.x -= 1;
		else wrprgs.x += 1;
		if (wrprgs.y > homura.y) wrprgs.y -= 1;
		else wrprgs.y += 1;
	}
	if (x == 1) {//快逼近
		if (wrprgs.x > homura.x) wrprgs.x -= 2;
		else wrprgs.x += 2;
		if (wrprgs.y > homura.y) wrprgs.y -= 2;
		else wrprgs.y += 2;
	}
	if (x == 3 && i % 10000 == 0) {//远离
		if (wrprgs.x < homura.x) wrprgs.x -= 3;
		else wrprgs.x += 3;
		if (wrprgs.y < homura.y) wrprgs.y -= 3;
		else wrprgs.y += 3;
	}
	if (x == 4) wrprgs.x += 2;
	if (x == 5) wrprgs.x -= 2;
	if (x == 6) wrprgs.y -= 2;
	if (x == 7) wrprgs.y += 2;

	}
	
}

void bulletHit() {
	for (int i = 0; i < BULLET_NUM; ++i) {
		bool xx = bullet[i].x >= wrprgs.x && bullet[i].x <= wrprgs.x + WRPRGS_WIDTH;
		bool yy = bullet[i].y >= wrprgs.y && bullet[i].y <= wrprgs.y + WRPRGS_HEIGHT;
		if (xx && yy && bullet[i].alive == true) {
			bullet[i].alive = false;
			wrprgs.hp -= 1;
		}
	}
}

void printHp(long long i) {
	int ran = rand() % 50; //防止控制台一直输出瓦夜血量
	if (wrprgs.hp % 50 == 0 && i%999999999999==0 && ran==0) printf("瓦夜还剩%i点血！\n", wrprgs.hp);
}

void theEnd() {
	putimage(0, 0, &victory);
	settextcolor(RED);
	settextstyle(90, 0, _T("黑体"));
	setbkmode(TRANSPARENT);
	outtextxy(WIDTH/2, HEIGHT/2, _T("VICTORY!"));

	printf("游戏结束！你击败了魔女之夜！！\n");
	printf("感谢试玩，欢迎反馈bug。更多功能敬请期待！\n");
}

//2025.12.5.
//2025.12.6.