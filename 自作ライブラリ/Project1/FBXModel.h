#pragma once
#include<string>
#include<DirectXMath.h>
#include"Model.h"
#include<vector>
#include<DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <fbxsdk.h>
struct Node
{
	//名前
	std::string name;
	//ローカルスケール
	DirectX::XMVECTOR scaling = { 1,1,1,0 };
	//ローカル回転角
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
	//ローカル移動
	DirectX::XMVECTOR translation = { 0,0,0,1 };
	//ローカル変形行列
	DirectX::XMMATRIX transform;
	//グローバル変形行列
	DirectX::XMMATRIX globalTransform;
	//親ノード
	Node* parent = nullptr;
};
class FBXModel:public Model
{
private://エイリアス
	//Microsoft::WRL::省略
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	//std::を省略
	using string = std::string;
	template <class T>using vector = std::vector<T>;
public:
	//フレンドクラス
	friend class FbxLoader;
	struct Bone
	{
		std::string name;
		DirectX::XMMATRIX invInitialPose;
		FbxCluster* fbxCluster;
		Bone(const std::string& name)
		{
			this->name = name;
		}
	};

private:
	void CreateBuffers();
	//モデル名
	string name;
	//ノード配列
	vector<Node> nodes;
	//アンビエント係数
	XMFLOAT3 ambient = { 1,1,1 };
	//ディフューズ係数
	XMFLOAT3 diffuse = { 1,1,1 };
	//テクスチャメタデータ
	//TexMetadata metadata = {};
	//スクラッチイメージ
	ScratchImage scratchImg = {};
	//頂点バッファ
	ComPtr<ID3D12Resource>vertBuff;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//テクスチャバッファ
	//ComPtr<ID3D12Resource>texBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV用デスクリプタヒープ
	//ComPtr<ID3D12DescriptorHeap> descHeapSRV;
	//テクスチャ名
	string texName;
	//ボーンの配列
	std::vector<Bone> bones;
//FBXシーン
	FbxScene* fbxScene = nullptr;
public://メンバ関数
	~FBXModel();
	//描画
	void Draw()override;
	//getter
	std::vector <Bone> &GetBones() { return bones; }
	//ボーンインデックスの最大数
	static const int MAX_BONE_INDICES = 4;
	//モデルの変形行列取得
	const XMMATRIX& GetModelTransform() { return meshNode->globalTransform; }
	//getter
	FbxScene* GetFbxScene() { return fbxScene; }
		//ボーンの最大数
	static const int MAX_BONES = 32;
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};
	/// <summary>
	/// アニメーション開始
	/// </summary>
	void AnimationInit();

	void SetAnimationFrame(const int startFrame, const int endFrame,const int FrameTime = 1);

	bool PlayAnimation(bool endless = false);

	inline const FbxTime& GetCurrentAnimationTime() { return currentTime; }

private:
	ComPtr<ID3D12Resource> constBuffSkin; // 定数バッファ
		//1フレームの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメーション終了時間
	FbxTime endTime;
	//現在時間(アニメーション)
	FbxTime currentTime;
	//アニメーション再生中
	bool isPlay = false;

public://サブクラス
	//頂点データ構造体
	struct VertexPosNormalUvSkin
	{
		XMFLOAT3 pos;	 //位置
		XMFLOAT3 normal;//法線ベクトル
		XMFLOAT2 uv;    //uv座標
		UINT boneIndex[MAX_BONE_INDICES];//番号
		float boneWeight[MAX_BONE_INDICES];//重み
	};

	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	vector<VertexPosNormalUvSkin> vertices;
	//頂点インデックス配列
	vector<unsigned short>indices;

};

