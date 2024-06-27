#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bitmap.h"
#define PI 3.14159265358
#define MAX_MSK_SIZE 15	 // マスクサイズの最大値
#define LAP_MSK 3 //ラプラシアンフィルタのマスクサイズ

int MSK_SIZE; // フィルタのマスクサイズ
double SIGMA; // ガウス関数の分散

char LaplacianFilter(unsigned char aPixel[MSK_SIZE * MSK_SIZE])
{
	//MSK_SIZEは３であることを前提に作成してある
	int sum = 0;
	for(int i=0;i<LAP_MSK*LAP_MSK;i++){
		if(i == 4) sum -= 8*aPixel[4];
		else sum += aPixel[i];
	}

	//境目をよりくっきりとさせるため
	if(sum > 200) sum = 255;
	else if(sum < 80) sum = 0;

	return sum;
}

char GaussianLaplacianFilter(unsigned char aPixel[MSK_SIZE * MSK_SIZE])
{
	int cnt = 0;
	unsigned char box[MSK_SIZE][MSK_SIZE];	//MSK_SIZEの正方形
	//一次元配列からMSK_SIZE次元配列への変換
	while(cnt != MSK_SIZE){
		for(int i=0;i<MSK_SIZE;i++){box[cnt][i] = aPixel[i+cnt*MSK_SIZE];}
		cnt++;
	}

	double sum = 0;
	for(int i=0;i<MSK_SIZE;i++){
		for(int j=0;j<MSK_SIZE;j++){
			double tmp = exp(-(pow(i,2)+pow(j,2))/(2*SIGMA) ) / (2*PI*SIGMA);
			sum += tmp*box[i][j];
		}
	}

	return sum;
}


int main(int argc, char *argv[])
{

	char szInputFile[256];	// 入力ファイル名
	char szOutputFile1[256]; // 出力ファイル名
	char szOutputFile2[256]; // 出力ファイル名
	int i, j, k, num = 0;	// カウンタ

	const long lWidth = 256;  // 画像の幅
	const long lHeight = 256; // 画像の高さ

	unsigned char **aBitmapIn;	// ビットマップデータ(2次元配列へのポインタ)
	unsigned char **aBitmapOut1; // ビットマップデータ(2次元配列へのポインタ)
	unsigned char **aBitmapOut2; // ビットマップデータ(2次元配列へのポインタ)
	unsigned char **aBitmapOut3; // ビットマップデータ(2次元配列へのポインタ)

	unsigned char aPixel[MAX_MSK_SIZE * MAX_MSK_SIZE];
	// ビットマップデータ(2次元配列)の作成
	aBitmapIn = CreateBitmap8(lWidth, lHeight);
	aBitmapOut1 = CreateBitmap8(lWidth, lHeight);
	aBitmapOut2 = CreateBitmap8(lWidth, lHeight);
	aBitmapOut3 = CreateBitmap8(lWidth, lHeight);

	// ファイル名を指定
	printf("入力ファイル名: ");
	scanf("%s", szInputFile);
	printf("ラプラシアンフィルタの出力ファイル名: ");
	scanf("%s", szOutputFile1);
	printf("ガウシアンラプラシアンフィルタの出力ファイル名: ");
	scanf("%s", szOutputFile2);

	ReadBitmap8(szInputFile, aBitmapIn, lWidth, lHeight);

	printf("ラプラシアンフィルタのマスクサイズは３で固定されていることに注意すること\n");
	printf("マスクサイズを入力してください: ");
	scanf("%d", &MSK_SIZE);

	SIGMA = (double)(MSK_SIZE - 1) / 8;

	//ラプラシアンフィルタ処理
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			if ((i - (LAP_MSK - 1) / 2 < 0) || (i + (LAP_MSK - 1) / 2 > lHeight - 1) || (j - (LAP_MSK - 1) / 2 < 0) || (j + (LAP_MSK - 1) / 2 > lWidth - 1))
			{
				aBitmapOut1[i][j] = aBitmapIn[i][j]; // 外周ならそのままコピー
			}
			else
			{
				for (k = 0; k < LAP_MSK * LAP_MSK; k++)
				{
					aPixel[k] = aBitmapIn[i + k / LAP_MSK - (LAP_MSK - 1) / 2][j + k % LAP_MSK - (LAP_MSK - 1) / 2]; // マスク生成
				}
				aBitmapOut1[i][j] = LaplacianFilter(aPixel); // ラプラシアンフィルタをかける
			}
		}
	}
	//ラプラシアンフィルタをかけた ビットマップファイル書き込み
	WriteBitmap8(szOutputFile1, aBitmapOut1, lWidth, lHeight);


	//ガウシアンフィルタ処理
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			if ((i - (MSK_SIZE - 1) / 2 < 0) || (i + (MSK_SIZE - 1) / 2 > lHeight - 1) || (j - (MSK_SIZE - 1) / 2 < 0) || (j + (MSK_SIZE - 1) / 2 > lWidth - 1))
			{
				aBitmapOut2[i][j] = aBitmapIn[i][j];
			}
			else
			{
				for (k = 0; k < MSK_SIZE * MSK_SIZE; k++)
				{
					aPixel[k] = aBitmapIn[i + k / MSK_SIZE - (MSK_SIZE - 1) / 2][j + k % MSK_SIZE - (MSK_SIZE - 1) / 2]; // マスク生成
				}
				aBitmapOut2[i][j] = GaussianLaplacianFilter(aPixel); // ガウシアンラプラシアンフィルタをかける
			}
		}
	}
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			if ((i - (LAP_MSK - 1) / 2 < 0) || (i + (LAP_MSK - 1) / 2 > lHeight - 1) || (j - (LAP_MSK - 1) / 2 < 0) || (j + (LAP_MSK - 1) / 2 > lWidth - 1))
			{
				aBitmapOut3[i][j] = aBitmapOut2[i][j];
			}
			else
			{
				for (k = 0; k < LAP_MSK * LAP_MSK; k++)
				{
					aPixel[k] = aBitmapOut2[i + k / LAP_MSK - (LAP_MSK - 1) / 2][j + k % LAP_MSK - (LAP_MSK - 1) / 2]; // マスク生成
				}
				aBitmapOut3[i][j] = LaplacianFilter(aPixel); // ラプラシアンフィルタをかける
			}
		}
	}
	//ラプラシアンフィルタをかけた ビットマップファイル書き込み
	WriteBitmap8(szOutputFile2, aBitmapOut3, lWidth, lHeight);

	DeleteBitmap8(aBitmapIn, lWidth, lHeight);
	DeleteBitmap8(aBitmapOut1, lWidth, lHeight);
	DeleteBitmap8(aBitmapOut2, lWidth, lHeight);
	DeleteBitmap8(aBitmapOut3, lWidth, lHeight);

	return 0;
}