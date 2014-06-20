#include "stdafx.h"
#include "PmCipher.h"

//暗号化ファイルプロパティクラス
//暗号化後のファイルサイズ取得
unsigned long PmCipherProperty::GetCipherSize()
{
	return m_cipher_size;
}

//平文のファイルサイズ取得
unsigned long PmCipherProperty::GetSourceSize()
{
	return m_source_size;
}

//圧縮ファイルサイズ取得
unsigned long PmCipherProperty::GetCompressSize()
{
	return m_compress_size;
}

//暗号化後のファイルのオフセット取得
unsigned long PmCipherProperty::GetOffset()
{
	return m_offset;
}

//ファイルパスの取得
string PmCipherProperty::GetPath()
{
	return m_path;
}

//ファイル名の取得
string PmCipherProperty::GetFileName()
{
	return m_path.substr(m_path.rfind("\\") + 1);
}

//ディレクトリの取得
string PmCipherProperty::GetDirectory()
{
	return m_path.substr(0, m_path.rfind("\\"));
}

//ディレクトリの深さ取得
unsigned int PmCipherProperty::GetDepth()
{
	return m_depth;
}

//ディレクトリ番号取得
unsigned int PmCipherProperty::GetDirectoryNum()
{
	return m_directory_num;
}

//暗号化後のファイルサイズセット
void PmCipherProperty::SetCipherSize(unsigned long size)
{
	m_cipher_size = size;
}

//平文のファイルサイズセット
void PmCipherProperty::SetSourceSize(unsigned long size)
{
	m_source_size = size;
}

//圧縮ファイルサイズセット
void PmCipherProperty::SetCompressSize(unsigned long size)
{
	m_compress_size = size;
}

//暗号化後のファイルのオフセットセット
void PmCipherProperty::SetOffset(unsigned long offset)
{
	m_offset = offset;
}

//ファイルパスのセット
void PmCipherProperty::SetPath(string &path)
{
	m_path = path;
}

void PmCipherProperty::SetPath(const char *path)
{
	SetPath(string(path));
}

//ディレクトリの深さセット
void PmCipherProperty::SetDepth(unsigned int depth)
{
	m_depth = depth;
}

//ディレクトリ番号セット
void PmCipherProperty::SetDirectoryNum(unsigned int num)
{
	m_directory_num = num;
}


//暗号化ファイルプロパティ一覧クラス
//コンストラクタ/デストラクタ
PmCipherPropertyList::_PmCipherPropertyList()
{
}

PmCipherPropertyList::~_PmCipherPropertyList()
{
	//プロパティ配列のメモリ解放
	for(unsigned int i = 0; i < m_properties.size(); i++)
	{
		SAFE_DELETE(m_properties[i]);
	}
	m_properties.clear();
	m_index.clear();
}

//プロパティをファイルパスで検索
PmCipherProperty *PmCipherPropertyList::FindProperty(string &source_path)
{
	//戻り値・・・プロパティクラスのポインタ
	//第1引数・・・元ファイルのパス

	//ファイルパスと添え字のマップにヒットしたらプロパティを返す
	hash_map<string, PmCipherProperty *>::iterator it;
	int a = source_path.compare(m_properties[0]->GetPath());
	if((it = m_index.find(source_path)) != m_index.end())
	{
		return it->second;
	}
	return NULL;
}

//プロパティ数取得
unsigned int PmCipherPropertyList::GetPropertySize()
{
	return m_properties.size();
}

//プロパティをインデックスで取得
PmCipherProperty *PmCipherPropertyList::GetProperty(unsigned int num)
{
	//戻り値・・・プロパティクラスのポインタ
	//第1引数・・・プロパティ配列の添え字

	if(num < m_properties.size())
	{
		return m_properties[num];
	}
	return NULL;
}

//プロパティを追加
void PmCipherPropertyList::AddProperty(PmCipherProperty *cipher_property)
{
	//第1引数・・・プロパティクラスのポインタ

	//プロパティ配列に格納
	m_properties.push_back(cipher_property);
	//パスと添え字のマップ
	pair<string, PmCipherProperty *> pair;
	pair.first = cipher_property->GetPath();
	pair.second = cipher_property;
	m_index.insert(pair);
}

//プロパティをソート
void PmCipherPropertyList::SortProperty()
{
	sort(m_properties.begin(), m_properties.end(), CompProperty);
}

//ソート用比較関数
bool PmCipherPropertyList::CompProperty(PmCipherProperty *a, PmCipherProperty *b)
{
	if(a->GetDepth() == b->GetDepth())
	{
		return a->GetDirectoryNum() < b->GetDirectoryNum();
	}
	return a->GetDepth() < b->GetDepth();
}

//暗号クラス
//定数
const unsigned int PmCipher::CIPHER_MIN_KEY_SIZE = 4;
const unsigned int PmCipher::CIPHER_MAX_KEY_SIZE = 56;
const unsigned int PmCipher::CIPHER_BUF_SIZE = 1024;
const unsigned int PmCipher::CIPHER_KEY_WORD_SIZE = 8;
const unsigned int PmCipher::CIPHER_HEADER_SIZE = 41;
const unsigned int PmCipher::CIPHER_MAX_INDEX_SIZE = 40 + MAX_PATH + 2;
//コンストラクタ/デストラクタ
PmCipher::_PmCipher()
{
	//ロケールを日本語に設定
	locale::global(locale("japanese"));
	m_key = NULL;
	//メモリの確保
	m_property_list = new PmCipherPropertyList();
	m_output_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	m_comp_input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	//復号準備フラグOFF
	m_decrypt_init = false;
	//圧縮フラグOFF
	m_compress = false;
}

PmCipher::~_PmCipher()
{
	//暗号化ファイルのクローズ
	if(m_fs_decrypt.is_open())
	{
		m_fs_decrypt.close();
	}
	//メモリの解放
	SAFE_DELETE_ARRAY(m_key);
	SAFE_DELETE(m_property_list);
	SAFE_DELETE(m_output_buf);
	SAFE_DELETE(m_comp_input_buf);
}

//鍵設定及び初期化
CIPHER_RESULT PmCipher::Init(unsigned char *key, unsigned int key_length)
{
	//戻り値・・・ステータス
	//第1引数・・・暗号鍵のポインタ
	//第2引数・・・暗号鍵の長さ
	//鍵長の検査
	CIPHER_RESULT cr = CIPHER_OK;
	if(CIPHER_BUF_SIZE % 8ul)
	{
		return CIPHER_ERR_INVALID_BUFFER_SIZE;
	}
	if(key_length >= PmCipher::CIPHER_MIN_KEY_SIZE && key_length <= PmCipher::CIPHER_MAX_KEY_SIZE)
	{
		//鍵のコピー
		m_key_length = key_length;
		m_key = new unsigned char[m_key_length];
		memcpy(m_key, key, m_key_length);
		//拡大鍵の生成
		m_blow_fish.Initialize(m_key, m_key_length);
	}
	//不正な鍵長エラー
	else
	{
		cr = CIPHER_ERR_INVALID_KEY_SIZE;
	}
	return cr;
}

//暗号化
CIPHER_RESULT PmCipher::Encrypt(const char *source_path, const char *cipher_path, bool compress, bool recursive)
{
	//戻り値・・・ステータス
	//第1引数・・・暗号化するファイル/ディレクトリのパス
	//第2引数・・・暗号化ファイルのパス
	//第3引数・・・圧縮フラグ（true:圧縮, false:無圧縮）
	//第4引数・・・再帰処理フラグ（true:再帰処理あり（デフォルト）, false:再帰処理なし）

	//ファイルストリーム
	fstream fs_cipher, fs_cipher_idx, fs_cipher_temp;
	//パス
	string str_find_path, str_source_path, str_idx_path, str_idx2_path, str_temp_path, str_cipher_path;
	//ファイルソート用
	unsigned int depth = 0, directory_num = 0;
	//ファイル検索続行フラグ
	BOOL next = TRUE;
	//戻り値
	CIPHER_RESULT cr = CIPHER_OK;
	//再帰処理用スタック
	stack<HANDLE> stack_handle;
	stack<BOOL> stack_next;
	stack<string> stack_directory;
	hash_map<string, unsigned int> map_directory_num;
	pair<string, unsigned int> pair_directory_num;
	//カレントディレクトリ
	string current_directory;
	//ファイル検索用変数
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//ファイルの位置
	unsigned long offset = 0;

	//圧縮フラグ保存
	m_compress = compress;
	char key_word[] = CIPHER_KEY_WORD;
	char cipher_key_word[CIPHER_KEY_WORD_SIZE];

	m_blow_fish.Encode((unsigned char *)key_word, (unsigned char *)cipher_key_word, 8);

	//ファイルプロパティクラス変数
	PmCipherProperty *cipher_property = NULL;

	//元ファイルパス、中間ファイルパスの生成
	str_source_path = string(source_path);
	if(str_source_path.rfind("\\") == str_source_path.size() - 1)
	{
		str_source_path = str_source_path.substr(0, str_source_path.size() - 1);
	}
	str_cipher_path = string(cipher_path);
	str_idx_path = str_cipher_path + string(".idx");
	str_idx2_path = str_cipher_path + string(".idx2");
	str_temp_path = str_cipher_path + string(".tmp");

	if(!PathFileExists(str_cipher_path.substr(0, str_cipher_path.rfind("\\")).c_str()) || !PathIsDirectory(str_cipher_path.substr(0, str_cipher_path.rfind("\\")).c_str()))
	{
		return CIPHER_ERR_FILE_NOT_FOUND;
	}

	//最初のファイル検索
	hFind = FindFirstFile(str_source_path.c_str(), &FindFileData);
	current_directory = str_source_path;

	//暗号化ファイル（中間ファイル）のオープン
	fs_cipher_temp.open(str_temp_path.c_str(), ios::out|ios::binary);
	//検索が成功した場合
	if(hFind != INVALID_HANDLE_VALUE)
	{
		//引数で指定されたパスがファイルの場合
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			//サイズチェック
			if(FindFileData.nFileSizeHigh > 0 || Over2GB(FindFileData.nFileSizeLow))
			{
				cr = CIPHER_ERR_INVALID_FILE_SIZE;
			}
			else
			{
				//ファイルを暗号化
				if((cr = PmCipher::EncryptFile(str_source_path.c_str(), FindFileData.nFileSizeLow, compress, &fs_cipher_temp, &cipher_property)) == CIPHER_OK)
				{
					cipher_property->SetOffset(offset);
					m_property_list->AddProperty(cipher_property);
					offset += cipher_property->GetCipherSize();
					cipher_property = NULL;
					FindClose(hFind);
				}
			}
			next = FALSE;
		}
		//引数で指定されたパスがディレクトリの場合
		else
		{
			//ワイルドカードを付加して再検索
			str_find_path = str_source_path + string("\\*");
			hFind = FindFirstFile(str_find_path.c_str(), &FindFileData);
			pair_directory_num.first = current_directory;
			pair_directory_num.second = directory_num;
			map_directory_num.insert(pair_directory_num);
		}
	}
	//検索が失敗した場合エラー終了
	else
	{
		cr = CIPHER_ERR_FILE_NOT_FOUND;
		next = FALSE;
	}
	//再帰処理が終了しかつ、ファイル検索が終了するまでループ
	while(!stack_handle.empty() || next)
	{
		//ディレクトリのすべてのファイルを検索し終わったら検索処理終了
		if(!next)
		{
			FindClose(hFind);
			//再帰処理の途中の場合、上位フォルダを復帰
			if(!stack_handle.empty())
			{
				hFind = stack_handle.top();
				stack_handle.pop();
				next = stack_next.top();
				stack_next.pop();
				current_directory = stack_directory.top();
				stack_directory.pop();
				depth--;
				if(next)
				{
					next = FindNextFile(hFind, &FindFileData);
				}
			}
		}

		//検索したファイル属性がディレクトリの場合でかつ、再帰処理を行う場合
		if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && recursive && next)
		{
			//下位ディレクトリの場合は再帰処理
			if(strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, ".."))
			{
				stack_handle.push(hFind);
				stack_next.push(next);
				stack_directory.push(current_directory);
				current_directory = current_directory + string("\\") + string(FindFileData.cFileName);
				depth++;
				directory_num++;
				pair_directory_num.first = current_directory;
				pair_directory_num.second = directory_num;
				map_directory_num.insert(pair_directory_num);
				str_find_path = current_directory + string("\\*");
				hFind = FindFirstFile(str_find_path.c_str(), &FindFileData);
				if(hFind != INVALID_HANDLE_VALUE)
				{
					next = TRUE;
					continue;
				}
				else
				{
					next = FALSE;
					cr = CIPHER_ERR_INVALID_FILE;
				}
			}
		}
		//検索したファイル属性がファイルの場合は暗号化
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && next)
		{
			//サイズチェック
			if(FindFileData.nFileSizeHigh > 0 || Over2GB(FindFileData.nFileSizeLow + offset))
			{
				next = FALSE;
				cr = CIPHER_ERR_INVALID_FILE_SIZE;
			}
			else
			{
				str_find_path = current_directory + string("\\") + string(FindFileData.cFileName);
				if((cr = PmCipher::EncryptFile(str_find_path.c_str(), FindFileData.nFileSizeLow, compress, &fs_cipher_temp, &cipher_property)) == CIPHER_OK)
				{
					cipher_property->SetOffset(offset);
					offset += cipher_property->GetCipherSize();
					cipher_property->SetDepth(depth);
					hash_map<string, unsigned int>::iterator it;
					if((it = map_directory_num.find(current_directory)) != map_directory_num.end())
					{
						cipher_property->SetDirectoryNum(it->second);
					}
					else
					{
						cr = CIPHER_ERR_FATAL;
						next = FALSE;
					}
					m_property_list->AddProperty(cipher_property);
					cipher_property = NULL;
				}
			}
		}
		//エラーが起きたら直ちに終了
		if(cr != CIPHER_OK)
		{
			while(!stack_handle.empty())
			{
				hFind = stack_handle.top();
				FindClose(hFind);
				stack_handle.pop();
			}
			next = FALSE;
		}
		//次のファイルを検索
		if(next)
		{
			next = FindNextFile(hFind, &FindFileData);
		}
	}
	//暗号化ファイル（中間ファイル）のクローズ
	fs_cipher_temp.close();

	//暗号化が正常に終了した場合
	if(cr == CIPHER_OK)
	{
		//ファイル、バッファ等のサイズ
		unsigned long index_size = 0, cipher_index_size, compress_index_size, buf_size, total_buf_size;
		//バッファメモリの確保
		unsigned char *input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
		unsigned char *output_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
		//文字列
		char str[CIPHER_MAX_INDEX_SIZE];

		//インデックスのソート
		m_property_list->SortProperty();
		//インデックス書き込み（平文中間ファイル）
		fs_cipher_idx.open(str_idx_path.c_str(), ios::out|ios::binary);
		//ルートディレクトリ書き込み
		sprintf_s(str, MAX_PATH + 1, "%s\n", str_source_path.c_str());
		index_size += str_source_path.length() + 1;
		fs_cipher_idx.write(str, str_source_path.length() + 1);
		for(unsigned int i = 0; i < m_property_list->GetPropertySize(); i++)
		{
			unsigned int str_size = 40 + m_property_list->GetProperty(i)->GetPath().length() + 1;
			sprintf_s(str, CIPHER_MAX_INDEX_SIZE, "%010lu%010lu%010lu%010lu%s\n", m_property_list->GetProperty(i)->GetOffset(), m_property_list->GetProperty(i)->GetSourceSize(), m_property_list->GetProperty(i)->GetCompressSize(), m_property_list->GetProperty(i)->GetCipherSize(), m_property_list->GetProperty(i)->GetPath().c_str());
			fs_cipher_idx.write(str, str_size);
			index_size += (unsigned long)str_size;
		}
		fs_cipher_idx.close();
		//インデックス書き込み（暗号化中間ファイル）
		fs_cipher_idx.open(str_idx2_path.c_str(), ios::out|ios::binary);
		if((cr = EncryptFile(str_idx_path.c_str(), index_size, compress, &fs_cipher_idx, &cipher_property)) == CIPHER_OK)
		{
			if(!Over2GB(cipher_property->GetCipherSize() + offset + CIPHER_HEADER_SIZE + CIPHER_KEY_WORD_SIZE))
			{
				compress_index_size = cipher_property->GetCompressSize();
				cipher_index_size = cipher_property->GetCipherSize();
				SAFE_DELETE(cipher_property);
				fs_cipher_idx.close();
				fs_cipher.open(str_cipher_path.c_str(), ios::out|ios::binary);
				//キーワード書き込み
				fs_cipher.write(cipher_key_word, CIPHER_KEY_WORD_SIZE);
				//ヘッダの書き込み
				sprintf_s(str, CIPHER_MAX_INDEX_SIZE, "%d%010lu%010lu%010lu%010lu", compress?1:0, index_size, compress_index_size, cipher_index_size, offset);
				fs_cipher.write(str, CIPHER_HEADER_SIZE);
				fs_cipher_idx.open(str_idx2_path.c_str(), ios::in|ios::binary);
				
				//ヘッダと暗号化したインデックス、データファイルの結合
				total_buf_size = 0;
				while(!fs_cipher_idx.eof())
				{
					buf_size = (total_buf_size + CIPHER_BUF_SIZE > cipher_index_size)?(cipher_index_size - total_buf_size):CIPHER_BUF_SIZE;
					fs_cipher_idx.read((char *)input_buf, buf_size);
					fs_cipher.write((char *)input_buf, buf_size);
					total_buf_size += buf_size;
					if(cipher_index_size <= total_buf_size)
					{
						break;
					}
				}
				fs_cipher_idx.close();
				fs_cipher_temp.open(str_temp_path.c_str(), ios::in|ios::binary);
				total_buf_size = 0;
				while(!fs_cipher_temp.eof())
				{
					buf_size = (total_buf_size + CIPHER_BUF_SIZE > offset)?(offset - total_buf_size):CIPHER_BUF_SIZE;
					fs_cipher_temp.read((char *)input_buf, buf_size);
					fs_cipher.write((char *)input_buf, buf_size);
					total_buf_size += buf_size;
					if(offset <= total_buf_size)
					{
						break;
					}
				}
				fs_cipher << std::flush;
			}
			else
			{
				cr = CIPHER_ERR_INVALID_FILE_SIZE;
			}
		}
		//ファイルのクローズ
		fs_cipher_temp.close();
		fs_cipher.close();
		m_cipher_index_size = cipher_index_size;
		//バッファメモリの解放
		SAFE_DELETE_ARRAY(input_buf);
		SAFE_DELETE_ARRAY(output_buf);
	}
	return cr;
}

//ファイルの暗号化
CIPHER_RESULT PmCipher::EncryptFile(const char *source_path, unsigned long file_size, bool compress, fstream *cipher_file, PmCipherProperty **cipher_property)
{
	//戻り値・・・ステータス
	//第1引数・・・暗号化するファイルのパス
	//第2引数・・・暗号化するファイルの元サイズ
	//第3引数・・・圧縮フラグ（true:圧縮, false:無圧縮）
	//第4引数・・・暗号化（出力）ファイルストリームのポインタ
	//第5引数(out)・・・暗号化ファイルのプロパティ

	CIPHER_RESULT cr = CIPHER_OK;

	//サイズチェック
	if(Over2GB(file_size))
	{
		return CIPHER_ERR_INVALID_FILE_SIZE;
	}

	//入力ファイルのストリーム
	fstream in_fs;
	//合計バッファサイズ
	unsigned long total_buf_size = 0, total_comp_buf_size = 0, total_cipher_buf_size = 0;
	//バッファサイズ
	DWORD buf_size, comp_size;
	//処理継続フラグ
	bool loop = true;
	//バッファメモリの確保
	unsigned char *comp_input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	unsigned char *input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	unsigned char *output_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	//入力ファイルのオープン
	in_fs.open(source_path, ios::in|ios::binary);
	//圧縮あり
	if(compress)
	{
		//ZLIBストリーム初期化
		z_stream z;
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;
		//ZLIBストリーム初期化
		if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK)
		{
			//失敗時は処理終了
			cr = CIPHER_ERR_FATAL;
			loop = false;
		}
		//ZLIB入力バッファの初期化
		z.avail_in = 0;
		z.next_out = input_buf;
		z.avail_out = PmCipher::CIPHER_BUF_SIZE;
		int status, flush = Z_NO_FLUSH;
		while(loop)
		{
			//入力バッファが空になったら平文ファイル読み込み
			if (z.avail_in == 0)
			{
				z.next_in = comp_input_buf;
				comp_size = (total_buf_size + (unsigned long)PmCipher::CIPHER_BUF_SIZE > file_size)?(DWORD)(file_size - total_buf_size):PmCipher::CIPHER_BUF_SIZE;
				z.avail_in = comp_size;
				if(total_buf_size + comp_size >= file_size)
				{
					flush = Z_FINISH;
				}
				in_fs.read((char *)comp_input_buf, comp_size);
				total_buf_size += comp_size;
			}
			//圧縮
			status = deflate(&z, flush);
			//圧縮が完了したらループ脱出
			if (status == Z_STREAM_END)
			{
				break;
			}
			//エラー時はループ脱出
			if (status != Z_OK)
			{
				break;
			}
			//出力バッファが一杯になったら暗号化ファイルに書き出し
			if (z.avail_out == 0)
			{
				//バッファサイズ算出
				buf_size = CIPHER_BUF_SIZE;
				total_comp_buf_size += buf_size;
				buf_size = m_blow_fish.GetOutputLength(buf_size);
				total_cipher_buf_size += buf_size;
				//暗号化
				m_blow_fish.Encode(input_buf, output_buf, buf_size);
				//ファイルへ書き込み
				cipher_file->write((char *)output_buf, buf_size);
				//バッファメモリ、バッファサイズ設定
				z.next_out = input_buf;
				z.avail_out = PmCipher::CIPHER_BUF_SIZE;
			}	
		}
		//最後のバッファを出力
		if ((buf_size = CIPHER_BUF_SIZE - z.avail_out) != 0)
		{
			//バッファサイズ算出
			total_comp_buf_size += buf_size;
			buf_size = m_blow_fish.GetOutputLength(buf_size);
			total_cipher_buf_size += buf_size;
			//暗号化
			m_blow_fish.Encode(input_buf, output_buf, buf_size);
			//ファイルへ書き込み
			cipher_file->write((char *)output_buf, buf_size);
			*cipher_file << std::flush;
		}
		//エラー時はサイズを0に
		if (deflateEnd(&z) != Z_OK)
		{
			cr = CIPHER_ERR_FATAL;
			total_buf_size = 0;
		}
	}
	//無圧縮
	else
	{
		//EOFまでループ
		while(!in_fs.eof())
		{
			//バッファサイズ算出
			buf_size = (total_buf_size + (unsigned long)PmCipher::CIPHER_BUF_SIZE > file_size)?(DWORD)(file_size - total_buf_size):PmCipher::CIPHER_BUF_SIZE;
			//平文ファイルの読み込み
			in_fs.read((char *)input_buf, buf_size);
			buf_size = m_blow_fish.GetOutputLength((DWORD)buf_size);
			//暗号化
			m_blow_fish.Encode(input_buf, output_buf, buf_size);
			//暗号化（出力）ファイルに書き出し
			cipher_file->write((char *)output_buf, buf_size);
			total_buf_size += buf_size;
			//ファイルサイズ分読み込んだらループ脱出
			if(file_size <= total_buf_size)
			{
				total_cipher_buf_size = total_buf_size;
				break;
			}
		}
		*cipher_file << std::flush;
	}
	//バッファメモリの解放
	SAFE_DELETE_ARRAY(comp_input_buf);
	SAFE_DELETE_ARRAY(input_buf);
	SAFE_DELETE_ARRAY(output_buf);
	//平文（入力）ファイルのクローズ
	in_fs.close();

	if(cr == CIPHER_OK)
	{
		//ファイルプロパティの生成、設定
		*cipher_property = new PmCipherProperty();
		(*cipher_property)->SetSourceSize(file_size);
		(*cipher_property)->SetCompressSize(total_comp_buf_size);
		(*cipher_property)->SetCipherSize(total_cipher_buf_size);
		(*cipher_property)->SetPath(source_path);
	}
	return cr;
}

//暗号化ファイルの解析
CIPHER_RESULT PmCipher::ParseCipherFile(const char *cipher_path)
{
	//戻り値・・・ステータス
	//第1引数・・・暗号化ファイルのパス

	//ファイルプロパティ文字列
	char ch, str_size[10], *str_index, key_word[CIPHER_KEY_WORD_SIZE], cipher_key_word[CIPHER_KEY_WORD_SIZE];
	//戻り値
	CIPHER_RESULT cr = CIPHER_OK;
	//プロパティ一覧の解放（未解放時のための処理）
	SAFE_DELETE(m_property_list);
	
	//復号準備フラグOFF
	m_decrypt_init = false;
	//圧縮フラグOFF
	m_compress = false;

	//ファイルが存在しかつ、ディレクトリ属性ではない場合に処理開始
	if(PathFileExists(cipher_path) && !(GetFileAttributes(cipher_path) & FILE_ATTRIBUTE_DIRECTORY))
	{

		m_cipher_path = string(cipher_path);
		//暗号化ファイルのオープン
		m_fs_decrypt.open(cipher_path, ios::in|ios::binary);
		//キーワード読み込み
		m_fs_decrypt.read(cipher_key_word, CIPHER_KEY_WORD_SIZE);
		m_blow_fish.Decode((unsigned char *)cipher_key_word, (unsigned char *)key_word, CIPHER_KEY_WORD_SIZE);
		key_word[7] = '\0';
		if(strcmp(key_word, CIPHER_KEY_WORD))
		{
			DecryptEnd();
			return CIPHER_ERR_INVALID_FILE;
		}
		m_fs_decrypt.get(ch);
		//圧縮フラグ読み込み
		if(ch == '0')
		{
			m_compress = false;
		}
		else
		{
			m_compress = true;
		}
		//プロパティ一覧ファイルの元サイズ読み込み
		m_fs_decrypt.read(str_size, 10);
		m_current_source_size = AlphaToLong(str_size, 10);
		//プロパティ一覧ファイルの圧縮サイズ読み込み
		m_fs_decrypt.read(str_size, 10);
		m_current_compress_size = AlphaToLong(str_size, 10);
		//プロパティ一覧ファイルの暗号化サイズ読み込み
		m_fs_decrypt.read(str_size, 10);
		m_cipher_index_size = AlphaToLong(str_size, 10);
		m_current_cipher_size = m_cipher_index_size;
		m_fs_decrypt.read(str_size, 10);
		//平文受け取りバッファ
		char *buf;
		//サイズ等
		unsigned int size, buf_size, j;
		PmCipherProperty *cipher_property;
		m_total_buf_size = 0ul;
		m_total_decomp_buf_size = 0ul;
		m_decrypt_init = true;
		//ZLIBストリーム初期化
		if(m_compress)
		{
			m_z.zalloc = Z_NULL;
			m_z.zfree = Z_NULL;
			m_z.opaque = Z_NULL;
			if (inflateInit(&m_z) != Z_OK)
			{
				DecryptEnd();
				return CIPHER_ERR_INVALID_FILE;
			}
			m_z.avail_in = 0;
			m_z.avail_out = (unsigned int)(min(CIPHER_BUF_SIZE, m_current_source_size - m_total_buf_size));
			m_z.next_out = m_output_buf;
		}
		CIPHER_RESULT dec_cr;
		buf_size = 0;
		str_index = new char[m_current_source_size];
		//暗号化プロパティ一覧の復号完了までループ
		do
		{
			dec_cr = Decrypt(&buf, &size);
			memcpy(str_index + buf_size, buf, size);
			buf_size += size;
		}
		while(dec_cr == CIPHER_DEC_NEXT);
		char str_path[MAX_PATH];
		//プロパティ一覧の生成
		m_property_list = new PmCipherPropertyList();
		buf_size = 0;
		//ルートパスの取得
		j = 0;
		ZeroMemory(str_path, MAX_PATH);
		for(j = 0; buf_size + j < m_current_source_size && j < MAX_PATH; j++)
		{
			if(str_index[buf_size + j] == '\n')
			{
				memcpy(str_path, str_index + buf_size, j);
				m_root_path = string(str_path);
				buf_size += j + 1;
				break;
			}
		}
		//プロパティ一覧文字列の解析
		while(dec_cr == CIPHER_DEC_FINISH && buf_size < m_current_source_size)
		{
			cipher_property = new PmCipherProperty();
			//ファイル位置の取得
			cipher_property->SetOffset(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//元ファイルサイズの取得
			cipher_property->SetSourceSize(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//圧縮ファイルサイズの取得
			cipher_property->SetCompressSize(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//暗号化ファイルサイズの取得
			cipher_property->SetCipherSize(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//ファイルパスの取得
			j = 0;
			ZeroMemory(str_path, MAX_PATH);
			for(j = 0; buf_size + j < m_current_source_size && j < MAX_PATH; j++)
			{
				if(str_index[buf_size + j] == '\n')
				{
					memcpy(str_path, str_index + buf_size, j);
					cipher_property->SetPath(string(str_path));
					buf_size += j + 1;
					break;
				}
			}
			//プロパティをプロパティ一覧に追加
			m_property_list->AddProperty(cipher_property);
		}
		//バッファメモリの解放
		SAFE_DELETE_ARRAY(str_index);
		DecryptEnd();
	}
	//ファイルが見つからないまたは、ディレクトリの場合エラー終了
	else
	{
		cr = CIPHER_ERR_FILE_NOT_FOUND;
	}
	return cr;
}

//元ファイルプロパティ一覧取得
PmCipherPropertyList *PmCipher::GetPropertyList()
{
	return m_property_list;
}

//ルートディレクトリ取得
string PmCipher::GetRootPath()
{
	return m_root_path;
}

//デコードする元ファイルの指定（暗号化ファイルのオープンなど
CIPHER_RESULT PmCipher::DecryptInit(const char *source_path)
{
	//戻り値・・・ステータス
	//第1引数・・・元ファイルのパス

	CIPHER_RESULT cr = CIPHER_OK;
	m_decrypt_init = false;
	PmCipherProperty *cipher_property;
	//ファイルパスでプロパティ一覧を検索してヒットした場合に処理開始
	if(cipher_property = m_property_list->FindProperty(string(source_path)))
	{
		//各種サイズの取得
		m_current_cipher_size = cipher_property->GetCipherSize();
		m_current_source_size = cipher_property->GetSourceSize();
		m_current_compress_size = cipher_property->GetCompressSize();
		//ファイルの位置取得
		m_current_offset = cipher_property->GetOffset() + m_cipher_index_size + (unsigned long)CIPHER_HEADER_SIZE + (unsigned long)CIPHER_KEY_WORD_SIZE;
		//バッファ初期化
		m_total_buf_size = 0ul;
		m_total_decomp_buf_size = 0ul;
		//圧縮ファイルの場合はZLIBストリーム初期化
		if(m_compress)
		{
			m_z.zalloc = Z_NULL;
			m_z.zfree = Z_NULL;
			m_z.opaque = Z_NULL;
			if (inflateInit(&m_z) != Z_OK)
			{
				return CIPHER_ERR_INVALID_FILE;
			}
			m_z.avail_in = 0;
			m_z.avail_out = (unsigned int)(min(CIPHER_BUF_SIZE, m_current_source_size - m_total_buf_size));
			m_z.next_out = m_output_buf;
		}
		//暗号化ファイルのオープンとシーク
		m_fs_decrypt.open(m_cipher_path.c_str(), ios::in|ios::binary);
		m_fs_decrypt.seekg(m_current_offset);
		//初期化フラグON
		m_decrypt_init = true;
	}
	//ファイルが存在しない場合はエラー終了
	else
	{
		cr = CIPHER_ERR_FILE_NOT_FOUND;
	}
	return cr;
}

//デコード
CIPHER_RESULT PmCipher::Decrypt(char **buf, unsigned int *size)
{
	//戻り値・・・ステータス
	//第1引数(out)・・・出力バッファ
	//第2引数(out)・・・バッファのサイズ

	//未初期化の場合はエラー終了
	if(!m_decrypt_init)
	{
		*buf = NULL;
		*size = 0;
		return CIPHER_ERR_FILE_NOT_FOUND;
	}
	//バッファメモリの確保
	unsigned char *input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	CIPHER_RESULT cr = CIPHER_DEC_NEXT;
	*size = (unsigned int)PmCipher::CIPHER_BUF_SIZE;
	unsigned int buf_size;
	//バッファゼロクリア
	ZeroMemory(m_output_buf, CIPHER_BUF_SIZE);
	//圧縮あり
	if(m_compress)
	{
		int status;
		while(1)
		{
			//入力バッファが空になったら暗号化ファイル読み込み
			if (m_z.avail_in == 0)
			{
				//バッファサイズ算出
				buf_size = (m_total_buf_size + CIPHER_BUF_SIZE > m_current_cipher_size)?(m_current_cipher_size - m_total_buf_size):CIPHER_BUF_SIZE;
				//暗号化ファイル読み込み
				m_fs_decrypt.read((char *)input_buf, buf_size);
				//復号
				m_blow_fish.Decode(input_buf, m_comp_input_buf, buf_size);
				buf_size = (m_total_buf_size + CIPHER_BUF_SIZE > m_current_compress_size)?(m_current_compress_size - m_total_buf_size):CIPHER_BUF_SIZE;
				m_total_buf_size += (unsigned long)buf_size;
				//ZLIBストリームに復号済みバッファを設定
				m_z.avail_in = buf_size;
				m_z.next_in = m_comp_input_buf;
			}
			//解凍
			status = inflate(&m_z, Z_NO_FLUSH);
			//出力バッファが一杯になった場合は処理終了
			if(m_z.avail_out == 0 || status == Z_STREAM_END)
			{
				//出力引数（バッファ、サイズ）設定
				*buf = (char *)m_output_buf;
				*size = (unsigned int)(min(CIPHER_BUF_SIZE, m_current_source_size - m_total_decomp_buf_size));
				//出力バッファをZLIBストリームに再設定
				m_z.next_out = m_output_buf;
				m_z.avail_out = (unsigned int)(min(CIPHER_BUF_SIZE, m_current_source_size - m_total_buf_size));
				//ファイルを最後まで出力したら戻り値のステータスを終了にする
				if(m_current_source_size <= m_total_decomp_buf_size + (unsigned long)*size)
				{
					cr = CIPHER_DEC_FINISH;
					m_decrypt_init = false;
				}
				m_total_decomp_buf_size += *size;
				break;
			}
			//解凍失敗ならエラー終了
			if(status != Z_OK)
			{
				*buf = NULL;
				*size = 0;
				cr = CIPHER_ERR_FATAL;
				break;
			}
		}

	}
	//無圧縮
	else
	{
		//バッファサイズの算出
		buf_size = (m_total_buf_size + CIPHER_BUF_SIZE > m_current_cipher_size)?(m_current_cipher_size - m_total_buf_size):CIPHER_BUF_SIZE;
		//暗号化ファイルの入力
		m_fs_decrypt.read((char *)input_buf, buf_size);
		//復号
		m_blow_fish.Decode(input_buf, m_output_buf, buf_size);
		//出力引数（バッファ）設定
		*buf = (char *)m_output_buf;
		//ファイルを最後まで出力したら戻り値のステータスを終了にする
		if(m_fs_decrypt.eof() || m_current_cipher_size <= m_total_buf_size + (unsigned long)buf_size)
		{
			cr = CIPHER_DEC_FINISH;
			*size = (unsigned int)(min(CIPHER_BUF_SIZE, m_current_source_size - m_total_buf_size));
			m_decrypt_init = false;
		}
		m_total_buf_size += (unsigned long)buf_size;
	}
	return cr;
}

//デコードの後始末（暗号化ファイルのクローズなど
void PmCipher::DecryptEnd()
{
	//ZLIBの後始末
	if(m_compress)
	{
		inflateEnd(&m_z);
	}
	//暗号化ファイルのクローズ
	if(m_fs_decrypt.is_open())
	{
		m_fs_decrypt.close();
	}
}

//テキスト（正の整数）をunsigned long型の数値に変換
unsigned long PmCipher::AlphaToLong(const char *str, unsigned int size)
{
	//10桁以上なら終了
	unsigned long result = 0;
	if(size > 10)
	{
		return 0;
	}
	//テキスト（正の整数）（10桁）をunsigned longに変換
	for(unsigned int i = 0; i < size; i++)
	{
		result = result * 10ul + (unsigned long)(str[i] - '0');
	}
	return result;
}

bool PmCipher::Over2GB(unsigned long size)
{
	return size > LONG_MAX;
}
