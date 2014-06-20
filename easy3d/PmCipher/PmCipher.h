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

//STL���O��Ԃ̐錾
using namespace std;
using namespace stdext;

//�N���X���̐錾
typedef class _PmCipherPropertyList PmCipherPropertyList;
typedef class _PmCipherProperty PmCipherProperty;
typedef class _PmCipher PmCipher;

//�}�N��
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    

#define CIPHER_KEY_WORD "PmCiphe\0"

//�߂�l
enum CIPHER_RESULT
{
	CIPHER_OK = 0,					//����
	CIPHER_ERR_FILE_NOT_FOUND,		//�t�@�C�������݂��Ȃ�
	CIPHER_ERR_NOT_INITIALIZED,		//��������
	CIPHER_ERR_INVALID_FILE,	    //�����ȃt�@�C��
	CIPHER_ERR_INVALID_FILE_SIZE,	//�����ȃt�@�C���T�C�Y
	CIPHER_ERR_INVALID_KEY_SIZE,	//�����Ȍ��T�C�Y
	CIPHER_ERR_INVALID_BUFFER_SIZE,	//�����ȃo�b�t�@�T�C�Y
	CIPHER_ERR_FATAL,				//�\�����ʃG���[
	CIPHER_DEC_NEXT,				//������
	CIPHER_DEC_FINISH,				//��������
};

//�Í����t�@�C���v���p�e�B�N���X
class _PmCipherProperty
{
//�ϐ�
protected:
	//�Í�����̃t�@�C���T�C�Y
	unsigned long m_cipher_size;
	//���t�@�C���T�C�Y
	unsigned long m_source_size;
	//���k�t�@�C���T�C�Y
	unsigned long m_compress_size;
	//�Í����t�@�C�����ł̈ʒu
	unsigned long m_offset;
	//���t�@�C���̃p�X
	string m_path;
	//�f�B���N�g���̐[��
	unsigned int m_depth;
	//�f�B���N�g���ԍ�
	unsigned int m_directory_num;

//�֐�
public:
	//�Í�����̃t�@�C���T�C�Y�擾
	unsigned long GetCipherSize();
	//�����̃t�@�C���T�C�Y�擾
	unsigned long GetSourceSize();
	//���k�t�@�C���T�C�Y�擾
	unsigned long GetCompressSize();
	//�Í�����̃t�@�C���̃I�t�Z�b�g�擾
	unsigned long GetOffset();
	//�t�@�C���p�X�̎擾
	string GetPath();
	//�t�@�C�����̎擾
	string GetFileName();
	//�f�B���N�g���̎擾
	string GetDirectory();
	//�f�B���N�g���̐[���擾
	unsigned int GetDepth();
	//�f�B���N�g���ԍ��擾
	unsigned int GetDirectoryNum();
	//�Í�����̃t�@�C���T�C�Y�Z�b�g
	void SetCipherSize(unsigned long size);
	//�����̃t�@�C���T�C�Y�Z�b�g
	void SetSourceSize(unsigned long size);
	//���k�t�@�C���T�C�Y�Z�b�g
	void SetCompressSize(unsigned long size);
	//�Í�����̃t�@�C���̃I�t�Z�b�g�Z�b�g
	void SetOffset(unsigned long offset);
	//�t�@�C���p�X�̃Z�b�g
	void SetPath(string &path);
	void SetPath(const char *path);
	//�f�B���N�g���̐[���Z�b�g
	void SetDepth(unsigned int depth);
	//�f�B���N�g���ԍ��Z�b�g
	void SetDirectoryNum(unsigned int num);
};

//�Í����t�@�C���v���p�e�B�ꗗ�N���X
class _PmCipherPropertyList
{
//�ϐ�
protected:
	//�v���p�e�B�z��
	vector<PmCipherProperty *> m_properties;
	//���t�@�C���p�X�ƓY�����̃}�b�v�i�����p�j
	hash_map<string, PmCipherProperty *> m_index;

//���J�֐�
public:
	//�R���X�g���N�^/�f�X�g���N�^
	_PmCipherPropertyList();
	~_PmCipherPropertyList();
	//�v���p�e�B���t�@�C���p�X�Ō���
	PmCipherProperty *FindProperty(string &source_path);
	//�v���p�e�B���擾
	unsigned int GetPropertySize();
	//�v���p�e�B���C���f�b�N�X�Ŏ擾
	PmCipherProperty *GetProperty(unsigned int num);
	//�v���p�e�B��ǉ�
	void AddProperty(PmCipherProperty *cipher_property);
	//�v���p�e�B���\�[�g
	void SortProperty();

//����J�֐�
protected:
	//�\�[�g�p��r�֐�
	static bool CompProperty(PmCipherProperty *a, PmCipherProperty *b);
};

//�Í��N���X
class _PmCipher
{
//�萔
public:
	//�ŏ����T�C�Y
	static const unsigned int CIPHER_MIN_KEY_SIZE;
	//�ő匮�T�C�Y
	static const unsigned int CIPHER_MAX_KEY_SIZE;
	//�o�b�t�@�̃T�C�Y ��8�̔{���ɂ��Ă��������B�Í����ƕ����͓����T�C�Y�ōs���Ă��������B
	static const unsigned int CIPHER_BUF_SIZE;
	//�Í����t�@�C���̎��ʎq ��8�̔{���ɂ��Ă��������B
	static const unsigned int CIPHER_KEY_WORD_SIZE;
	//�Í����t�@�C���̃w�b�_�T�C�Y
	static const unsigned int CIPHER_HEADER_SIZE;
	//���t�@�C�����Ƃ̏��̍ő�T�C�Y
	static const unsigned int CIPHER_MAX_INDEX_SIZE;

//�ϐ�
protected:
	//��
	unsigned char *m_key;
	unsigned int m_key_length;
	//�Í����N���X
	CBlowFish m_blow_fish;
	//�v���p�e�B���X�g
	PmCipherPropertyList *m_property_list;
	//�����������t���O
	bool m_decrypt_init;
	//���k�t���O
	bool m_compress;
	//�Í�����̃v���p�e�B�ꗗ�T�C�Y
	unsigned long m_cipher_index_size;
	//�Í�����̃t�@�C���T�C�Y
	unsigned long m_cipher_data_size;
	//�Í����t�@�C���X�g���[��
	fstream m_fs_decrypt;
	//�f�R�[�h���̃t�@�C���̈ʒu
	unsigned long m_current_offset;
	//�f�R�[�h���̈Í�����̃t�@�C���T�C�Y
	unsigned long m_current_cipher_size;
	//�f�R�[�h���̌��t�@�C���T�C�Y
	unsigned long m_current_source_size;
	//�f�R�[�h���̈��k�t�@�C���T�C�Y
	unsigned long m_current_compress_size;
	//�f�R�[�h���ɓǂݍ��񂾈Í����f�[�^�̃T�C�Y
	unsigned long m_total_buf_size;
	//�f�R�[�h���ɓǂݍ��񂾔񈳏k�f�[�^�̃T�C�Y
	unsigned int m_total_decomp_buf_size;
	//�f�R�[�h�Ŏg�p����o�̓o�b�t�@
	unsigned char *m_output_buf;
	//�f�R�[�h�Ŏg�p���鈳�k�f�[�^�̃o�b�t�@
	unsigned char *m_comp_input_buf;
	//�f�R�[�h���錳�t�@�C���̃p�X
	string m_cipher_path;
	//�G���R�[�h�����p�X
	string m_root_path;
	//ZLIB�̃X�g���[���N���X
	z_stream m_z;

//���J�֐�
public:
	//�R���X�g���N�^/�f�X�g���N�^
	_PmCipher();
	~_PmCipher();
	//���ݒ�y�я�����
	CIPHER_RESULT Init(unsigned char *key, unsigned int key_length);
	//�Í���
	CIPHER_RESULT Encrypt(const char *source_path, const char *cipher_path, bool compress, bool recursive = true);
	//�Í����t�@�C���̉��
	CIPHER_RESULT ParseCipherFile(const char *cipher_path);
	//���t�@�C���v���p�e�B�ꗗ�擾
	PmCipherPropertyList *GetPropertyList();
	//���[�g�f�B���N�g���擾
	string GetRootPath();
	//�f�R�[�h���錳�t�@�C���̎w��i�Í����t�@�C���̃I�[�v���Ȃ�
	CIPHER_RESULT DecryptInit(const char *source_path);
	//�f�R�[�h
	CIPHER_RESULT Decrypt(char **buf, unsigned int *size);
	//�f�R�[�h�̌�n���i�Í����t�@�C���̃N���[�Y�Ȃ�
	void DecryptEnd();

//����J�֐�
protected:
	//�t�@�C���̈Í���
	CIPHER_RESULT EncryptFile(const char *source_path, unsigned long file_size, bool compress, fstream *cipher_file, PmCipherProperty **cipher_property);
	//�e�L�X�g�i���̐����j��unsigned long�^�̐��l�ɕϊ�
	unsigned long AlphaToLong(const char *str, unsigned int size);
	bool Over2GB(unsigned long size);
};