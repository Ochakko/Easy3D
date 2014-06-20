#include <windows.h>
#include <stdio.h>
#include <imagehlp.h>
#include <string>
#include "PmCipher.h"

using namespace std;

int main(int argc, char *argv[])
{
	//************�������************
	//��1�����E�E�E�G���R�[�h/�f�R�[�h�w��(e/d)
	//�G���R�[�h�̏ꍇ
	//��2�����E�E�E�Í�������t�@�C��/�f�B���N�g���p�X
	//��3�����E�E�E�Í�����̃p�b�N�t�@�C���p�X�i�t�@�C�����w��A�f�B���N�g���s�j
	//��4�����E�E�E���k�t���O�i0/1)
	//��5�����E�E�E�ċA�����t���O�i0/1)
	//�f�R�[�h�̏ꍇ
	//��2�����E�E�E�Í�����̃p�b�N�t�@�C���p�X�i�t�@�C�����w��A�f�B���N�g���s�j
	//��3�����E�E�E���������t�@�C���̏o�͐�f�B���N�g���i�t�@�C�����s�j

	//����`
	unsigned char key[] = {0x01, 0x02, 0x03, 0x04}; //���ۂɂ͂����ƕ��G�Ȓl�ɂ��Ă��������B
	unsigned int key_length = 4;
	//�Í����N���X�ϐ��̒�`
	PmCipher cipher;

	if(argc < 2)
	{
		printf("�����Ȉ����ł��B\n");
		return 0;
	}

	//�G���R�[�h
	//e "Test" "Cipher\test.cph"
	if(*argv[1] == 'e')
	{
		CIPHER_RESULT cr;
		if(argc < 6)
		{
			printf("�����Ȉ����ł��B\n");
			return 0;
		}
		printf("�Í����J�n�B\n");
		//�������i�����̑�
		if((cr = cipher.Init(key, key_length)) != CIPHER_OK)
		{
			printf("�������I���R�[�h�F%d\n", cr);
			return 0;
		}
		//�Í���
		cr = cipher.Encrypt(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
		printf("�Í����I���R�[�h�F%d\n", cr);
	}
	//�f�R�[�h
	//d "Cipher\test.cph" "Cipher"
	else if(*argv[1] == 'd')
	{
		CIPHER_RESULT cr;
		if(argc < 4)
		{
			printf("�����Ȉ����ł��B\n");
			return 0;
		}
		if(PathFileExists(argv[3]))
		{
			if(!PathIsDirectory(argv[3]))
			{
				printf("�o�͐悪�f�B���N�g���ł͂���܂���B\n");
				return 0;
			}
		}
		else
		{
			string path = string(argv[3]) + string("\\");
			if(MakeSureDirectoryPathExists(path.c_str()))
			{
				printf("�f�B���N�g�����쐬���܂����B\n");
			}
			else
			{
				printf("�f�B���N�g���쐬�Ɏ��s���܂����B\n");
				return 0;
			}
		}

		char outdir[MAX_PATH];
		strcpy_s( outdir, MAX_PATH, argv[3] );

		printf("�����J�n�B\n");
		//�������i�����̑�
		if((cr = cipher.Init(key, key_length)) != CIPHER_OK)
		{
			printf("�������I���R�[�h�F%d\n", cr);
			return 0;
		}

		//�Í����t�@�C�����̉��
		cr = cipher.ParseCipherFile(argv[2]);
		printf("�p�[�X�I���R�[�h�F%d\n", cr);
		//�Í����t�@�C�����̎擾
		PmCipherPropertyList *list = cipher.GetPropertyList();


		char rootpath[MAX_PATH];
		strcpy_s( rootpath, MAX_PATH, cipher.GetRootPath().c_str() );
		printf("���[�g�p�X:%s\n", rootpath );


		//�Í����t�@�C���̐��������[�v
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
					::MessageBox( NULL, "�f�B���N�g���̍쐬�Ɏ��s���܂����B", "�G���[", MB_OK );
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

			//�f�R�[�h���錳�t�@�C���̎w��i�Í����t�@�C���̃I�[�v���Ȃ�
			cipher.DecryptInit(list->GetProperty(i)->GetPath().c_str());
			char *buf;
			unsigned int size;
			//���������f�[�^�̏o�͐�

			unsigned long writeleng;
			while((cr = cipher.Decrypt(&buf, &size)) != CIPHER_DEC_FINISH && size > 0)
			{
				//�o�b�t�@���o��
				WriteFile( hfile, buf, size, &writeleng, NULL );
			}
			//�Ō�̃o�b�t�@���o��
			if(size > 0)
			{
				WriteFile( hfile, buf, size, &writeleng, NULL );
			}

			//��n���i�Í����t�@�C���̃N���[�Y�Ȃ�
			cipher.DecryptEnd();

			FlushFileBuffers( hfile );
			SetEndOfFile( hfile );
			CloseHandle( hfile );
		}



/***
		printf("�����J�n�B\n");
		//�������i�����̑�
		if((cr = cipher.Init(key, key_length)) != CIPHER_OK)
		{
			printf("�������I���R�[�h�F%d\n", cr);
			return 0;
		}

		//�Í����t�@�C�����̉��
		cr = cipher.ParseCipherFile(argv[2]);
		printf("�p�[�X�I���R�[�h�F%d\n", cr);
		//�Í����t�@�C�����̎擾
		PmCipherPropertyList *list = cipher.GetPropertyList();
		printf("���[�g�p�X:%s\n", cipher.GetRootPath().c_str());
		//�Í����t�@�C���̐��������[�v
		for(unsigned int i = 0; list && i < list->GetPropertySize(); i++)
		{
			//�f�R�[�h���錳�t�@�C���̎w��i�Í����t�@�C���̃I�[�v���Ȃ�
			cipher.DecryptInit(list->GetProperty(i)->GetPath().c_str());
			char *buf;
			unsigned int size;
			//���������f�[�^�̏o�͐�
			fstream fs;
			fs.open(string(string(argv[3]) + string("\\") + list->GetProperty(i)->GetFileName()).c_str(), ios::out|ios::binary);
			//�f�[�^�̍Ō�܂Ń��[�v
			if(fs.is_open())
			{
				while((cr = cipher.Decrypt(&buf, &size)) != CIPHER_DEC_FINISH && size > 0)
				{
					//�o�b�t�@���o��
					fs.write(buf, size);
				}
				//�Ō�̃o�b�t�@���o��
				if(size > 0)
				{
					fs.write(buf, size);
				}
				//��n���i�Í����t�@�C���̃N���[�Y�Ȃ�
				cipher.DecryptEnd();
				fs << flush;
				fs.close();
			}
		}
***/
	}
	else
	{
		printf("�����Ȉ����ł��B\n");
	}
	printf("���������B\n");
	return 0;
}