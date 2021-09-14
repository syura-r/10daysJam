#pragma once
#include "Object.h"
class GSShaderTestObj :
    public Object
{
public:
	GSShaderTestObj(const Vector3& pos);
	void Draw() override;
private:
	void CreateConstBuff();
	struct ConstBuffData
	{
		//XMFLOAT4 status;
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
};

