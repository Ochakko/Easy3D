#include <windows.h>
#include <stdio.h>
#include <imagehlp.h>
#include <string>
#include "PmCipher.h"

using namespace std;

int main(int argc, char *argv[])
{
	//************引数解説************
	//第1引数・・・エンコード/デコード指定(e/d)
	//エンコードの場合
	//第2引数・・・暗号化するファイル/ディレクトリパス
	//第3引数・・・暗号化後のパックファイルパス（ファイル名指定、ディレクトリ不可）
	//第4引数・・・圧縮フラグ（0/1)
	//第5引数・・・再帰処理フラグ（0/1)
	//デコードの場合
	//第2引数・・・暗号化後のパックファイルパス（ファイル名指定、ディレクトリ不可）
	//第3引数・・・復号したファイルの出力先ディレクトリ（ファイル名不可）

	//鍵定義
	unsigned char key[] = {0x01, 0x02, 0x03, 0x04}; //実際にはもっと複雑な値にしてください。
	unsigned int key_length = 4;
	//暗号化クラス変数の定義
	PmCipher cipher;

	if(argc < 2)
	{
		printf("無効な引数です。\n");
		return 0;
	}

	//エンコード
	//e "Test" "Cipher\test.cph"
	if(*argv[1] == 'e')
	{
		CIPHER_RESULT cr;
		if(argc < 6)
		{
			printf("無効な引数です。\n");
			return 0;
		}
		printf("暗号化開始。\n");
		//初期化（鍵その他
		if((cr = cipher.Init(key, key_length)) != CIPHER_OK)
		{
			printf("初期化終了コード：%d\n", cr);
			return 0;
		}
		//暗号化
		cr = cipher.Encrypt(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
		printf("暗号化終了コード：%d\n", cr);
	}
	//デコード
	//d "Cipher\test.cph" "Cipher"
	else if(*argv[1] == 'd')
	{
		CIPHER_RESULT cr;
		if(argc < 4)
		{
			printf("無効な引数です。\n");
			return 0;
		}
		if(PathFileExists(argv[3]))
		{
			if(!PathIsDirectory(argv[3]))
			{
				printf("出力先がディレクトリではありません。\n");
				return 0;
			}
		}
		else
		{
			string path = string(argv[3]) + string("\\");
			if(MakeSureDirectoryPathExists(path.c_str()))
			{
				printf("ディレクトリを作成しました。\n");
			}
			else
			{
				printf("ディレクトリ作成に失敗しました。\n");
				return 0;
			}
		}

		char outdir[MAX_PATH];
		strcpy_s( outdir, MAX_PATH, argv[3] );

		printf("復号開始。\n");
		//初期化（鍵その他
		if((cr = cipher.Init(key, key_length)) != CIPHER_OK)
		{
			printf("初期化終了コード：%d\n", cr);
			return 0;
		}

		//暗号化ファイル情報の解析
		cr = cipher.ParseCipherFile(argv[2]);
		printf("パース終了コード：%d\n", cr);
		//暗号化ファイル情報の取得
		PmCipherPropertyList *list = cipher.GetPropertyList();


		char rootpath[MAX_PATH];
		strcpy_s( rootpath, MAX_PATH, cipher.GetRootPath().c_str() );
		printf("ルートパス:%s\n", rootpath );


		//暗号化ファイルの数だけループ
		for(unsigned int i = 0; list && i < list->GetPropertySize(); i++)
		{
			char srcpath[MAX_PATH];
			strcpy_s( srcpath, MAX_PATH, list->GetProperty(i)->GetPath().c_str() );

			char srcdir[MAX_PATH];
			strcpy_s( srcdir, MAX_PATH, list->GetProperty(i)->GetDirectory().c_str() );
			int srcdirlen = (int)strlen( srcdir );

			int rootdirlen = (int)strlen( rootpath );
			char dstdir[MAX_PATH];
			strcpy_s( dstdir, MAX_PATH, outdir );
			if( srcdirlen > rootdirlen ){
				strcat_s( dstdir, MAX_PATH, srcdir + rootdirlen );
			}

			DWORD fattr;
			fattr = GetFileAttributes( dstdir );
			if( (fattr == -1) || ((fattr & FILE_ATTRIBUTE_DIRECTORY) == 0) ){
				int bret;
				bret = CreateDirectory( dstdir, NULL );
				if( bret == 0 ){
					::MessageBox( NULL, "ディレクトリの作成に失敗しました。", "エラー", MB_OK );
					_ASSERT( 0 );
					return 1;
				}
			}

			char dstpath[MAX_PATH];
			strcpy_s( dstpath, MAX_PATH, outdir );
			strcat_s( dstpath, MAX_PATH, srcpath + rootdirlen );

			HANDLE hfile;
			hfile = CreateFile( dstpath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
				FILE_FLAG_SEQUENTIAL_SCAN, NULL );
			if( hfile == INVALID_HANDLE_VALUE ){
				_ASSERT( 0 );
				return 1;
			}

			//デコードする元ファイルの指定（暗号化ファイルのオープンなど
			cipher.DecryptInit(list->GetProperty(i)->GetPath().c_str());
			char *buf;
			unsigned int size;
			//復号したデータの出力先

			unsigned long writeleng;
			while((cr = cipher.Decrypt(&buf, &size)) != CIPHER_DEC_FINISH && size > 0)
			{
				//バッファを出力
				WriteFile( hfile, buf, size, &writeleng, NULL );
			}
			//最後のバッファを出力
			if(size > 0)
			{
				WriteFile( hfile, buf, size, &writeleng, NULL );
			}

			//後始末（暗号化ファイルのクローズなど
			cipher.DecryptEnd();

			FlushFileBuffers( hfile );
			SetEndOfFile( hfile );
			CloseHandle( hfile );
		}



/***
		printf("復号開始。\n");
		//初期化（鍵その他
		if((cr = cipher.Init(key, key_length)) != CIPHER_OK)
		{
			printf("初期化終了コード：%d\n", cr);
			return 0;
		}

		//暗号化ファイル情報の解析
		cr = cipher.ParseCipherFile(argv[2]);
		printf("パース終了コード：%d\n", cr);
		//暗号化ファイル情報の取得
		PmCipherPropertyList *list = cipher.GetPropertyList();
		printf("ルートパス:%s\n", cipher.GetRootPath().c_str());
		//暗号化ファイルの数だけループ
		for(unsigned int i = 0; list && i < list->GetPropertySize(); i++)
		{
			//デコードする元ファイルの指定（暗号化ファイルのオープンなど
			cipher.DecryptInit(list->GetProperty(i)->GetPath().c_str());
			char *buf;
			unsigned int size;
			//復号したデータの出力先
			fstream fs;
			fs.open(string(string(argv[3]) + string("\\") + list->GetProperty(i)->GetFileName()).c_str(), ios::out|ios::binary);
			//データの最後までループ
			if(fs.is_open())
			{
				while((cr = cipher.Decrypt(&buf, &size)) != CIPHER_DEC_FINISH && size > 0)
				{
					//バッファを出力
					fs.write(buf, size);
				}
				//最後のバッファを出力
				if(size > 0)
				{
					fs.write(buf, size);
				}
				//後始末（暗号化ファイルのクローズなど
				cipher.DecryptEnd();
				fs << flush;
				fs.close();
			}
		}
***/
	}
	else
	{
		printf("無効な引数です。\n");
	}
	printf("処理完了。\n");
	return 0;
}