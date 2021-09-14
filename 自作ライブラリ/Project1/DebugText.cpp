#include "DebugText.h"
#include"PtrDelete.h"
Sprite* DebugText::spriteDatas[maxCharCount] = {};
std::vector<Status> DebugText::statues = {};
int  DebugText::spriteIndex = 0;


void DebugText::Initialize()
{
	for (int i = 0; i < _countof(spriteDatas); i++)
	{
		spriteDatas[i] = nullptr;
	}
	statues.resize(_countof(spriteDatas));
}

void DebugText::Print(const std::string & text,const float& posX,const float& posY,const float& scale)
{
	for (int i = 0; i < text.size(); i++)
	{
		if (spriteDatas[spriteIndex] == nullptr)
		{
			spriteDatas[spriteIndex] = new Sprite();
		}
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		const unsigned char& character = text[i];

		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;
		Status status;
		status.position = XMFLOAT2(posX + fontWidth * scale*scale * i, posY);
		status.scale = XMFLOAT2{ scale,scale };
		statues[spriteIndex] = status;
		spriteDatas[spriteIndex]->SpriteSetTextureRect("Debug",fontIndexX*fontWidth, fontIndexY*fontHeight, fontWidth, fontHeight);
		spriteIndex++;
	}

}

void DebugText::Draw(const XMFLOAT4& color)
{
	for (int i = 0; i < spriteIndex; i++)
	{
		spriteDatas[i]->DrawSprite("Debug", statues[i].position, 0.0f, statues[i].scale, color);
	}
	spriteIndex = 0;
}

void DebugText::End()
{
	for (int i = 0; i < _countof(spriteDatas); i++) {
		PtrDelete(spriteDatas[i]);
	}
}
