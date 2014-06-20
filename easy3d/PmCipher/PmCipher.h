#include "shlwapi.h"
#include "zlib.h"
#include "blowfish.h"

#include <Windows.h>
#include <vector>
#include <string>
#include <stack>
#include <fstream>
#include <hash_map>
#include <algorithm>

//STL名前空間の宣言
using namespace std;
using namespace stdext;

//クラス名の宣言
typedef class _PmCipherPropertyList PmCipherPropertyList;
typedef class _PmCipherProperty PmCipherProperty;
typedef class _PmCipher PmCipher;

//マクロ
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    

#define CIPHER_KEY_WORD "PmCiphe\0"

//戻り値
enum CIPHER_RESULT
{
	CIPHER_OK = 0,					//成功
	CIPHER_ERR_FILE_NOT_FOUND,		//ファイルが存在しない
	CIPHER_ERR_NOT_INITIALIZED,		//未初期化
	CIPHER_ERR_INVALID_FILE,	    //無効なファイル
	CIPHER_ERR_INVALID_FILE_SIZE,	//無効なファイルサイズ
	CIPHER_ERR_INVALID_KEY_SIZE,	//無効な鍵サイズ
	CIPHER_ERR_INVALID_BUFFER_SIZE,	//無効なバッファサイズ
	CIPHER_ERR_FATAL,				//予期せぬエラー
	CIPHER_DEC_NEXT,				//復号中
	CIPHER_DEC_FINISH,				//復号完了
};

//暗号化ファイルプロパティクラス
class _PmCipherProperty
{
//変数
protected:
	//暗号化後のファイルサイズ
	unsigned long m_cipher_size;
	//元ファイルサイズ
	unsigned long m_source_size;
	//圧縮ファイルサイズ
	unsigned long m_compress_size;
	//暗号化ファイル内での位置
	unsigned long m_offset;
	//元ファイルのパス
	string m_path;
	//ディレクトリの深さ
	unsigned int m_depth;
	//ディレクトリ番号
	unsigned int m_directory_num;

//関数
public:
	//暗号化後のファイルサイズ取得
	unsigned long GetCipherSize();
	//平文のファイルサイズ取得
	unsigned long GetSourceSize();
	//圧縮ファイルサイズ取得
	unsigned long GetCompressSize();
	//暗号化後のファイルのオフセット取得
	unsigned long GetOffset();
	//ファイルパスの取得
	string GetPath();
	//ファイル名の取得
	string GetFileName();
	//ディレクトリの取得
	string GetDirectory();
	//ディレクトリの深さ取得
	unsigned int GetDepth();
	//ディレクトリ番号取得
	unsigned int GetDirectoryNum();
	//暗号化後のファイルサイズセット
	void SetCipherSize(unsigned long size);
	//平文のファイルサイズセット
	void SetSourceSize(unsigned long size);
	//圧縮ファイルサイズセット
	void SetCompressSize(unsigned long size);
	//暗号化後のファイルのオフセットセット
	void SetOffset(unsigned long offset);
	//ファイルパスのセット
	void SetPath(string &path);
	void SetPath(const char *path);
	//ディレクトリの深さセット
	void SetDepth(unsigned int depth);
	//ディレクトリ番号セット
	void SetDirectoryNum(unsigned int num);
};

//暗号化ファイルプロパティ一覧クラス
class _PmCipherPropertyList
{
//変数
protected:
	//プロパティ配列
	vector<PmCipherProperty *> m_properties;
	//元ファイルパスと添え字のマップ（検索用）
	hash_map<string, PmCipherProperty *> m_index;

//公開関数
public:
	//コンストラクタ/デストラクタ
	_PmCipherPropertyList();
	~_PmCipherPropertyList();
	//プロパティをファイルパスで検索
	PmCipherProperty *FindProperty(string &source_path);
	//プロパティ数取得
	unsigned int GetPropertySize();
	//プロパティをインデックスで取得
	PmCipherProperty *GetProperty(unsigned int num);
	//プロパティを追加
	void AddProperty(PmCipherProperty *cipher_property);
	//プロパティをソート
	void SortProperty();

//非公開関数
protected:
	//ソート用比較関数
	static bool CompProperty(PmCipherProperty *a, PmCipherProperty *b);
};

//暗号クラス
class _PmCipher
{
//定数
public:
	//最少鍵サイズ
	static const unsigned int CIPHER_MIN_KEY_SIZE;
	//最大鍵サイズ
	static const unsigned int CIPHER_MAX_KEY_SIZE;
	//バッファのサイズ ※8の倍数にしてください。暗号化と復号は同じサイズで行ってください。
	static const unsigned int CIPHER_BUF_SIZE;
	//暗号化ファイルの識別子 ※8の倍数にしてください。
	static const unsigned int CIPHER_KEY_WORD_SIZE;
	//暗号化ファイルのヘッダサイズ
	static const unsigned int CIPHER_HEADER_SIZE;
	//元ファイルごとの情報の最大サイズ
	static const unsigned int CIPHER_MAX_INDEX_SIZE;

//変数
protected:
	//鍵
	unsigned char *m_key;
	unsigned int m_key_length;
	//暗号化クラス
	CBlowFish m_blow_fish;
	//プロパティリスト
	PmCipherPropertyList *m_property_list;
	//復号初期化フラグ
	bool m_decrypt_init;
	//圧縮フラグ
	bool m_compress;
	//暗号化後のプロパティ一覧サイズ
	unsigned long m_cipher_index_size;
	//暗号化後のファイルサイズ
	unsigned long m_cipher_data_size;
	//暗号化ファイルストリーム
	fstream m_fs_decrypt;
	//デコード中のファイルの位置
	unsigned long m_current_offset;
	//デコード中の暗号化後のファイルサイズ
	unsigned long m_current_cipher_size;
	//デコード中の元ファイルサイズ
	unsigned long m_current_source_size;
	//デコード中の圧縮ファイルサイズ
	unsigned long m_current_compress_size;
	//デコード中に読み込んだ暗号化データのサイズ
	unsigned long m_total_buf_size;
	//デコード中に読み込んだ非圧縮データのサイズ
	unsigned int m_total_decomp_buf_size;
	//デコードで使用する出力バッファ
	unsigned char *m_output_buf;
	//デコードで使用する圧縮データのバッファ
	unsigned char *m_comp_input_buf;
	//デコードする元ファイルのパス
	string m_cipher_path;
	//エンコードしたパス
	string m_root_path;
	//ZLIBのストリームクラス
	z_stream m_z;

//公開関数
public:
	//コンストラクタ/デストラクタ
	_PmCipher();
	~_PmCipher();
	//鍵設定及び初期化
	CIPHER_RESULT Init(unsigned char *key, unsigned int key_length);
	//暗号化
	CIPHER_RESULT Encrypt(const char *source_path, const char *cipher_path, bool compress, bool recursive = true);
	//暗号化ファイルの解析
	CIPHER_RESULT ParseCipherFile(const char *cipher_path);
	//元ファイルプロパティ一覧取得
	PmCipherPropertyList *GetPropertyList();
	//ルートディレクトリ取得
	string GetRootPath();
	//デコードする元ファイルの指定（暗号化ファイルのオープンなど
	CIPHER_RESULT DecryptInit(const char *source_path);
	//デコード
	CIPHER_RESULT Decrypt(char **buf, unsigned int *size);
	//デコードの後始末（暗号化ファイルのクローズなど
	void DecryptEnd();

//非公開関数
protected:
	//ファイルの暗号化
	CIPHER_RESULT EncryptFile(const char *source_path, unsigned long file_size, bool compress, fstream *cipher_file, PmCipherProperty **cipher_property);
	//テキスト（正の整数）をunsigned long型の数値に変換
	unsigned long AlphaToLong(const char *str, unsigned int size);
	bool Over2GB(unsigned long size);
};