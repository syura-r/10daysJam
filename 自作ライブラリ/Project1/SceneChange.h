#pragma once
#include "Sprite.h"

class SceneChange
{
public:
	SceneChange();
	~SceneChange();

	void Initialize();
	void Update();
	void Draw(XMFLOAT4 color);

	void ChangeStart();
	bool GetToBigEnd() { return toBigEnd; }
	bool GetToSmallEnd() { return toSmallEnd; }

private:
	Sprite* mainSp;
	XMFLOAT2 pos;
	XMFLOAT2 scale;
	int easingcount;
	bool toBig, toBigEnd;
	bool toSmall, toSmallEnd;
};

