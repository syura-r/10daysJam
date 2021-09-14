#include "Audio.h"
#include<fstream>
#include<cassert>
ComPtr<IXAudio2> Audio::xAudio2 = {};
IXAudio2MasteringVoice* Audio::masterVoice = {};
std::map < std::string, IXAudio2SourceVoice* > Audio::soundVoices = {};
XAudio2VoiceCallback voiceCallback = {};
std::map<std::string, File> Audio::soundFiles = {};

Audio::Audio()
{
}


Audio::~Audio()
{
}

void Audio::Initialize()
{
	HRESULT result;
	//XAudioエンジンのインスタンスを作成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//マスターボイスを作成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
}

void Audio::LoadFile(const std::string& keyName, const std::string& fileName)
{

	File waveFile;

	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//.wavファイルをバイナリモードで開く
	std::string str = "Resources/Sound/" + fileName;
	const char* filename = str.c_str();
	file.open(filename, std::ios_base::binary);

	//ファイルオープン失敗を検出する
	if (file.fail())assert(0);

	//RIFFヘッダーの読み込み
	file.read((char*)&waveFile.riffheader, sizeof(waveFile.riffheader));
	//ファイルがRIFFかチェック
	if (strncmp(waveFile.riffheader.chunk.id, "RIFF", 4) != 0)assert(0);

	//Formatチャンクの読み込み
	file.read((char*)&waveFile.fChunk, sizeof(waveFile.fChunk));
	if (strncmp(waveFile.fChunk.chunk.id, "fmt ", 4) != 0)assert(0);

	//Dataチャンクの読み込み
	file.read((char*)&waveFile.chunk, sizeof(waveFile.chunk));
	if (strncmp(waveFile.chunk.id, "data", 4) != 0)assert(0);

	//Dataチャンクのデータ部(波形データ)の読み込み
	waveFile.buff = new char[waveFile.chunk.size];
	file.read(waveFile.buff, waveFile.chunk.size);

	//Waveファイルを閉じる
	file.close();

	soundFiles.emplace(keyName, waveFile);

}

void Audio::PlayWave(const std::string& keyName, const float& soundVol, bool loop, int loopCount)
{
	HRESULT result;

	//サウンド再生
	WAVEFORMATEX wfex{};

	if (soundVoices[keyName] != nullptr && loop)
		return;

	//波形フォーマットの設定
	memcpy(&wfex, &soundFiles[keyName].fChunk.fmt, sizeof(soundFiles[keyName].fChunk.fmt));
	wfex.wBitsPerSample = soundFiles[keyName].fChunk.fmt.nBlockAlign * 8 / soundFiles[keyName].fChunk.fmt.nChannels;

	//波形フォーマットを元にSourceVoiceの再生
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex);
	if FAILED(result)
	{
		delete[] soundFiles[keyName].buff;
		return;
	}
	pSourceVoice->SetVolume(soundVol);
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)soundFiles[keyName].buff;
	buf.pContext = soundFiles[keyName].buff;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = soundFiles[keyName].chunk.size;
	if (loop)
	{
		buf.LoopBegin = 0;
		buf.LoopLength = 0;
		buf.LoopCount = loopCount;
	}

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();

	soundVoices[keyName] = pSourceVoice;
}

void Audio::StopWave(const std::string& keyName)
{
	if (soundVoices[keyName] == nullptr)
		return;
	soundVoices[keyName]->Stop();
	soundVoices[keyName]->DestroyVoice();
	soundVoices.erase(keyName);

}

void Audio::End()
{
	masterVoice->DestroyVoice();
	if (xAudio2.Get() != nullptr)xAudio2->StopEngine();
	for (auto itr = soundFiles.begin(); itr != soundFiles.end(); itr++)
	{
		delete[](*itr).second.buff;
	}
}