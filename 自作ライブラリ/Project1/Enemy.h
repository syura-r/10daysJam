#pragma once
#include "Object.h"
class Enemy :
	public Object
{
public:
	Enemy(const Vector3& pos);
	void Update() override;
	void Draw() override;
	bool IsDelete() { return isDelete; }
private:
	void CreateConstBuff();
	struct ConstBuffData
	{
		float _Destruction; //分解度合い
		float _ScaleFactor; //スケールの変化量
		float _PositionFactor;//ポジションの変化量
		float _RotationFactor;//回転の変化量
		int _Tessellation;//ポリゴン分割度
	};
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	float destruction;
	float scaleFactor; //スケールの変化量
	float positionFactor;//ポジションの変化量
	float rotationFactor;//回転の変化量
	int  tessellation;//ポリゴン分割度

	//死亡フラグ
	bool isDead = false;
	//死亡アニメーション終了フラグ
	bool isDelete = false;

	const float breakSpeed = 0.05f;
};



