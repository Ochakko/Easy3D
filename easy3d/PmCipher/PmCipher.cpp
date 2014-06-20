#include "stdafx.h"
#include "PmCipher.h"

//�Í����t�@�C���v���p�e�B�N���X
//�Í�����̃t�@�C���T�C�Y�擾
unsigned long PmCipherProperty::GetCipherSize()
{
	return m_cipher_size;
}

//�����̃t�@�C���T�C�Y�擾
unsigned long PmCipherProperty::GetSourceSize()
{
	return m_source_size;
}

//���k�t�@�C���T�C�Y�擾
unsigned long PmCipherProperty::GetCompressSize()
{
	return m_compress_size;
}

//�Í�����̃t�@�C���̃I�t�Z�b�g�擾
unsigned long PmCipherProperty::GetOffset()
{
	return m_offset;
}

//�t�@�C���p�X�̎擾
string PmCipherProperty::GetPath()
{
	return m_path;
}

//�t�@�C�����̎擾
string PmCipherProperty::GetFileName()
{
	return m_path.substr(m_path.rfind("\\") + 1);
}

//�f�B���N�g���̎擾
string PmCipherProperty::GetDirectory()
{
	return m_path.substr(0, m_path.rfind("\\"));
}

//�f�B���N�g���̐[���擾
unsigned int PmCipherProperty::GetDepth()
{
	return m_depth;
}

//�f�B���N�g���ԍ��擾
unsigned int PmCipherProperty::GetDirectoryNum()
{
	return m_directory_num;
}

//�Í�����̃t�@�C���T�C�Y�Z�b�g
void PmCipherProperty::SetCipherSize(unsigned long size)
{
	m_cipher_size = size;
}

//�����̃t�@�C���T�C�Y�Z�b�g
void PmCipherProperty::SetSourceSize(unsigned long size)
{
	m_source_size = size;
}

//���k�t�@�C���T�C�Y�Z�b�g
void PmCipherProperty::SetCompressSize(unsigned long size)
{
	m_compress_size = size;
}

//�Í�����̃t�@�C���̃I�t�Z�b�g�Z�b�g
void PmCipherProperty::SetOffset(unsigned long offset)
{
	m_offset = offset;
}

//�t�@�C���p�X�̃Z�b�g
void PmCipherProperty::SetPath(string &path)
{
	m_path = path;
}

void PmCipherProperty::SetPath(const char *path)
{
	SetPath(string(path));
}

//�f�B���N�g���̐[���Z�b�g
void PmCipherProperty::SetDepth(unsigned int depth)
{
	m_depth = depth;
}

//�f�B���N�g���ԍ��Z�b�g
void PmCipherProperty::SetDirectoryNum(unsigned int num)
{
	m_directory_num = num;
}


//�Í����t�@�C���v���p�e�B�ꗗ�N���X
//�R���X�g���N�^/�f�X�g���N�^
PmCipherPropertyList::_PmCipherPropertyList()
{
}

PmCipherPropertyList::~_PmCipherPropertyList()
{
	//�v���p�e�B�z��̃��������
	for(unsigned int i = 0; i < m_properties.size(); i++)
	{
		SAFE_DELETE(m_properties[i]);
	}
	m_properties.clear();
	m_index.clear();
}

//�v���p�e�B���t�@�C���p�X�Ō���
PmCipherProperty *PmCipherPropertyList::FindProperty(string &source_path)
{
	//�߂�l�E�E�E�v���p�e�B�N���X�̃|�C���^
	//��1�����E�E�E���t�@�C���̃p�X

	//�t�@�C���p�X�ƓY�����̃}�b�v�Ƀq�b�g������v���p�e�B��Ԃ�
	hash_map<string, PmCipherProperty *>::iterator it;
	int a = source_path.compare(m_properties[0]->GetPath());
	if((it = m_index.find(source_path)) != m_index.end())
	{
		return it->second;
	}
	return NULL;
}

//�v���p�e�B���擾
unsigned int PmCipherPropertyList::GetPropertySize()
{
	return m_properties.size();
}

//�v���p�e�B���C���f�b�N�X�Ŏ擾
PmCipherProperty *PmCipherPropertyList::GetProperty(unsigned int num)
{
	//�߂�l�E�E�E�v���p�e�B�N���X�̃|�C���^
	//��1�����E�E�E�v���p�e�B�z��̓Y����

	if(num < m_properties.size())
	{
		return m_properties[num];
	}
	return NULL;
}

//�v���p�e�B��ǉ�
void PmCipherPropertyList::AddProperty(PmCipherProperty *cipher_property)
{
	//��1�����E�E�E�v���p�e�B�N���X�̃|�C���^

	//�v���p�e�B�z��Ɋi�[
	m_properties.push_back(cipher_property);
	//�p�X�ƓY�����̃}�b�v
	pair<string, PmCipherProperty *> pair;
	pair.first = cipher_property->GetPath();
	pair.second = cipher_property;
	m_index.insert(pair);
}

//�v���p�e�B���\�[�g
void PmCipherPropertyList::SortProperty()
{
	sort(m_properties.begin(), m_properties.end(), CompProperty);
}

//�\�[�g�p��r�֐�
bool PmCipherPropertyList::CompProperty(PmCipherProperty *a, PmCipherProperty *b)
{
	if(a->GetDepth() == b->GetDepth())
	{
		return a->GetDirectoryNum() < b->GetDirectoryNum();
	}
	return a->GetDepth() < b->GetDepth();
}

//�Í��N���X
//�萔
const unsigned int PmCipher::CIPHER_MIN_KEY_SIZE = 4;
const unsigned int PmCipher::CIPHER_MAX_KEY_SIZE = 56;
const unsigned int PmCipher::CIPHER_BUF_SIZE = 1024;
const unsigned int PmCipher::CIPHER_KEY_WORD_SIZE = 8;
const unsigned int PmCipher::CIPHER_HEADER_SIZE = 41;
const unsigned int PmCipher::CIPHER_MAX_INDEX_SIZE = 40 + MAX_PATH + 2;
//�R���X�g���N�^/�f�X�g���N�^
PmCipher::_PmCipher()
{
	//���P�[������{��ɐݒ�
	locale::global(locale("japanese"));
	m_key = NULL;
	//�������̊m��
	m_property_list = new PmCipherPropertyList();
	m_output_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	m_comp_input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	//���������t���OOFF
	m_decrypt_init = false;
	//���k�t���OOFF
	m_compress = false;
}

PmCipher::~_PmCipher()
{
	//�Í����t�@�C���̃N���[�Y
	if(m_fs_decrypt.is_open())
	{
		m_fs_decrypt.close();
	}
	//�������̉��
	SAFE_DELETE_ARRAY(m_key);
	SAFE_DELETE(m_property_list);
	SAFE_DELETE(m_output_buf);
	SAFE_DELETE(m_comp_input_buf);
}

//���ݒ�y�я�����
CIPHER_RESULT PmCipher::Init(unsigned char *key, unsigned int key_length)
{
	//�߂�l�E�E�E�X�e�[�^�X
	//��1�����E�E�E�Í����̃|�C���^
	//��2�����E�E�E�Í����̒���
	//�����̌���
	CIPHER_RESULT cr = CIPHER_OK;
	if(CIPHER_BUF_SIZE % 8ul)
	{
		return CIPHER_ERR_INVALID_BUFFER_SIZE;
	}
	if(key_length >= PmCipher::CIPHER_MIN_KEY_SIZE && key_length <= PmCipher::CIPHER_MAX_KEY_SIZE)
	{
		//���̃R�s�[
		m_key_length = key_length;
		m_key = new unsigned char[m_key_length];
		memcpy(m_key, key, m_key_length);
		//�g�匮�̐���
		m_blow_fish.Initialize(m_key, m_key_length);
	}
	//�s���Ȍ����G���[
	else
	{
		cr = CIPHER_ERR_INVALID_KEY_SIZE;
	}
	return cr;
}

//�Í���
CIPHER_RESULT PmCipher::Encrypt(const char *source_path, const char *cipher_path, bool compress, bool recursive)
{
	//�߂�l�E�E�E�X�e�[�^�X
	//��1�����E�E�E�Í�������t�@�C��/�f�B���N�g���̃p�X
	//��2�����E�E�E�Í����t�@�C���̃p�X
	//��3�����E�E�E���k�t���O�itrue:���k, false:�����k�j
	//��4�����E�E�E�ċA�����t���O�itrue:�ċA��������i�f�t�H���g�j, false:�ċA�����Ȃ��j

	//�t�@�C���X�g���[��
	fstream fs_cipher, fs_cipher_idx, fs_cipher_temp;
	//�p�X
	string str_find_path, str_source_path, str_idx_path, str_idx2_path, str_temp_path, str_cipher_path;
	//�t�@�C���\�[�g�p
	unsigned int depth = 0, directory_num = 0;
	//�t�@�C���������s�t���O
	BOOL next = TRUE;
	//�߂�l
	CIPHER_RESULT cr = CIPHER_OK;
	//�ċA�����p�X�^�b�N
	stack<HANDLE> stack_handle;
	stack<BOOL> stack_next;
	stack<string> stack_directory;
	hash_map<string, unsigned int> map_directory_num;
	pair<string, unsigned int> pair_directory_num;
	//�J�����g�f�B���N�g��
	string current_directory;
	//�t�@�C�������p�ϐ�
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//�t�@�C���̈ʒu
	unsigned long offset = 0;

	//���k�t���O�ۑ�
	m_compress = compress;
	char key_word[] = CIPHER_KEY_WORD;
	char cipher_key_word[CIPHER_KEY_WORD_SIZE];

	m_blow_fish.Encode((unsigned char *)key_word, (unsigned char *)cipher_key_word, 8);

	//�t�@�C���v���p�e�B�N���X�ϐ�
	PmCipherProperty *cipher_property = NULL;

	//���t�@�C���p�X�A���ԃt�@�C���p�X�̐���
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

	//�ŏ��̃t�@�C������
	hFind = FindFirstFile(str_source_path.c_str(), &FindFileData);
	current_directory = str_source_path;

	//�Í����t�@�C���i���ԃt�@�C���j�̃I�[�v��
	fs_cipher_temp.open(str_temp_path.c_str(), ios::out|ios::binary);
	//���������������ꍇ
	if(hFind != INVALID_HANDLE_VALUE)
	{
		//�����Ŏw�肳�ꂽ�p�X���t�@�C���̏ꍇ
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			//�T�C�Y�`�F�b�N
			if(FindFileData.nFileSizeHigh > 0 || Over2GB(FindFileData.nFileSizeLow))
			{
				cr = CIPHER_ERR_INVALID_FILE_SIZE;
			}
			else
			{
				//�t�@�C�����Í���
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
		//�����Ŏw�肳�ꂽ�p�X���f�B���N�g���̏ꍇ
		else
		{
			//���C���h�J�[�h��t�����čČ���
			str_find_path = str_source_path + string("\\*");
			hFind = FindFirstFile(str_find_path.c_str(), &FindFileData);
			pair_directory_num.first = current_directory;
			pair_directory_num.second = directory_num;
			map_directory_num.insert(pair_directory_num);
		}
	}
	//���������s�����ꍇ�G���[�I��
	else
	{
		cr = CIPHER_ERR_FILE_NOT_FOUND;
		next = FALSE;
	}
	//�ċA�������I�������A�t�@�C���������I������܂Ń��[�v
	while(!stack_handle.empty() || next)
	{
		//�f�B���N�g���̂��ׂẴt�@�C�����������I������猟�������I��
		if(!next)
		{
			FindClose(hFind);
			//�ċA�����̓r���̏ꍇ�A��ʃt�H���_�𕜋A
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

		//���������t�@�C���������f�B���N�g���̏ꍇ�ł��A�ċA�������s���ꍇ
		if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && recursive && next)
		{
			//���ʃf�B���N�g���̏ꍇ�͍ċA����
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
		//���������t�@�C���������t�@�C���̏ꍇ�͈Í���
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && next)
		{
			//�T�C�Y�`�F�b�N
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
		//�G���[���N�����璼���ɏI��
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
		//���̃t�@�C��������
		if(next)
		{
			next = FindNextFile(hFind, &FindFileData);
		}
	}
	//�Í����t�@�C���i���ԃt�@�C���j�̃N���[�Y
	fs_cipher_temp.close();

	//�Í���������ɏI�������ꍇ
	if(cr == CIPHER_OK)
	{
		//�t�@�C���A�o�b�t�@���̃T�C�Y
		unsigned long index_size = 0, cipher_index_size, compress_index_size, buf_size, total_buf_size;
		//�o�b�t�@�������̊m��
		unsigned char *input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
		unsigned char *output_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
		//������
		char str[CIPHER_MAX_INDEX_SIZE];

		//�C���f�b�N�X�̃\�[�g
		m_property_list->SortProperty();
		//�C���f�b�N�X�������݁i�������ԃt�@�C���j
		fs_cipher_idx.open(str_idx_path.c_str(), ios::out|ios::binary);
		//���[�g�f�B���N�g����������
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
		//�C���f�b�N�X�������݁i�Í������ԃt�@�C���j
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
				//�L�[���[�h��������
				fs_cipher.write(cipher_key_word, CIPHER_KEY_WORD_SIZE);
				//�w�b�_�̏�������
				sprintf_s(str, CIPHER_MAX_INDEX_SIZE, "%d%010lu%010lu%010lu%010lu", compress?1:0, index_size, compress_index_size, cipher_index_size, offset);
				fs_cipher.write(str, CIPHER_HEADER_SIZE);
				fs_cipher_idx.open(str_idx2_path.c_str(), ios::in|ios::binary);
				
				//�w�b�_�ƈÍ��������C���f�b�N�X�A�f�[�^�t�@�C���̌���
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
		//�t�@�C���̃N���[�Y
		fs_cipher_temp.close();
		fs_cipher.close();
		m_cipher_index_size = cipher_index_size;
		//�o�b�t�@�������̉��
		SAFE_DELETE_ARRAY(input_buf);
		SAFE_DELETE_ARRAY(output_buf);
	}
	return cr;
}

//�t�@�C���̈Í���
CIPHER_RESULT PmCipher::EncryptFile(const char *source_path, unsigned long file_size, bool compress, fstream *cipher_file, PmCipherProperty **cipher_property)
{
	//�߂�l�E�E�E�X�e�[�^�X
	//��1�����E�E�E�Í�������t�@�C���̃p�X
	//��2�����E�E�E�Í�������t�@�C���̌��T�C�Y
	//��3�����E�E�E���k�t���O�itrue:���k, false:�����k�j
	//��4�����E�E�E�Í����i�o�́j�t�@�C���X�g���[���̃|�C���^
	//��5����(out)�E�E�E�Í����t�@�C���̃v���p�e�B

	CIPHER_RESULT cr = CIPHER_OK;

	//�T�C�Y�`�F�b�N
	if(Over2GB(file_size))
	{
		return CIPHER_ERR_INVALID_FILE_SIZE;
	}

	//���̓t�@�C���̃X�g���[��
	fstream in_fs;
	//���v�o�b�t�@�T�C�Y
	unsigned long total_buf_size = 0, total_comp_buf_size = 0, total_cipher_buf_size = 0;
	//�o�b�t�@�T�C�Y
	DWORD buf_size, comp_size;
	//�����p���t���O
	bool loop = true;
	//�o�b�t�@�������̊m��
	unsigned char *comp_input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	unsigned char *input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	unsigned char *output_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	//���̓t�@�C���̃I�[�v��
	in_fs.open(source_path, ios::in|ios::binary);
	//���k����
	if(compress)
	{
		//ZLIB�X�g���[��������
		z_stream z;
		z.zalloc = Z_NULL;
		z.zfree = Z_NULL;
		z.opaque = Z_NULL;
		//ZLIB�X�g���[��������
		if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK)
		{
			//���s���͏����I��
			cr = CIPHER_ERR_FATAL;
			loop = false;
		}
		//ZLIB���̓o�b�t�@�̏�����
		z.avail_in = 0;
		z.next_out = input_buf;
		z.avail_out = PmCipher::CIPHER_BUF_SIZE;
		int status, flush = Z_NO_FLUSH;
		while(loop)
		{
			//���̓o�b�t�@����ɂȂ����畽���t�@�C���ǂݍ���
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
			//���k
			status = deflate(&z, flush);
			//���k�����������烋�[�v�E�o
			if (status == Z_STREAM_END)
			{
				break;
			}
			//�G���[���̓��[�v�E�o
			if (status != Z_OK)
			{
				break;
			}
			//�o�̓o�b�t�@����t�ɂȂ�����Í����t�@�C���ɏ����o��
			if (z.avail_out == 0)
			{
				//�o�b�t�@�T�C�Y�Z�o
				buf_size = CIPHER_BUF_SIZE;
				total_comp_buf_size += buf_size;
				buf_size = m_blow_fish.GetOutputLength(buf_size);
				total_cipher_buf_size += buf_size;
				//�Í���
				m_blow_fish.Encode(input_buf, output_buf, buf_size);
				//�t�@�C���֏�������
				cipher_file->write((char *)output_buf, buf_size);
				//�o�b�t�@�������A�o�b�t�@�T�C�Y�ݒ�
				z.next_out = input_buf;
				z.avail_out = PmCipher::CIPHER_BUF_SIZE;
			}	
		}
		//�Ō�̃o�b�t�@���o��
		if ((buf_size = CIPHER_BUF_SIZE - z.avail_out) != 0)
		{
			//�o�b�t�@�T�C�Y�Z�o
			total_comp_buf_size += buf_size;
			buf_size = m_blow_fish.GetOutputLength(buf_size);
			total_cipher_buf_size += buf_size;
			//�Í���
			m_blow_fish.Encode(input_buf, output_buf, buf_size);
			//�t�@�C���֏�������
			cipher_file->write((char *)output_buf, buf_size);
			*cipher_file << std::flush;
		}
		//�G���[���̓T�C�Y��0��
		if (deflateEnd(&z) != Z_OK)
		{
			cr = CIPHER_ERR_FATAL;
			total_buf_size = 0;
		}
	}
	//�����k
	else
	{
		//EOF�܂Ń��[�v
		while(!in_fs.eof())
		{
			//�o�b�t�@�T�C�Y�Z�o
			buf_size = (total_buf_size + (unsigned long)PmCipher::CIPHER_BUF_SIZE > file_size)?(DWORD)(file_size - total_buf_size):PmCipher::CIPHER_BUF_SIZE;
			//�����t�@�C���̓ǂݍ���
			in_fs.read((char *)input_buf, buf_size);
			buf_size = m_blow_fish.GetOutputLength((DWORD)buf_size);
			//�Í���
			m_blow_fish.Encode(input_buf, output_buf, buf_size);
			//�Í����i�o�́j�t�@�C���ɏ����o��
			cipher_file->write((char *)output_buf, buf_size);
			total_buf_size += buf_size;
			//�t�@�C���T�C�Y���ǂݍ��񂾂烋�[�v�E�o
			if(file_size <= total_buf_size)
			{
				total_cipher_buf_size = total_buf_size;
				break;
			}
		}
		*cipher_file << std::flush;
	}
	//�o�b�t�@�������̉��
	SAFE_DELETE_ARRAY(comp_input_buf);
	SAFE_DELETE_ARRAY(input_buf);
	SAFE_DELETE_ARRAY(output_buf);
	//�����i���́j�t�@�C���̃N���[�Y
	in_fs.close();

	if(cr == CIPHER_OK)
	{
		//�t�@�C���v���p�e�B�̐����A�ݒ�
		*cipher_property = new PmCipherProperty();
		(*cipher_property)->SetSourceSize(file_size);
		(*cipher_property)->SetCompressSize(total_comp_buf_size);
		(*cipher_property)->SetCipherSize(total_cipher_buf_size);
		(*cipher_property)->SetPath(source_path);
	}
	return cr;
}

//�Í����t�@�C���̉��
CIPHER_RESULT PmCipher::ParseCipherFile(const char *cipher_path)
{
	//�߂�l�E�E�E�X�e�[�^�X
	//��1�����E�E�E�Í����t�@�C���̃p�X

	//�t�@�C���v���p�e�B������
	char ch, str_size[10], *str_index, key_word[CIPHER_KEY_WORD_SIZE], cipher_key_word[CIPHER_KEY_WORD_SIZE];
	//�߂�l
	CIPHER_RESULT cr = CIPHER_OK;
	//�v���p�e�B�ꗗ�̉���i��������̂��߂̏����j
	SAFE_DELETE(m_property_list);
	
	//���������t���OOFF
	m_decrypt_init = false;
	//���k�t���OOFF
	m_compress = false;

	//�t�@�C�������݂����A�f�B���N�g�������ł͂Ȃ��ꍇ�ɏ����J�n
	if(PathFileExists(cipher_path) && !(GetFileAttributes(cipher_path) & FILE_ATTRIBUTE_DIRECTORY))
	{

		m_cipher_path = string(cipher_path);
		//�Í����t�@�C���̃I�[�v��
		m_fs_decrypt.open(cipher_path, ios::in|ios::binary);
		//�L�[���[�h�ǂݍ���
		m_fs_decrypt.read(cipher_key_word, CIPHER_KEY_WORD_SIZE);
		m_blow_fish.Decode((unsigned char *)cipher_key_word, (unsigned char *)key_word, CIPHER_KEY_WORD_SIZE);
		key_word[7] = '\0';
		if(strcmp(key_word, CIPHER_KEY_WORD))
		{
			DecryptEnd();
			return CIPHER_ERR_INVALID_FILE;
		}
		m_fs_decrypt.get(ch);
		//���k�t���O�ǂݍ���
		if(ch == '0')
		{
			m_compress = false;
		}
		else
		{
			m_compress = true;
		}
		//�v���p�e�B�ꗗ�t�@�C���̌��T�C�Y�ǂݍ���
		m_fs_decrypt.read(str_size, 10);
		m_current_source_size = AlphaToLong(str_size, 10);
		//�v���p�e�B�ꗗ�t�@�C���̈��k�T�C�Y�ǂݍ���
		m_fs_decrypt.read(str_size, 10);
		m_current_compress_size = AlphaToLong(str_size, 10);
		//�v���p�e�B�ꗗ�t�@�C���̈Í����T�C�Y�ǂݍ���
		m_fs_decrypt.read(str_size, 10);
		m_cipher_index_size = AlphaToLong(str_size, 10);
		m_current_cipher_size = m_cipher_index_size;
		m_fs_decrypt.read(str_size, 10);
		//�����󂯎��o�b�t�@
		char *buf;
		//�T�C�Y��
		unsigned int size, buf_size, j;
		PmCipherProperty *cipher_property;
		m_total_buf_size = 0ul;
		m_total_decomp_buf_size = 0ul;
		m_decrypt_init = true;
		//ZLIB�X�g���[��������
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
		//�Í����v���p�e�B�ꗗ�̕��������܂Ń��[�v
		do
		{
			dec_cr = Decrypt(&buf, &size);
			memcpy(str_index + buf_size, buf, size);
			buf_size += size;
		}
		while(dec_cr == CIPHER_DEC_NEXT);
		char str_path[MAX_PATH];
		//�v���p�e�B�ꗗ�̐���
		m_property_list = new PmCipherPropertyList();
		buf_size = 0;
		//���[�g�p�X�̎擾
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
		//�v���p�e�B�ꗗ������̉��
		while(dec_cr == CIPHER_DEC_FINISH && buf_size < m_current_source_size)
		{
			cipher_property = new PmCipherProperty();
			//�t�@�C���ʒu�̎擾
			cipher_property->SetOffset(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//���t�@�C���T�C�Y�̎擾
			cipher_property->SetSourceSize(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//���k�t�@�C���T�C�Y�̎擾
			cipher_property->SetCompressSize(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//�Í����t�@�C���T�C�Y�̎擾
			cipher_property->SetCipherSize(AlphaToLong(str_index + buf_size, 10));
			buf_size += 10;
			//�t�@�C���p�X�̎擾
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
			//�v���p�e�B���v���p�e�B�ꗗ�ɒǉ�
			m_property_list->AddProperty(cipher_property);
		}
		//�o�b�t�@�������̉��
		SAFE_DELETE_ARRAY(str_index);
		DecryptEnd();
	}
	//�t�@�C����������Ȃ��܂��́A�f�B���N�g���̏ꍇ�G���[�I��
	else
	{
		cr = CIPHER_ERR_FILE_NOT_FOUND;
	}
	return cr;
}

//���t�@�C���v���p�e�B�ꗗ�擾
PmCipherPropertyList *PmCipher::GetPropertyList()
{
	return m_property_list;
}

//���[�g�f�B���N�g���擾
string PmCipher::GetRootPath()
{
	return m_root_path;
}

//�f�R�[�h���錳�t�@�C���̎w��i�Í����t�@�C���̃I�[�v���Ȃ�
CIPHER_RESULT PmCipher::DecryptInit(const char *source_path)
{
	//�߂�l�E�E�E�X�e�[�^�X
	//��1�����E�E�E���t�@�C���̃p�X

	CIPHER_RESULT cr = CIPHER_OK;
	m_decrypt_init = false;
	PmCipherProperty *cipher_property;
	//�t�@�C���p�X�Ńv���p�e�B�ꗗ���������ăq�b�g�����ꍇ�ɏ����J�n
	if(cipher_property = m_property_list->FindProperty(string(source_path)))
	{
		//�e��T�C�Y�̎擾
		m_current_cipher_size = cipher_property->GetCipherSize();
		m_current_source_size = cipher_property->GetSourceSize();
		m_current_compress_size = cipher_property->GetCompressSize();
		//�t�@�C���̈ʒu�擾
		m_current_offset = cipher_property->GetOffset() + m_cipher_index_size + (unsigned long)CIPHER_HEADER_SIZE + (unsigned long)CIPHER_KEY_WORD_SIZE;
		//�o�b�t�@������
		m_total_buf_size = 0ul;
		m_total_decomp_buf_size = 0ul;
		//���k�t�@�C���̏ꍇ��ZLIB�X�g���[��������
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
		//�Í����t�@�C���̃I�[�v���ƃV�[�N
		m_fs_decrypt.open(m_cipher_path.c_str(), ios::in|ios::binary);
		m_fs_decrypt.seekg(m_current_offset);
		//�������t���OON
		m_decrypt_init = true;
	}
	//�t�@�C�������݂��Ȃ��ꍇ�̓G���[�I��
	else
	{
		cr = CIPHER_ERR_FILE_NOT_FOUND;
	}
	return cr;
}

//�f�R�[�h
CIPHER_RESULT PmCipher::Decrypt(char **buf, unsigned int *size)
{
	//�߂�l�E�E�E�X�e�[�^�X
	//��1����(out)�E�E�E�o�̓o�b�t�@
	//��2����(out)�E�E�E�o�b�t�@�̃T�C�Y

	//���������̏ꍇ�̓G���[�I��
	if(!m_decrypt_init)
	{
		*buf = NULL;
		*size = 0;
		return CIPHER_ERR_FILE_NOT_FOUND;
	}
	//�o�b�t�@�������̊m��
	unsigned char *input_buf = new unsigned char[PmCipher::CIPHER_BUF_SIZE];
	CIPHER_RESULT cr = CIPHER_DEC_NEXT;
	*size = (unsigned int)PmCipher::CIPHER_BUF_SIZE;
	unsigned int buf_size;
	//�o�b�t�@�[���N���A
	ZeroMemory(m_output_buf, CIPHER_BUF_SIZE);
	//���k����
	if(m_compress)
	{
		int status;
		while(1)
		{
			//���̓o�b�t�@����ɂȂ�����Í����t�@�C���ǂݍ���
			if (m_z.avail_in == 0)
			{
				//�o�b�t�@�T�C�Y�Z�o
				buf_size = (m_total_buf_size + CIPHER_BUF_SIZE > m_current_cipher_size)?(m_current_cipher_size - m_total_buf_size):CIPHER_BUF_SIZE;
				//�Í����t�@�C���ǂݍ���
				m_fs_decrypt.read((char *)input_buf, buf_size);
				//����
				m_blow_fish.Decode(input_buf, m_comp_input_buf, buf_size);
				buf_size = (m_total_buf_size + CIPHER_BUF_SIZE > m_current_compress_size)?(m_current_compress_size - m_total_buf_size):CIPHER_BUF_SIZE;
				m_total_buf_size += (unsigned long)buf_size;
				//ZLIB�X�g���[���ɕ����ς݃o�b�t�@��ݒ�
				m_z.avail_in = buf_size;
				m_z.next_in = m_comp_input_buf;
			}
			//��
			status = inflate(&m_z, Z_NO_FLUSH);
			//�o�̓o�b�t�@����t�ɂȂ����ꍇ�͏����I��
			if(m_z.avail_out == 0 || status == Z_STREAM_END)
			{
				//�o�͈����i�o�b�t�@�A�T�C�Y�j�ݒ�
				*buf = (char *)m_output_buf;
				*size = (unsigned int)(min(CIPHER_BUF_SIZE, m_current_source_size - m_total_decomp_buf_size));
				//�o�̓o�b�t�@��ZLIB�X�g���[���ɍĐݒ�
				m_z.next_out = m_output_buf;
				m_z.avail_out = (unsigned int)(min(CIPHER_BUF_SIZE, m_current_source_size - m_total_buf_size));
				//�t�@�C�����Ō�܂ŏo�͂�����߂�l�̃X�e�[�^�X���I���ɂ���
				if(m_current_source_size <= m_total_decomp_buf_size + (unsigned long)*size)
				{
					cr = CIPHER_DEC_FINISH;
					m_decrypt_init = false;
				}
				m_total_decomp_buf_size += *size;
				break;
			}
			//�𓀎��s�Ȃ�G���[�I��
			if(status != Z_OK)
			{
				*buf = NULL;
				*size = 0;
				cr = CIPHER_ERR_FATAL;
				break;
			}
		}

	}
	//�����k
	else
	{
		//�o�b�t�@�T�C�Y�̎Z�o
		buf_size = (m_total_buf_size + CIPHER_BUF_SIZE > m_current_cipher_size)?(m_current_cipher_size - m_total_buf_size):CIPHER_BUF_SIZE;
		//�Í����t�@�C���̓���
		m_fs_decrypt.read((char *)input_buf, buf_size);
		//����
		m_blow_fish.Decode(input_buf, m_output_buf, buf_size);
		//�o�͈����i�o�b�t�@�j�ݒ�
		*buf = (char *)m_output_buf;
		//�t�@�C�����Ō�܂ŏo�͂�����߂�l�̃X�e�[�^�X���I���ɂ���
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

//�f�R�[�h�̌�n���i�Í����t�@�C���̃N���[�Y�Ȃ�
void PmCipher::DecryptEnd()
{
	//ZLIB�̌�n��
	if(m_compress)
	{
		inflateEnd(&m_z);
	}
	//�Í����t�@�C���̃N���[�Y
	if(m_fs_decrypt.is_open())
	{
		m_fs_decrypt.close();
	}
}

//�e�L�X�g�i���̐����j��unsigned long�^�̐��l�ɕϊ�
unsigned long PmCipher::AlphaToLong(const char *str, unsigned int size)
{
	//10���ȏ�Ȃ�I��
	unsigned long result = 0;
	if(size > 10)
	{
		return 0;
	}
	//�e�L�X�g�i���̐����j�i10���j��unsigned long�ɕϊ�
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
