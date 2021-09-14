#include "Title.h"

Title::Title()
{
	next = SCENE::Test;
}


Title::~Title()
{
}

void Title::Initialize()
{
	isEnd = false;
}

void Title::Update()
{
	if(Input::TriggerKey(DIK_RETURN))
	ShutDown();
}

void Title::PreDraw()
{
}

void Title::PostDraw()
{
}
