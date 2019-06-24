
#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<iostream>
#include<tchar.h>
#include<fstream>
#include<random>
#include<time.h>

using namespace std;

//用于读取字体点阵的FILE变量
FILE* fp;

//用于显示窗体的全局变量
//----------------------------------
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int bits = 24;
extern BYTE buffer[SCREEN_WIDTH * SCREEN_HEIGHT * bits / 8];

class Color
{
public:
	Color(int R, int G, int B) :r(R), g(G), b(B) {};
	Color() :r(255), g(255), b(255) {};
	int r, g, b;
};

struct LandShow
{
	int x, y;
};

struct BridgeShow
{
	int indexA, indexB;
	int num;
	float len;
};

int Step = 0; //动态模拟步长

const int landSum = 4;

LandShow landShow[landSum] =
{ {100,150},
{500,200},
{200,350},
{600,400} };

clock_t start, finish;

struct bridge
{
	int A, B;
	float len;
	int num;
};

int LandSum = 0;
int BridgeSum = 0;
bridge *Bridge;

bool findAnswerFlag = false;

int LoadSum;

int *BestLoad;
float BestLen = INT_MAX;


float temperature = 100 * LoadSum;
int iter = 100; //循环迭代次数
int IterationTimes = 0;   // 总共迭代次数
int *Load;

float Len[100000];


float CalLoad(int* load, int n);
void showLoad(int* load, int n);

void  InitBridge()
{
	ifstream infile;
	infile.open("data.txt", ios::in);

	infile >> LandSum >> BridgeSum;
	
	srand(time(0));

	Bridge = new bridge[BridgeSum];

	for (int i = 0; i < BridgeSum; i++)
	{
		infile >> Bridge[i].A >> Bridge[i].B >> Bridge[i].len;
		int num = -1;
		for (int j = 0; j <= i; j++)
		{
			
			if (Bridge[j].A == Bridge[i].A && Bridge[j].B == Bridge[i].B)
			{
				num++;
			}
			if (Bridge[j].A == Bridge[i].B && Bridge[j].B == Bridge[i].A)
			{
				num++;
			}

		}
		Bridge[i].num = num;
	}
	/*	
	cout << "LandSum " << LandSum << " BridgeSum " << BridgeSum << endl;
	for (int i = 0; i < BridgeSum; i++)
	{
		cout << "Bridge "  << Bridge[i].A << " " << Bridge[i].B  << endl;

	}
	*/

	cout << endl;
	infile.close();
}


void InitLoad(int *Load,int n,int reLoadA=0,int reLoadB=0,int reLoadC=0)
{
	srand(time(0));
	int specialLoadA = reLoadA;
	int specialLoadB = reLoadB;
	int specialLoadC = reLoadC;

	int* ChoiceBridgeList = new int[n]; 


	for (int i = 0; i < min(BridgeSum,n); i++)
	{
		ChoiceBridgeList[i] = i;
	}
	
	if (n - BridgeSum == 1)
	{
		ChoiceBridgeList[n - 1] = specialLoadA;
	}
	if (n - BridgeSum == 2)
	{
		ChoiceBridgeList[n - 2] = specialLoadA;
		ChoiceBridgeList[n - 1] = specialLoadB;
	}
	if (n - BridgeSum == 3)
	{
		ChoiceBridgeList[n - 3] = specialLoadA;
		ChoiceBridgeList[n - 2] = specialLoadB;
		ChoiceBridgeList[n - 1] = specialLoadC;

	}

	bool* ChoiceBridgeFlag = new bool[n];

	for (int i = 0; i < n; i++)
	{
		ChoiceBridgeFlag[i] = false;
	}
	for (int i = 0; i < n; i++)
	{
		int index = rand() % n;
		while (ChoiceBridgeFlag[index])
		{
			index = rand() % n;
		}
		Load[i] = ChoiceBridgeList[index];
		ChoiceBridgeFlag[index] = true;
	}
}

float CalLoadLen(int* load, int n)
{
	float len = 0;
	for (int i = 0; i < n; i++)
	{
		len += Bridge[load[i]].len;
	}
	return len;
}


float random()
{
	float a = rand() % 1000;
	return a / 1000.0;
}

void LoadCopy(int* LoadA, int* LoadB, int n)
{
	for (int i = 0; i < n; i++)
	{
		LoadA[i] = LoadB[i];
	}
}

int* RerandomLoad(int* load, int n)
{
	int* TempLoad = new int[n];
	int mark = CalLoad(load, n);

	for (int i = 0; i < n; i++)
	{
		TempLoad[i] = load[i];
	}

	int Bridge1 = rand() % n;
	int Bridge2 = rand() % n;
	
	while (Bridge1 == Bridge2)
	{
		Bridge1 = rand() % n;
		Bridge2 = rand() % n;
	}
	int tmp = TempLoad[Bridge1];
	TempLoad[Bridge1] = TempLoad[Bridge2];
	TempLoad[Bridge2] = tmp;
	return TempLoad;
}

float CalLoad(int* load, int n)
{
	int mark = 0;
	int start = Bridge[load[0]].A;
	int now = Bridge[load[0]].B;
	for (int i = 1; i < n; i++)
	{
		if (now == Bridge[load[i]].A)
		{
			mark++;
			now = Bridge[load[i]].B;
		}
		else if (now == Bridge[load[i]].B)
		{
			mark++;
			now = Bridge[load[i]].A;
		}
		else
		{
			
			now = Bridge[load[i]].A;
		}
	}
	if (start== now)
	{
		mark++;
	}

	return mark;
}

void showLoad(int* load, int n)
{
	for (int i = 0; i < n; i++)
	{
		cout << load[i] << " ";
	}
	cout << endl;
}

bool isEulerPath(int* load, int n)
{
	int BasePointSum = 0;
	for (int i = 0; i < LandSum; i++)
	{
		int degree = 0;
		for (int j = 0; j < n; j++)
		{
			if (Bridge[load[j]].A == i || Bridge[load[j]].B == i)
			{
				degree++;
			}
		}
		if (degree % 2 == 1)
		{
			BasePointSum++;
		}
	}
	if (BasePointSum == 0 || BasePointSum == 2)
	{
		return true;
	}
	return false;
}



void TuiHuo()
{
	temperature = 100 * LoadSum;
	IterationTimes = 0;
	

	if (!isEulerPath(Load, LoadSum))
	{
		return;
	}

	Len[0] = CalLoad(Load, LoadSum);
	 
	while (temperature > 0.001)
	{
		for (int i = 0; i < iter; i++)
		{
			float len1 = CalLoad(Load, LoadSum);
			int* Tmp_Load = RerandomLoad(Load, LoadSum);
			float len2 = CalLoad(Tmp_Load, LoadSum);

			float delta_e = len2 - len1;
			if (delta_e < 0)
			{
				LoadCopy(Load, Tmp_Load, LoadSum);
			}
			else
			{
				if (exp(-delta_e / temperature) > random())
				{
					LoadCopy(Load, Tmp_Load, LoadSum);
				}
			}
			delete[] Tmp_Load;
		}
		IterationTimes++;
		Len[IterationTimes] = CalLoad(Load, LoadSum);
		if (Len[IterationTimes] == LoadSum)
		{
			cout << "match solution ";
			showLoad(Load, LoadSum);
			float len = CalLoadLen(Load, LoadSum);
			if (len < BestLen)
			{
				BestLen = len;
				if(!BestLoad)
				{
					BestLoad = new int[LoadSum];
				}
				LoadCopy(BestLoad, Load, LoadSum);
				findAnswerFlag = true;

			}
		}
		temperature *= 0.99;
	}
}


void GameLoop();

void CleanScreen();

void DrawPoint(int x, int y, Color color = Color(255, 255, 255));

void PutBufferToScreen();

//=====================================================================
// 基础的绘制函数
void DrawString(int startX, int startY, const char* word);
void DrawWord(int startX, int startY, char CharToDraw, Color color = Color(255, 255, 255));

void DrawPoint(int x, int y, const Color color)
{
	if (x <= 0 || x >= SCREEN_WIDTH)return;
	if (y <= 0 || y >= SCREEN_HEIGHT)return;

	buffer[int(y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 1] = color.r;
	buffer[int(y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 2] = color.g;
	buffer[int(y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 3] = color.b;
}

void DrawLine(int x1, int y1, int x2, int y2, Color color = Color(255, 255, 255))
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int ux = ((dx > 0) << 1) - 1;
	int uy = ((dy > 0) << 1) - 1;
	int x = x1, y = y1, eps;

	eps = 0; dx = abs(dx); dy = abs(dy);

	if (dx > dy)
	{
		for (x = x1; x != x2; x += ux)
		{

			DrawPoint(x, y, color);
			eps += dy;
			if ((eps << 1) >= dx)
			{
				y += uy; eps -= dx;
			}
		}

	}
	else
	{
		for (y = y1; y != y2; y += uy)
		{
			DrawPoint(x, y, color);
			eps += dx;
			if ((eps << 1) >= dy)
			{
				x += ux; eps -= dy;
			}
		}
	}
}

void DrawRectangle(int x0, int y0, int x1, int y1, Color color = Color(255, 255, 255))
{
	int minY = min(y0, y1);
	int maxY = max(y0, y1);
	int minX = min(x0, x1);
	int maxX = max(x0, x1);
	for (int y = minY; y < maxY; y++)
	{
		for (int x = minX; x < maxX; x++)
		{
			DrawPoint(x, y, color);
		}
	}
}



void DrawQuadraticCurveLine(float x0, float y0, float x1, float y1, float x2, float y2, Color color = Color(255, 255, 255))
{

	if (x0 == x2 && x0 == x1)
	{
		DrawLine(x0, y0, x2, y2, color);
		return;
	}

	float A;
	float B;
	float C;



	A = ((y1 - y2) * (x0 - x1) - (y0 - y1) * (x1 - x2)) / ((x1 * x1 - x2 * x2) * (x0 - x1) + (x1 * x1 - x0 * x0) * (x1 - x2));
	B = (y0 - y1 + A * (x1 * x1 - x0 * x0)) / (x0 - x1);
	C = y0 - A * x0 * x0 - B * x0;

	float StartX = min(x0, x2);
	float EndX = max(x0, x2);
	float LastY = -1;

	for (float x = StartX; x <= EndX; x++)
	{
		int y = A * x * x + B * x + C;
		if (LastY != -1)
		{
			DrawLine(x - 1, LastY, x, y, color);
		}
		LastY = y;
		LastY = y;
	}
}

void DrawBridge(int x0, int y0, int x2, int y2, int n, char* DrawWord = NULL, Color color = Color(255, 255, 255))
{
	int tempX = (x0 + x2) / 2;
	int tempY = (y0 + y2) / 2;
	float kRaw = (float)(y0 - y2) / (float)(x0 - x2);
	int x1;
	int y1;
	float k;
	float Distance = 30;
	float Dx;

	if (kRaw != 0)
	{
		k = -1 / kRaw;

		Dx = sqrt(Distance * Distance / (k * k + 1));
		x1 = tempX + Dx * n;
		y1 = tempY + k * Dx * n;
	}
	else
	{
		x1 = tempX;
		y1 = tempY + Distance;
	}
	DrawString(x1, y1 + 20, DrawWord);

	DrawQuadraticCurveLine(x0, y0, x1, y1, x2, y2, color);
}



void DrawWord(int startX, int startY, char CharToDraw,Color color)
{
	int START = 0x20;
	int DATANUM = 16;
	int location = (CharToDraw - START) * DATANUM;

	int x = startY;  //由于储存数据方式问题，这里需要颠倒
	int y = startX;

	int i = 0;
	int buf = 0;

	fseek(fp, location, SEEK_SET);

	int DataX, DataY, DrawX, DrawY;
	for (int i = 0; i < 16; i++)
	{

		buf = fgetc(fp);
		//显示一个字节
		for (int j = 0; j < 8; j++)
		{
			if (buf % 2 == 1)
			{

				if (i < 8)
				{
					DataX = x + j;
					DataY = y + i;
				}
				else
				{
					DataX = x + j + 8;
					DataY = y + i - 8;
				}
				DrawX = DataY;
				DrawY = DataX;
				DrawPoint(DrawX, DrawY,color);
			}
			buf = buf / 2;
		}
	}
}

void DrawString(int startX, int startY, const char* word)
{
	if (word == NULL)
	{
		return;
	}
	int len = strlen(word);
	for (int i = 0; i < len; i++)
	{
		DrawWord(startX + i * 8, startY, word[i]);
	}

}


void DrawCircle(int x, int y, int r, Color color = Color(255, 255, 255))
{
	for (int ny = y - r; ny < y + r; ny++)
	{
		for (int nx = x - r; nx < x + r; nx++)
		{
			float d = sqrt((ny - y) * (ny - y) + (nx - x) * (nx - x));
			if (d < r)
			{
				DrawPoint(nx, ny, color);
			}
		}
	}
}

//===================================================================

void CleanScreen()
{
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{

			buffer[int(y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 1] = 139;
			buffer[int(y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 2] = 129;
			buffer[int(y) * SCREEN_WIDTH * 3 + (int(x) + 1) * 3 - 3] = 195;
		}
	}
}

BYTE buffer[SCREEN_WIDTH * SCREEN_HEIGHT * bits / 8];

HDC screen_hdc;
HDC hCompatibleDC;
HBITMAP hCompatibleBitmap;
HBITMAP hOldBitmap;
BITMAPINFO binfo;

HINSTANCE hInstance;
WNDCLASS Draw;
HWND hwnd;
MSG msg;


LRESULT CALLBACK WindowProc(
	_In_	HWND hwnd,
	_In_	UINT uMsg,
	_In_	WPARAM wParam,
	_In_	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			exit(0);
		if (wParam == VK_RETURN)
		{
			Step++;
			if (Step > LoadSum * 2)
			{
				Step = 0;
			}
		}
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void PutBufferToScreen()
{
	SetDIBits(screen_hdc, hCompatibleBitmap, 0, SCREEN_HEIGHT, buffer, (BITMAPINFO*)& binfo, DIB_RGB_COLORS);
	BitBlt(screen_hdc, -1, -1, SCREEN_WIDTH, SCREEN_HEIGHT, hCompatibleDC, 0, 0, SRCCOPY);
}

void initWindow()
{
	hInstance = GetModuleHandle(NULL);

	Draw.cbClsExtra = 0;
	Draw.cbWndExtra = 0;
	Draw.hCursor = LoadCursor(hInstance, IDC_ARROW);
	Draw.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	Draw.lpszMenuName = NULL;
	Draw.style = WS_MINIMIZEBOX | CS_HREDRAW | CS_VREDRAW;
	Draw.hbrBackground = (HBRUSH)COLOR_WINDOW;
	Draw.lpfnWndProc = WindowProc;
	Draw.lpszClassName = "DDraw";
	Draw.hInstance = hInstance;

	RegisterClass(&Draw);

	hwnd = CreateWindow(
		"DDraw",
		"Draw",
		WS_OVERLAPPEDWINDOW,
		38,
		20,
		SCREEN_WIDTH + 15,
		SCREEN_HEIGHT + 38,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	//init bitbuffer
	ZeroMemory(&binfo, sizeof(BITMAPINFO));
	binfo.bmiHeader.biBitCount = bits;
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biHeight = -SCREEN_HEIGHT;
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biSizeImage = 0;
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = SCREEN_WIDTH;

	screen_hdc = GetDC(hwnd);
	hCompatibleDC = CreateCompatibleDC(screen_hdc);
	hCompatibleBitmap = CreateCompatibleBitmap(screen_hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
	hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, hCompatibleBitmap);
}



int main()
{
	InitBridge();
	start = clock();

	Load = new int[BridgeSum];
	InitLoad(Load, BridgeSum);
	LoadSum = BridgeSum;
	TuiHuo();
	delete[] Load;


	if (!findAnswerFlag)
	{
		for (int i = 0; i < BridgeSum; i++)
		{
			Load = new int[BridgeSum + 1];
			InitLoad(Load, BridgeSum + 1, i);
			LoadSum = BridgeSum + 1;
			TuiHuo();
			delete[] Load;
		}

		if (!findAnswerFlag)
		{
			//cout << "No Find in " << LoadSum << " loads" << endl;
			for (int i = 0; i < BridgeSum; i++)
			{
				for (int j = 0; j < BridgeSum; j++)
				{

					Load = new int[BridgeSum + 2];
					InitLoad(Load, BridgeSum + 2, i, j);
					LoadSum = BridgeSum + 2;
					TuiHuo();
					delete[] Load;
				}
			}
			if (!findAnswerFlag)
			{
				//cout << "No Find in " << LoadSum << " loads" << endl;
				for (int i = 0; i < BridgeSum; i++)
				{
					for (int j = 0; j < BridgeSum; j++)
					{
						for (int z = 0; z < BridgeSum; z++)
						{
							Load = new int[BridgeSum + 3];
							InitLoad(Load, BridgeSum + 3, i, j, z);
							LoadSum = BridgeSum + 3;
							TuiHuo();
							delete[] Load;

						}
					}
				}
			}
		}
	}


	finish = clock();
	cout << "Spend " << (double)(finish - start) / CLOCKS_PER_SEC << " seconds\n";

	cout << "Best Len \t" << BestLen << endl;
	cout << "Best Load \t";
	showLoad(BestLoad, LoadSum);


initWindow();
SetWindowText(hwnd, _T("七桥问题"));
fp = fopen("ascii_zk.bin", "r");
while (1)
{
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GameLoop();
}

fclose(fp);

return 0;
}


void GameLoop()
{
	CleanScreen();

	for (int i = 0; i < landSum; i++)
	{
		DrawCircle(landShow[i].x, landShow[i].y, 10);
		char DrawWord[10];
		sprintf(DrawWord, "land %d", i);
		DrawString(landShow[i].x, landShow[i].y + 20, DrawWord);
	}

	for (int i = 0; i < BridgeSum; i++)
	{
		char DrawWord[30];
		sprintf(DrawWord, "bridge %d len: %.2f", i, Bridge[i].len);

		DrawBridge(landShow[Bridge[i].A].x, landShow[Bridge[i].A].y, landShow[Bridge[i].B].x, landShow[Bridge[i].B].y, Bridge[i].num, DrawWord);
	}

	int* LandStep = new int[LoadSum + 1];

	LandStep[0] = Bridge[BestLoad[0]].A;
	for (int i = 0; i < LoadSum; i++)
	{

		if (LandStep[i] == Bridge[BestLoad[i]].A)
		{
			LandStep[i + 1] = Bridge[BestLoad[i]].B;
		}
		else  if (LandStep[i] == Bridge[BestLoad[i]].B)
		{
			LandStep[i + 1] = Bridge[BestLoad[i]].A;
		}
		//cout << i << " LandStep[i-1] " << LandStep[i]  << " bridgeShow[Load[i]].indexA "<< Bridge[BestLoad[i]].A <<" bridgeShow[Load[i]].indexB "<< Bridge[BestLoad[i]].B << " LandStep[i] " << LandStep[i+1] << endl;
	}
	LandStep[LoadSum] = LandStep[0];


	for (int i = 0; i < LoadSum + 1; i++)
	{
		int LoadStep = Step / 2;
		if (Step % 2 == 0)
		{
			if (Step / 2 == i)
			{
				int now = Step / 2;
				DrawCircle(landShow[LandStep[now]].x, landShow[LandStep[now]].y, 10, Color(102, 186, 183));
			}
		}
		else
		{

			if (i == LoadStep)
			{
				DrawBridge(landShow[Bridge[BestLoad[i]].A].x, landShow[Bridge[BestLoad[i]].A].y, landShow[Bridge[BestLoad[i]].B].x, landShow[Bridge[BestLoad[i]].B].y, Bridge[BestLoad[i]].num, NULL, Color(0, 255, 0));
			}
		}
		if (i < LoadStep)
		{
			DrawBridge(landShow[Bridge[BestLoad[i]].A].x, landShow[Bridge[BestLoad[i]].A].y, landShow[Bridge[BestLoad[i]].B].x, landShow[Bridge[BestLoad[i]].B].y, Bridge[BestLoad[i]].num, NULL, Color(96, 55, 62));
		}
	}

	DrawRectangle(0, 500, SCREEN_WIDTH, SCREEN_HEIGHT, Color(83, 61, 91));

	int startX = 150;
	int startY = SCREEN_HEIGHT - 80;
	char TempMsg2[20];

	sprintf(TempMsg2, "Bridge: ");
	DrawString(startX, startY, TempMsg2);
	startX += 70;
	for (int i = 0; i < LoadSum; i++)
	{
		int index = (Step) / 2;
		if (index== i)
		{
			if (Step % 2 == 1)
			{
				DrawWord(startX + 50 * i, startY, '0' + BestLoad[i], Color(102, 186, 183));
			}	
			else
			{
				DrawWord(startX + 50 * i, startY, '0' + BestLoad[i], Color(255, 255, 255));
			}
		}
		else if (index > i)
		{
			DrawWord(startX + 50 * i, startY, '0' + BestLoad[i], Color(0, 0, 0));
		}
		else
		{
			DrawWord(startX + 50 * i, startY, '0' + BestLoad[i], Color(255, 255, 255));
		}

	}
	char TempMsg3[40];
	float lenNow = 0;
	for (int i = 0; i < (Step + 1) / 2; i++)
	{
		lenNow += Bridge[BestLoad[i]].len;
	}

	sprintf(TempMsg3, "step: %d/%d   len:%.2f/%.2f", (Step+1)/2,LoadSum,lenNow,BestLen);
	DrawString(150, SCREEN_HEIGHT - 60, TempMsg3);

	char TempMsg[40];
	sprintf(TempMsg, "[Enter] next step [Esc] exit");
	DrawString(150, SCREEN_HEIGHT - 40, TempMsg);

	PutBufferToScreen();
}