/*
 * File name - GESound.cpp
 * Author - CuiXue
 * Updated date - 2013.1.27
 * Desc -
 *
 */
#include "GESound.h"

CGESound::CGESound()
{
	this->m_pBaseBuf = 0;
	this->m_pBaseBuf_3D = 0;
	this->m_pListener = 0;
	this->m_pDirectSound = 0;
}

CGESound::CGESound(const CGESound& source)
{
}

CGESound& CGESound::operator=(const CGESound& source)
{
	return *this;
}

CGESound::~CGESound()
{
}

bool CGESound::Initialize(HWND hWnd)
{
	bool result;

	result = this->_initialize_direct_sound(hWnd);
	if( !result )
		return false;

	return true;
}

bool CGESound::LoadAudioFile(char* _path, int audioID)
{
	bool result;

	char* pFilename = NULL;
	char* pType = NULL;

	for(int i = 0; i != this->m_vecSounds.GetMyVectorLength(); i++)
	{
		if( this->m_vecSounds[i].audioID == audioID )
			return false;
	}

	this->_audio_filename_recognition(_path, &pFilename, &pType);

	_Structure_Audio node;
	node.audioID = audioID;
	node.pAudioFilename = new char[128];
	if( !node.pAudioFilename )
		return false;

	node.pType = new char[128];
	if( !node.pType )
		return false;

	node.pLocation = new char[128];
	if( !node.pLocation )
		return false;

	char* tmp = pFilename;
	int len = strlen(tmp);
	strcpy_s(node.pAudioFilename, len + 1, pFilename);

	tmp = pType;
	len = strlen(tmp);
	strcpy_s(node.pType, len + 1, pType);

	tmp = _path;
	len = strlen(tmp);
	strcpy_s(node.pLocation, len + 1, _path);

	result = this->_load_wav_audio_file(_path, &node.pData);
	if( !result )
		return false;

	this->m_vecSounds.Pushback(node);

	node.pLocation = NULL;
	node.pAudioFilename = NULL;
	node.pType = NULL;
	
	return true;
}

bool CGESound::LoadAudioFile(char* _path, int audioID, GE_SOUND_VOLUME_TYPE soundVolumeType)
{
	if( soundVolumeType == GE2D_SOUND )
	{
		bool result;

		char* pFilename = NULL;
		char* pType = NULL;

		for(int i = 0; i != this->m_vecSounds.GetMyVectorLength(); i++)
		{
			if( this->m_vecSounds[i].audioID == audioID )
				return false;
		}

		this->_audio_filename_recognition(_path, &pFilename, &pType);

		_Structure_Audio node;
		node.audioID = audioID;
		node.pAudioFilename = new char[128];
		if( !node.pAudioFilename )
			return false;

		node.pType = new char[128];
		if( !node.pType )
			return false;

		node.pLocation = new char[128];
		if( !node.pLocation )
			return false;

		char* tmp = pFilename;
		int len = strlen(tmp);
		strcpy_s(node.pAudioFilename, len + 1, pFilename);

		tmp = pType;
		len = strlen(tmp);
		strcpy_s(node.pType, len + 1, pType);

		tmp = _path;
		len = strlen(tmp);
		strcpy_s(node.pLocation, len + 1, _path);

		result = this->_load_wav_audio_file(_path, &node.pData);
		if( !result )
			return false;

		this->m_vecSounds.Pushback(node);

		node.pLocation = NULL;
		node.pAudioFilename = NULL;
		node.pType = NULL;

		return true;
	}

	if( soundVolumeType == GE3D_SOUND )
	{
		bool result;

		char* pFilename = NULL;
		char* pType = NULL;

		for(int i = 0; i != this->m_vec3DSounds.GetMyVectorLength(); i++)
		{
			if( this->m_vec3DSounds[i].audioID == audioID )
				return false;
		}

		this->_audio_filename_recognition(_path, &pFilename, &pType);


		_Structure_Audio_3D node;
		node.audioID = audioID;
		node.pAudioFilename = new char[128];
		if( !node.pAudioFilename )
			return false;

		node.pType = new char[128];
		if( !node.pType )
			return false;

		node.pLocation = new char[128];
		if( !node.pLocation )
			return false;

		char* tmp = pFilename;
		int len = strlen(tmp);
		strcpy_s(node.pAudioFilename, len + 1, pFilename);

		tmp = pType;
		len = strlen(tmp);
		strcpy_s(node.pType, len + 1, pType);

		tmp = _path;
		len = strlen(tmp);
		strcpy_s(node.pLocation, len + 1, _path);

		result = this->_load_wav_audio_file(_path, &node.pData, &node.p3DProperties);
		if( !result )
			return false;
		
		this->m_vec3DSounds.Pushback(node);

		node.pLocation = NULL;
		node.pAudioFilename = NULL;
		node.pType = NULL;

		return true;
	}

	return true;
}

bool CGESound::Play(int audioID)
{
	bool result;

	result = this->_play(audioID);
	if( !result )
		return false;

	return true;
}

bool CGESound::Play(GE_SOUND_VOLUME_TYPE soundVolumeType, int audioID)
{
	bool result;
	
	if( soundVolumeType == GE2D_SOUND )
	{
		result = this->_play(audioID);
		if( !result )
			return false;
	}

	if( soundVolumeType == GE3D_SOUND )
	{
		result = this->_play3DSound(audioID);
		if( !result )
			return false;
	}

	return true;
}

void CGESound::Shutdown()
{
	for(int i = 0; i != this->m_vecSounds.GetMyVectorLength(); i++)
	{
		this->_shutdown_audio_file(&this->m_vecSounds[i].pData);
	}

	for(int i = 0; i != this->m_vec3DSounds.GetMyVectorLength(); i++)
	{
		this->_shutdown_audio_file(&this->m_vec3DSounds[i].pData);
	}

	this->_shutdown();
	this->m_vec3DSounds.Reset();
	this->m_vecSounds.Reset();
}

bool CGESound::_initialize_direct_sound(HWND hWnd)
{
	HRESULT result;

	DSBUFFERDESC sound3DBufDesc;
	WAVEFORMATEX wave3DFormat;

	result = ::DirectSoundCreate8(NULL, &this->m_pDirectSound, NULL);
	if( FAILED(result) )
		return false;

	result = this->m_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if( FAILED(result) )
		return false;

	// Fill out buffer description structure.
	// ...Working...
	sound3DBufDesc.dwSize = sizeof(DSBUFFERDESC);
	sound3DBufDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	sound3DBufDesc.dwBufferBytes = 0;
	sound3DBufDesc.dwReserved = 0;
	sound3DBufDesc.lpwfxFormat = NULL;
	sound3DBufDesc.guid3DAlgorithm = GUID_NULL;

	result = this->m_pDirectSound->CreateSoundBuffer(&sound3DBufDesc, &this->m_pBaseBuf_3D, NULL);
	if( FAILED(result) )
		return false;

	wave3DFormat.wFormatTag = WAVE_FORMAT_PCM;
	wave3DFormat.nSamplesPerSec = 44100;
	wave3DFormat.wBitsPerSample = 16;
	wave3DFormat.nChannels = 1;
	wave3DFormat.nBlockAlign = (wave3DFormat.wBitsPerSample / 8 ) * wave3DFormat.nChannels;
	wave3DFormat.nAvgBytesPerSec = wave3DFormat.nSamplesPerSec * wave3DFormat.nBlockAlign;
	wave3DFormat.cbSize = 0;

	result = this->m_pBaseBuf_3D->SetFormat(&wave3DFormat);
	if( FAILED(result) )
		return false;

	result = this->m_pBaseBuf_3D->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&this->m_pListener);
	if( FAILED(result) )
		return false;

	return true;
}

void CGESound::_shutdown()
{
	::ReleaseWithoutDel<IDirectSoundBuffer*>(this->m_pBaseBuf);
	::ReleaseWithoutDel<IDirectSoundBuffer*>(this->m_pBaseBuf_3D);

	for(int i = 0; i != this->m_vecSounds.GetMyVectorLength(); i++)
	{
		::ReleaseWithoutDel<IDirectSoundBuffer8*>(this->m_vecSounds[i].pData);
	}

	for(int i = 0; i != this->m_vec3DSounds.GetMyVectorLength(); i++)
	{
		::ReleaseWithoutDel<IDirectSoundBuffer8*>(this->m_vec3DSounds[i].pData);
		::ReleaseWithoutDel<IDirectSound3DBuffer8*>(this->m_vec3DSounds[i].p3DProperties);
	}
}

bool CGESound::_load_wav_audio_file(char* audioFilename, IDirectSoundBuffer8 **ppAudioBuffer)
{
	int error;
	FILE* pAudioFile = NULL;
	unsigned int iCount;
	WaveHeaderTypeExt waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC audioBufferDesc;
	HRESULT result;
	IDirectSoundBuffer* pTempBuffer = NULL;
	unsigned char* pWaveData = NULL;
	unsigned char* pBuffer = NULL;
	unsigned long bufferSize;

	error = fopen_s(&pAudioFile, audioFilename, "rb");
	if( 0 != error )
		return false;

	iCount = fread(&waveFileHeader, sizeof(waveFileHeader), 1, pAudioFile);
	if( 1 != iCount )
		return false;

	if( waveFileHeader.chunkID[0] != 'R' || waveFileHeader.chunkID[1] != 'I' ||
		waveFileHeader.chunkID[2] != 'F' || waveFileHeader.chunkID[3] != 'F' )
		return false;

	if( waveFileHeader.format[0] != 'W' || waveFileHeader.format[1] != 'A' ||
		waveFileHeader.format[2] != 'V' || waveFileHeader.format[3] != 'E' )
		return false;

	if( waveFileHeader.subChunkId[0] != 'f' || waveFileHeader.subChunkId[1] != 'm' ||
		waveFileHeader.subChunkId[2] != 't' || waveFileHeader.subChunkId[3] != ' ' )
		return false;

	if( waveFileHeader.audioFormat != WAVE_FORMAT_PCM )
		return false;

	if( waveFileHeader.numChannels != 2 )
		return false;

	if( waveFileHeader.sampleRate != 44100 )
		return false;

	if( waveFileHeader.bitsPerSample != 16 )
		return false;

	if( waveFileHeader.dataChunkId[0] != 'd' || waveFileHeader.dataChunkId[1] != 'a' ||
		waveFileHeader.dataChunkId[2] != 't' || waveFileHeader.dataChunkId[3] != 'a' )
		return false;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	audioBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	audioBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	audioBufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	audioBufferDesc.dwReserved = 0;
	audioBufferDesc.lpwfxFormat = &waveFormat;
	audioBufferDesc.guid3DAlgorithm = GUID_NULL;

	result = this->m_pDirectSound->CreateSoundBuffer(&audioBufferDesc, &pTempBuffer, NULL);
	if( FAILED(result) )
		return false;

	result = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*ppAudioBuffer);
	
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<IDirectSoundBuffer*>(pTempBuffer);

	fseek(pAudioFile, sizeof(waveFileHeader), SEEK_SET);

	pWaveData = new unsigned char[waveFileHeader.dataSize];
	if( !pWaveData )
		return false;

	iCount = fread(pWaveData, 1, waveFileHeader.dataSize, pAudioFile);
	if( iCount != waveFileHeader.dataSize )
		return false;

	error = fclose(pAudioFile);
	if( 0 != error )
		return false;

	result = (*ppAudioBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&pBuffer, (DWORD*)&bufferSize, NULL, 0, 0);
	if( FAILED(result) )
		return false;

	memcpy(pBuffer, pWaveData, waveFileHeader.dataSize);

	result = (*ppAudioBuffer)->Unlock((void*)pBuffer, bufferSize, NULL, 0);
	if( FAILED(result) )
		return false;

	::DeleteArray<unsigned char*>(pWaveData);

	return true;
}

bool CGESound::_load_wav_audio_file(char* audioFilename, IDirectSoundBuffer8 **ppAudioBuffer, IDirectSound3DBuffer8** ppSound3DProperties)
{
	int error;
	FILE* pAudioFile = NULL;
	unsigned int iCount;
	WaveHeaderTypeExt waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC audioBufferDesc;
	HRESULT result;
	IDirectSoundBuffer* pTempBuffer = NULL;
	unsigned char* pWaveData = NULL;
	unsigned char* pBuffer = NULL;
	unsigned long bufferSize;

	error = fopen_s(&pAudioFile, audioFilename, "rb");
	if( 0 != error )
		return false;

	iCount = fread(&waveFileHeader, sizeof(waveFileHeader), 1, pAudioFile);
	if( 1 != iCount )
		return false;

	if( waveFileHeader.chunkID[0] != 'R' || waveFileHeader.chunkID[1] != 'I' ||
		waveFileHeader.chunkID[2] != 'F' || waveFileHeader.chunkID[3] != 'F' )
		return false;

	if( waveFileHeader.format[0] != 'W' || waveFileHeader.format[1] != 'A' ||
		waveFileHeader.format[2] != 'V' || waveFileHeader.format[3] != 'E' )
		return false;

	if( waveFileHeader.subChunkId[0] != 'f' || waveFileHeader.subChunkId[1] != 'm' ||
		waveFileHeader.subChunkId[2] != 't' || waveFileHeader.subChunkId[3] != ' ' )
		return false;

	if( waveFileHeader.audioFormat != WAVE_FORMAT_PCM )
		return false;

	if( waveFileHeader.numChannels != 1 )
		return false;

	if( waveFileHeader.sampleRate != 44100 )
		return false;

	if( waveFileHeader.bitsPerSample != 16 )
		return false;

	if( waveFileHeader.dataChunkId[0] != 'd' || waveFileHeader.dataChunkId[1] != 'a' ||
		waveFileHeader.dataChunkId[2] != 't' || waveFileHeader.dataChunkId[3] != 'a' )
		return false;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	audioBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	audioBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	audioBufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	audioBufferDesc.dwReserved = 0;
	audioBufferDesc.lpwfxFormat = &waveFormat;
	audioBufferDesc.guid3DAlgorithm = GUID_NULL;

	result = this->m_pDirectSound->CreateSoundBuffer(&audioBufferDesc, &pTempBuffer, NULL);
	if( FAILED(result) )
		return false;

	result = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*ppAudioBuffer);
	
	if( FAILED(result) )
		return false;

	::ReleaseWithoutDel<IDirectSoundBuffer*>(pTempBuffer);

	fseek(pAudioFile, sizeof(waveFileHeader), SEEK_SET);

	pWaveData = new unsigned char[waveFileHeader.dataSize];
	if( !pWaveData )
		return false;

	iCount = fread(pWaveData, 1, waveFileHeader.dataSize, pAudioFile);
	if( iCount != waveFileHeader.dataSize )
		return false;

	error = fclose(pAudioFile);
	if( 0 != error )
		return false;

	result = (*ppAudioBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&pBuffer, (DWORD*)&bufferSize, NULL, 0, 0);
	if( FAILED(result) )
		return false;

	memcpy(pBuffer, pWaveData, waveFileHeader.dataSize);

	result = (*ppAudioBuffer)->Unlock((void*)pBuffer, bufferSize, NULL, 0);
	if( FAILED(result) )
		return false;

	::DeleteArray<unsigned char*>(pWaveData);

	result = (*ppAudioBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*ppSound3DProperties);
	if( FAILED(result) )
		return false;
	
	return true;
}

/*
 * Funciton name  - AudioFileIdentify
 * Params - (in)	char*	pPath
			(out)	char**	_name
			(out)	char**  _type
 * Author - CuiXue
 * Version - 1.0
 * Desc - 识别音频文件名字以及类型
 */
bool CGESound::_audio_filename_recognition(char* pPath, char** _name, char** _type)
{
	if( NULL == pPath )
		return false;

	*_name = NULL;
	*_type = NULL;

	char* ptr = pPath;
	int len = strlen(ptr);
	int i = 0;

	while( *ptr != '\0' && i != len )
	{
		if( *ptr == '.' && ((i + 3) != len) )
		{
			if( *(ptr + 1) == 'w' && *(ptr + 2) == 'a' && *(ptr + 3) == 'v' )
			{
				int curLen = strlen(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					char *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}

				return true;
			}

			if( *(ptr + 1) == 'm' && *(ptr + 2) == 'p' && *(ptr + 3) == '3' )
			{
				int curLen = strlen(ptr);

				if( curLen != 0 )
				{
					*_type = ptr;
					char *tmpName = ptr;


					while( *tmpName != '/' )
					{
						tmpName--;
					}

					*_name = tmpName + 1;

					tmpName = NULL;
					ptr = NULL;
				}
				return true;
			}
		}

		i++;
		ptr++;
	}

	return true;
}

bool CGESound::_load_mp3_audio_file(char* audioFilename, IDirectSoundBuffer8 **ppAudioBuffer)
{
	return true;
}

void CGESound::_shutdown_audio_file(IDirectSoundBuffer8** ppAudioBuffer)
{
	::ReleaseWithoutDel<IDirectSoundBuffer8*>(*ppAudioBuffer);
}

bool CGESound::_play(int ID)
{
	HRESULT result;

	int _count = 0;
	while( ID != this->m_vecSounds[_count].audioID )
		_count++;

	if( _count == this->m_vecSounds.GetMyVectorLength() )
		return false;

	result = this->m_vecSounds[_count].pData->SetCurrentPosition(0);
	if( FAILED(result) )
		return false;

	result = this->m_vecSounds[_count].pData->SetVolume(DSBVOLUME_MAX);
	if( FAILED(result) )
		return false;

	result = this->m_vecSounds[_count].pData->Play(0, 0, DSBPLAY_LOOPING);
	if( FAILED(result) )
		return false;

	return true;
}

bool CGESound::_play3DSound(int audioID)
{
	HRESULT result;
	float soundPosX, soundPosY, soundPosZ;
	int _count = 0;

	while( audioID != this->m_vec3DSounds[_count].audioID )
		_count++;

	if( _count == this->m_vec3DSounds.GetMyVectorLength() )
		return false;

	soundPosX = this->m_vec3DSounds[_count].position.x;
	soundPosY = this->m_vec3DSounds[_count].position.y;
	soundPosZ = this->m_vec3DSounds[_count].position.z;

	result = this->m_vec3DSounds[_count].pData->SetCurrentPosition(0);
	if( FAILED(result) )
		return false;

	result = this->m_vec3DSounds[_count].pData->SetVolume(DSBVOLUME_MAX);
	if( FAILED(result) )
		return false;

	this->m_vec3DSounds[_count].p3DProperties->SetPosition(soundPosX, soundPosY, soundPosZ, DS3D_IMMEDIATE);

	result = this->m_vec3DSounds[_count].pData->Play(0, 0, DSBPLAY_LOOPING);
	if( FAILED(result) )
		return false;

	return true;	
}

void CGESound::Set3DSoundPosition(int audioID, float x, float y, float z)
{
	if( 0 > audioID )
		return;

	for(int i = 0; i != this->m_vec3DSounds.GetMyVectorLength(); i++)
	{
		if( audioID == this->m_vec3DSounds[i].audioID )
		{
			this->m_vec3DSounds[i].position.x = x;
			this->m_vec3DSounds[i].position.y = y;
			this->m_vec3DSounds[i].position.z = z;

			return;
		}
	}

#ifdef _WIN32
	MessageBox(NULL, L"Can't find the audio", L"Error", MB_OK);
#endif

	return;
}

void CGESound::SetListenerPosition(float x, float y, float z)
{
	if( !this->m_pListener )
		return;

	this->m_pListener->SetPosition(x, y, z, DS3D_IMMEDIATE);
}



