/*
 * File name - GESound.h
 * Author - CuiXue
 * Updated date - 2013.1.27
 * Desc -
 *
 * Modifying...
 *
 * 正在重新设计，将原本只能保存一首歌曲的 AudioBuffer 更改为
 * 自行设计的 MyVector 容器元素，实现声音缓存序列化，方便多首
 * 音乐的查找与播放（考虑实际应用中，特别是游戏中，不会经常对
 * 歌曲进行添加和删除，所以才用了顺序存储容器）。
 *
 * 支持的格式： .wav | .mp3
 */
#ifndef _GE_SOUND_H_
#define _GE_SOUND_H_

#ifdef _WIN32
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include "Utility.h"
#endif

// 1.Background music
// 2.Scenary effects sound
// enum GE_SOUND_TYPE
// {
//	  GE_MUSIC,
//	  GE_EFFECT_SOUND,
// };

// 1.2D Sound
// 2.3D Sound
enum GE_SOUND_VOLUME_TYPE
{
	GE2D_SOUND,
	GE3D_SOUND,
};

/*
 * Class name - CGESound
 * Author - CuiXue
 * Updated date - 2013.1.27
 * Desc -
 */
class CGESound
{
private:
	// 2D Audio data structure.
	typedef struct _ge_sound_type
	{
	public:
#ifdef __cplusplus
		_ge_sound_type(){
			audioID = -1;
			pLocation = NULL;
			pAudioFilename = NULL;
			pType = NULL;
			pData = 0;
		}
#endif

		int	  audioID;							// 不同声音文件的识别标示符	
		char* pLocation;						// 声音文件位置
		char* pAudioFilename;					// 声音文件名
		char* pType;							// 声音文件类型
		IDirectSoundBuffer8* pData;				// 数据
	}_Structure_Audio, *_pStructure_Audio;

	// 3D Audio data structure.
	typedef struct _get_3d_sound_type
	{
#ifdef __cplusplus
#endif

		int audioID;
		char* pLocation;
		char* pAudioFilename;
		char* pType;

		D3DXVECTOR3 position;
		IDirectSoundBuffer8* pData;
		IDirectSound3DBuffer8* p3DProperties;
	}_Structure_Audio_3D, *_pStructure_Audio_3D;

public:
	// Interfaces
	CGESound();
	CGESound(const CGESound&);
	CGESound& operator=(const CGESound&);
	~CGESound();

public:
	// Functions interfaces
	bool Initialize(HWND);
	bool LoadAudioFile(char*, int audioID);
	bool LoadAudioFile(char* ,int audioID, GE_SOUND_VOLUME_TYPE);
	void SetListenerPosition(float, float, float);
	D3DXVECTOR4 GetSoundPosition(int);
	void Set3DSoundPosition(int, float, float, float);
	bool Play(GE_SOUND_VOLUME_TYPE, int);
	bool Play(int);
	void Shutdown();

private:
	// Kernels
	bool _initialize_direct_sound(HWND);
	void _shutdown();

	bool _load_wav_audio_file(char*, IDirectSoundBuffer8**);
	bool _load_wav_audio_file(char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	bool _load_mp3_audio_file(char*, IDirectSoundBuffer8**);
	void _shutdown_audio_file(IDirectSoundBuffer8**);
	bool _audio_filename_recognition(char* pPath, char** _name, char** _type);
	bool _play3DSound(int);
	bool _play(int);

private:
	// Var members
	IDirectSound8* m_pDirectSound;
	IDirectSoundBuffer* m_pBaseBuf;
	IDirectSoundBuffer* m_pBaseBuf_3D;
	IDirectSound3DListener8* m_pListener;

	MyVector<_Structure_Audio_3D> m_vec3DSounds;
	MyVector<_Structure_Audio> m_vecSounds;
};

#endif