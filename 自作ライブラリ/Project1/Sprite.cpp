#include "Sprite.h"
#include<math.h>
#include<d3dx12.h>
#include"Texture.h"
Window* Sprite::window = nullptr;
XMMATRIX Sprite::spriteMatProjection = {};
Sprite::Sprite()
{
	CreateSprite();
}

Sprite::~Sprite()
{
}

void Sprite::StaticInitialize(Window * window)
{
	Sprite::window = window;
	// 射影行列計算
	spriteMatProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)window->GetWindowWidth(),
		(float)window->GetWindowHeight(), 0.0f,
		0.0f, 1.0f);
}

void Sprite::CreateSprite()
{
	HRESULT result;
	vertices[0].uv = { 0.0f,1.0f };
	vertices[1].uv = { 0.0f,0.0f };
	vertices[2].uv = { 1.0f,1.0f };
	vertices[3].uv = { 1.0f,0.0f };

	auto dev = DirectXLib::GetInstance()->GetDevice();
	
	//頂点バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));
	
	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	//定数バッファの作成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDate) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

}

void Sprite::DrawSprite(const std::string & name, const XMFLOAT2 & position, const float & rotation, const XMFLOAT2 & scale, const XMFLOAT4 & color, const XMFLOAT2 & anchorpoint, BLENDTYPE type)
{
	auto cmdList = DirectXLib::GetInstance()->GetCommandList();
	
	D3D12_RESOURCE_DESC resDesc = Texture::GetTexBuff(name).Get()->GetDesc();
	float width = (float)resDesc.Width; //画像の横幅
	float height = (float)resDesc.Height; //画像の縦幅
	HRESULT result;

	spriteRotation = rotation;
	spritePosition = XMMatrixTranslation(position.x, position.y, 0);
	spriteMatScale = XMMatrixScaling(scale.x, scale.y, 1);

	if (rect)
	{
		vertices[0].pos = { (0.0f - anchorpoint.x) *rectTexSize.x,(1.0f - anchorpoint.y)*rectTexSize.y,0.0f };
		vertices[1].pos = { (0.0f - anchorpoint.x) *rectTexSize.x,(0.0f - anchorpoint.y)*rectTexSize.y,0.0f };
		vertices[2].pos = { (1.0f - anchorpoint.x) *rectTexSize.x,(1.0f - anchorpoint.y)*rectTexSize.y,0.0f };
		vertices[3].pos = { (1.0f - anchorpoint.x) *rectTexSize.x,(0.0f - anchorpoint.y)*rectTexSize.y,0.0f };
	}
	else
	{
		vertices[0].pos = { (0.0f - anchorpoint.x) * width,(1.0f - anchorpoint.y) * height,0.0f };
		vertices[1].pos = { (0.0f - anchorpoint.x) * width,(0.0f - anchorpoint.y) * height,0.0f };
		vertices[2].pos = { (1.0f - anchorpoint.x) * width,(1.0f - anchorpoint.y) * height,0.0f };
		vertices[3].pos = { (1.0f - anchorpoint.x) * width,(0.0f - anchorpoint.y) * height,0.0f };
	}
	VERTEX* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);

	//ワールド行列の更新
	spriteMatWorld = XMMatrixIdentity();
	spriteMatWorld *= spriteMatScale;
	spriteMatWorld *= XMMatrixRotationZ(XMConvertToRadians(spriteRotation));
	spriteMatWorld *= spritePosition;

	ConstBufferDate* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = spriteMatWorld * spriteMatProjection;//行列の合成
	constMap->color = color;
	constBuff->Unmap(0, nullptr);

	PipelineState::SetPipeline("Sprite",type);
	//DirectXLib::GetCommandList()->SetPipelineState(PipelineState::GetPipelinestate2D(type).Get());
	//DirectXLib::GetCommandList()->SetGraphicsRootSignature(PipelineState::GetRootsugnature2D().Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::GetBasicDescHeap().Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//インデックスバッファのセットコマンド
	cmdList->SetGraphicsRootDescriptorTable(1, Texture::GetGpuDescHandleSRV(name));  //ヒープの先頭が定数バッファ

	cmdList->DrawInstanced(4, 1, 0, 0);

}

void Sprite::SpriteSetTextureRect(const std::string & name, const float & tex_x, const float & tex_y, const float & tex_width, const float & tex_height)
{
	HRESULT result;

	D3D12_RESOURCE_DESC resDesc = Texture::GetTexBuff(name).Get()->GetDesc();

	float tex_left = tex_x / resDesc.Width;
	float tex_right = (tex_x + tex_width) / resDesc.Width;
	float tex_top = tex_y / resDesc.Height;
	float tex_bottom = (tex_y + tex_height) / resDesc.Height;

	vertices[0].uv = { tex_left,tex_bottom };
	vertices[1].uv = { tex_left,tex_top };
	vertices[2].uv = { tex_right,tex_bottom };
	vertices[3].uv = { tex_right,tex_top };

	rectTexSize.x = tex_width;
	rectTexSize.y = tex_height;


	rect = true;
}
