#pragma once
#include<DirectXMath.h>

class PointLight
{
private:
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public://�o�b�t�@�p�f�[�^
	struct ConstBuff
	{
		XMFLOAT3 lightpos;
		float pad1;//�p�f�B���O
		XMFLOAT3 lightcolor;
		float pad2;//�p�f�B���O
		XMFLOAT3 lightatten;
		UINT active;
	};
public://�����o�֐�
	inline void SetLightPos(const XMFLOAT3& pos){
		this->lightpos = pos;}
	inline const XMFLOAT3& GetLightPos(){ return lightpos; }

	inline void SetLightColor(const XMFLOAT3&  color) {
		this->lightcolor = color;}
	inline const XMFLOAT3& GetLightColor() { return lightcolor; }

	inline void SetLightAtten(const XMFLOAT3& atten) {
		this->lightAtten = atten;
	}
	inline const XMFLOAT3& GetLightAtten() { return lightAtten; }
	inline void SetActive(bool active) { this->active = active; }
	inline bool IsActive() { return active; }

private://�����o�ϐ�
	XMFLOAT3 lightpos = { 0,0,0 };//���C�g���W(���[���h���W�n)
	XMFLOAT3 lightcolor = { 1,1,1 };//���C�g�̐F
	XMFLOAT3 lightAtten = { 1.0f,1.0f,1.0f };//���C�g�̋��������W��
	bool active = false;
};
