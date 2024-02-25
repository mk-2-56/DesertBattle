//=============================================================================
//
// キーフレームファイル読み込み処理 [keyframe.cpp]
// Author : GP11A132 15 高橋 ほの香 
//
//=============================================================================
#include "main.h"
#include "keyframe.h"

//=============================================================================
// アニメーションデータの初期化 ※テキスト
//=============================================================================
void LoadAnimDataText(char* FileName, std::vector<INTERPOLATION_DATA>& tbl)
{
	FILE* file;
	char line[256];
	float tx, ty, tz, rx, ry, rz, sx, sy, sz, frame;

	// ファイルを開く
	file = fopen(FileName, "r");
	if (file == NULL) {
		perror("ファイルのオープンに失敗しました");
		return;
	}

	// テーブルを空にする
	tbl.clear();

	// ファイルを読み込んで初期化する
	while (fgets(line, sizeof(line), file) != NULL)
	{
		(void)sscanf(line, "tx:%f ty:%f tz:%f rx:%f ry:%f rz:%f sx:%f sy:%f sz:%f frame:%f", &tx, &ty, &tz, &rx, &ry, &rz, &sx, &sy, &sz, &frame);

		tbl.push_back(
			{
			XMFLOAT3(tx * 10.0f, ty * 10.0f, tz * 10.0f),
			XMFLOAT3(XMConvertToRadians(rx), XMConvertToRadians(ry), XMConvertToRadians(rz)),
			XMFLOAT3(sx, sy, sz),
			frame
			}
		);
	}

	// ファイルを閉じる
	fclose(file);
}


//=============================================================================
// アニメーションデータの初期化 ※CSV
//=============================================================================
void LoadAnimDataCSV(char* FileName, std::vector<INTERPOLATION_DATA>& tbl)
{
	FILE* file;
	char line[256];
	float tx, ty, tz, rx, ry, rz, sx, sy, sz, frame;

	// ファイルを開く
	file = fopen(FileName, "r");
	if (file == NULL) {
		perror("ファイルのオープンに失敗しました");
		return;
	}

	// テーブルを空にする
	tbl.clear();

	// ファイルを読み込んで初期化する
	fgets(line, sizeof(line), file);

	while (fgets(line, sizeof(line), file) != NULL)
	{


		(void)sscanf(line, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &tx, &ty, &tz, &rx, &ry, &rz, &sx, &sy, &sz, &frame);

		tbl.push_back(
			{
			XMFLOAT3(tx * 10.0f, ty * 10.0f, tz * 10.0f),
			XMFLOAT3(XMConvertToRadians(rx), XMConvertToRadians(ry), XMConvertToRadians(rz)),
			XMFLOAT3(sx, sy, sz),
			frame
			}
		);
	}

	// ファイルを閉じる
	fclose(file);
}