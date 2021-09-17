#include "Title.h"
#include "PtrDelete.h"
#include "Easing.h"

DebugCamera* Title::camera = nullptr;
Vector2 Title::windowsize = Vector2{ 0,0 };

Title::Title()
{
	next = SCENE::Play;

	//parts_array.push_back(new Parts("Hidari1_1"));
	//parts_array.push_back(new Parts("Hidari1_2"));
	//parts_array.push_back(new Parts("Hidari2_1"));
	//parts_array.push_back(new Parts("Hidari2_2"));
	//parts_array.push_back(new Parts("Hidari2_3"));

	logo = new Sprite();
	start = new Sprite();
	end = new Sprite();
}


Title::~Title()
{
	//for (int i = 0; i < parts_array.size(); i++)
	//{
	//	PtrDelete(parts_array.at(i));
	//}
	PtrDelete(logo);
	PtrDelete(start);
	PtrDelete(end);
}

void Title::Initialize()
{
	isEnd = false;

	////カメラリセット
	//camera->SetMatrixView({ 0, 0, -10 }, { 0, 0, 1 }, { 0, 1, 0 });

	//parts_array.at(0)->Initialize(Vector3{ -6,-2,0 }, Vector3{ 0,0,0 });
	//parts_array.at(1)->Initialize(Vector3{ -3,-2,0 }, Vector3{ 0,0,0 });
	//parts_array.at(2)->Initialize(Vector3{ 0,-2,0 }, Vector3{ 0,0,0 });
	//parts_array.at(3)->Initialize(Vector3{ 3,-2,0 }, Vector3{ 0,0,0 });
	//parts_array.at(4)->Initialize(Vector3{ 6,-2,0 }, Vector3{ 0,0,0 });

	//movienum = 0;
	//unionNum = 0;
	//unionAfterCounter = 0;

	selectNum = 0;
}

void Title::Update()
{
	if(Input::TriggerKey(DIK_RETURN))
	ShutDown();

	//選択
	XMFLOAT2 selectScale = { 0.8f,0.8f };
	XMFLOAT2 notSelectScale = { 0.6f,0.6f };
	XMFLOAT4 selectColor = { 1,1,1,1 };
	XMFLOAT4 notSelectColor = { 0.5f,0.5f,0.5f,1.0f };
	if (selectNum == 0)
	{
		//大きさ
		startScale = selectScale;
		endScale = notSelectScale;
		//色
		startColor = selectColor;
		endColor = notSelectColor;
	}
	else if (selectNum == 1)
	{
		//大きさ
		startScale = notSelectScale;
		endScale = selectScale;
		//色
		startColor = notSelectColor;
		endColor = selectColor;
	}

	//上選択
	if (Input::TriggerKey(DIK_UP) || Input::TriggerKey(DIK_W) || Input::CheckPadLStickUp() || Input::CheckPadButton(XINPUT_GAMEPAD_DPAD_UP))
	{
		selectNum = 0;
	}
	//下選択
	if (Input::TriggerKey(DIK_DOWN) || Input::TriggerKey(DIK_S) || Input::CheckPadLStickDown() || Input::CheckPadButton(XINPUT_GAMEPAD_DPAD_DOWN))
	{
		selectNum = 1;
	}


	//switch (movienum)
	//{
	//case 0:
	//	//スタート入力
	//	if (Input::TriggerKey(DIK_SPACE))
	//	{
	//		movienum++;
	//	}
	//	break;

	//case 1:
	//	//パーツ上昇
	//	if (RiseObjects())
	//	{
	//		movienum++;
	//	}
	//	break;

	//case 2:
	//	//パーツ合体
	//	if (UnionObjects())
	//	{
	//		movienum++;
	//	}
	//	break;

	//case 3:
	//	//決めポーズ的な
	//	if (poseObjects())
	//	{
	//		movienum++;
	//	}
	//	break;

	//case 4:
	//	isEnd = true;
	//	break;

	//default:
	//	break;
	//}

	//for (int i = 0; i < parts_array.size(); i++)
	//{
	//	parts_array.at(i)->obj->Update();
	//}
}

void Title::PreDraw()
{
}

void Title::PostDraw()
{
	//for (int i = 0; i < parts_array.size(); i++)
	//{
	//	parts_array.at(i)->obj->CustomDraw(true);
	//}
	//if (movienum < 2)
	//{
	//	logo->DrawSprite("TitleLogo", logoPosition, 0.0f, { 1.3f, 1.3f }, logoColor);
	//}


	XMFLOAT2 logoPosition = { windowsize.x / 2.0f, windowsize.y / 14.0f * 5.0f };
	XMFLOAT2 startPosition = { windowsize.x / 2.0f, windowsize.y / 14.0f * 10.0f };
	XMFLOAT2 endPosition = { windowsize.x / 2.0f, windowsize.y / 14.0f * 12.0f };

	logo->DrawSprite("title_logo", logoPosition, 0.0f, { 1.0f, 1.0f });
	start->DrawSprite("title_start", startPosition, 0.0f, startScale, startColor);
	end->DrawSprite("title_end", endPosition, 0.0f, endScale, endColor);

}

bool Title::RiseObjects()
{
	//各文字オブジェクト順に上へ移動
	const float targetYpos = 10.0f;
	const float easingMax = 30.0f;
	int movenum = 1;
	for (int i = 0; i < parts_array.size(); i++)
	{
		if (i != 0 &&
			parts_array.at(i - 1)->easingCounter >= easingMax / 3)
		{
			movenum = i + 1;
		}
	}

	for (int i = 0; i < movenum; i++)
	{
		//初期設定
		if (parts_array.at(i)->easingCounter <= 0.0f)
		{
			parts_array.at(i)->startPosition = parts_array.at(i)->obj->GetPosition();
			parts_array.at(i)->startRotation = parts_array.at(i)->obj->GetRotation();
		}

		//位置
		Vector3 pos = parts_array.at(i)->obj->GetPosition();
		pos.y = Easing::EaseInBack(parts_array.at(i)->startPosition.y, targetYpos, easingMax, parts_array.at(i)->easingCounter);
		parts_array.at(i)->obj->SetPosition(pos);

		//角度
		Vector3 rota = parts_array.at(i)->obj->GetRotation();
		rota.y = Easing::EaseInBack(parts_array.at(i)->startRotation.y, 0.0f, easingMax, parts_array.at(i)->easingCounter);
		rota.z = Easing::EaseInBack(parts_array.at(i)->startRotation.z, 0.0f, easingMax, parts_array.at(i)->easingCounter);
		parts_array.at(i)->obj->SetRotation(rota);

		if (parts_array.at(i)->easingCounter < easingMax)
		{
			parts_array.at(i)->easingCounter++;
		}
	}

	//最後尾が上に行ったら次へ
	if (parts_array.at(parts_array.size() - 1)->obj->GetPosition().y >= targetYpos)
	{
		//イージングカウントリセット
		for (int i = 0; i < parts_array.size(); i++)
		{
			parts_array.at(i)->easingCounter = 0.0f;
			parts_array.at(i)->obj->SetRotation(Vector3{ 0.0f, 0.0f, 0.0f });
		}
		return true;
	}
	return false;
}

bool Title::UnionObjects()
{
	Vector3 unionTargetPosition;
	Vector3 posA, posB;
	const float easingMax = 30.0f;
	const int aftertime = 30;

	switch (unionNum)
	{
	case 0://case1の準備
		//合体準備地点
		parts_array.at(0)->startPosition = Vector3{ 0, 90, 0 };
		parts_array.at(1)->startPosition = Vector3{ 6, 110, 0 };
		unionNum++;
		break;

	case 1://0-1
		//合体目標地点
		unionTargetPosition = Vector3{ 0, 100, 0 };
		camera->SetTarget(unionTargetPosition + Vector3{ 0, 2, 0 });

		posA.x = Easing::EaseInQuint(parts_array.at(0)->startPosition.x, unionTargetPosition.x, easingMax, parts_array.at(0)->easingCounter);
		posA.y = Easing::EaseInQuint(parts_array.at(0)->startPosition.y, unionTargetPosition.y, easingMax, parts_array.at(0)->easingCounter);
		posA.z = Easing::EaseInQuint(parts_array.at(0)->startPosition.z, unionTargetPosition.z, easingMax, parts_array.at(0)->easingCounter);
		if (parts_array.at(0)->easingCounter < easingMax)
		{
			parts_array.at(0)->easingCounter++;
		}
		parts_array.at(0)->obj->SetPosition(posA);

		posB.x = Easing::EaseInQuint(parts_array.at(1)->startPosition.x, unionTargetPosition.x, easingMax, parts_array.at(1)->easingCounter);
		posB.y = Easing::EaseInQuint(parts_array.at(1)->startPosition.y, unionTargetPosition.y, easingMax, parts_array.at(1)->easingCounter);
		posB.z = Easing::EaseInQuint(parts_array.at(1)->startPosition.z, unionTargetPosition.z, easingMax, parts_array.at(1)->easingCounter);
		if (parts_array.at(1)->easingCounter < easingMax)
		{
			parts_array.at(1)->easingCounter++;
		}
		parts_array.at(1)->obj->SetPosition(posB);


		//合体完了
		if (parts_array.at(0)->easingCounter >= easingMax ||
			parts_array.at(1)->easingCounter >= easingMax)
		{
			unionAfterCounter++;
		}
		//次の合体までの余韻
		if (unionAfterCounter >= aftertime)
		{
			parts_array.at(0)->easingCounter = 0.0f;
			parts_array.at(1)->easingCounter = 0.0f;
			unionAfterCounter = 0;
			unionNum++;
		}
		break;

	case 2://case3の準備
		//0と1をどける
		parts_array.at(0)->obj->SetPosition(Vector3(0, 0, 0));
		parts_array.at(1)->obj->SetPosition(Vector3(0, 0, 0));
		//合体準備地点
		parts_array.at(2)->startPosition = Vector3{ 0, 110, 0 };
		parts_array.at(3)->startPosition = Vector3{ 0, 100, 0 };
		parts_array.at(4)->startPosition = Vector3{ 0, 90, 0 };
		unionNum++;
		break;

	case 3://2-3-4
		//合体目標地点
		unionTargetPosition = Vector3{ 0, 100, 0 };

		posA.x = Easing::EaseInQuint(parts_array.at(2)->startPosition.x, unionTargetPosition.x, easingMax, parts_array.at(2)->easingCounter);
		posA.y = Easing::EaseInQuint(parts_array.at(2)->startPosition.y, unionTargetPosition.y, easingMax, parts_array.at(2)->easingCounter);
		posA.z = Easing::EaseInQuint(parts_array.at(2)->startPosition.z, unionTargetPosition.z, easingMax, parts_array.at(2)->easingCounter);
		if (parts_array.at(2)->easingCounter < easingMax)
		{
			parts_array.at(2)->easingCounter++;
		}
		parts_array.at(2)->obj->SetPosition(posA);

		posB.x = Easing::EaseInQuint(parts_array.at(3)->startPosition.x, unionTargetPosition.x, easingMax, parts_array.at(3)->easingCounter);
		posB.y = Easing::EaseInQuint(parts_array.at(3)->startPosition.y, unionTargetPosition.y, easingMax, parts_array.at(3)->easingCounter);
		posB.z = Easing::EaseInQuint(parts_array.at(3)->startPosition.z, unionTargetPosition.z, easingMax, parts_array.at(3)->easingCounter);
		if (parts_array.at(3)->easingCounter < easingMax)
		{
			parts_array.at(3)->easingCounter++;
		}
		parts_array.at(3)->obj->SetPosition(posB);

		posB.x = Easing::EaseInQuint(parts_array.at(4)->startPosition.x, unionTargetPosition.x, easingMax, parts_array.at(4)->easingCounter);
		posB.y = Easing::EaseInQuint(parts_array.at(4)->startPosition.y, unionTargetPosition.y, easingMax, parts_array.at(4)->easingCounter);
		posB.z = Easing::EaseInQuint(parts_array.at(4)->startPosition.z, unionTargetPosition.z, easingMax, parts_array.at(4)->easingCounter);
		if (parts_array.at(4)->easingCounter < easingMax)
		{
			parts_array.at(4)->easingCounter++;
		}
		parts_array.at(4)->obj->SetPosition(posB);


		//合体完了
		if (parts_array.at(2)->easingCounter >= easingMax ||
			parts_array.at(3)->easingCounter >= easingMax ||
			parts_array.at(4)->easingCounter >= easingMax)
		{
			unionAfterCounter++;
		}
		//次の合体までの余韻
		if (unionAfterCounter >= aftertime)
		{
			parts_array.at(2)->easingCounter = 0.0f;
			parts_array.at(3)->easingCounter = 0.0f;
			parts_array.at(4)->easingCounter = 0.0f;
			unionAfterCounter = 0;
			unionNum++;
		}
		break;

	case 4://case5の準備
		//合体準備地点
		parts_array.at(0)->startPosition = Vector3{ 0, 120, 0 };
		parts_array.at(1)->startPosition = parts_array.at(0)->startPosition;
		parts_array.at(2)->startPosition = Vector3{ 0, 75, 0 };
		parts_array.at(3)->startPosition = parts_array.at(2)->startPosition;
		parts_array.at(4)->startPosition = parts_array.at(2)->startPosition;
		unionNum++;
		break;

	case 5://(0-1)-(2-3-4)
		//合体目標地点
		unionTargetPosition = Vector3{ 0, 100, 0 };

		posA.x = Easing::EaseInQuint(parts_array.at(0)->startPosition.x, unionTargetPosition.x, easingMax, parts_array.at(0)->easingCounter);
		posA.y = Easing::EaseInQuint(parts_array.at(0)->startPosition.y, unionTargetPosition.y, easingMax, parts_array.at(0)->easingCounter);
		posA.z = Easing::EaseInQuint(parts_array.at(0)->startPosition.z, unionTargetPosition.z, easingMax, parts_array.at(0)->easingCounter);
		if (parts_array.at(0)->easingCounter < easingMax)
		{
			parts_array.at(0)->easingCounter += 0.5f;
		}
		parts_array.at(0)->obj->SetPosition(posA);
		parts_array.at(1)->obj->SetPosition(posA);

		posB.x = Easing::EaseInQuint(parts_array.at(2)->startPosition.x, unionTargetPosition.x, easingMax, parts_array.at(2)->easingCounter);
		posB.y = Easing::EaseInQuint(parts_array.at(2)->startPosition.y, unionTargetPosition.y, easingMax, parts_array.at(2)->easingCounter);
		posB.z = Easing::EaseInQuint(parts_array.at(2)->startPosition.z, unionTargetPosition.z, easingMax, parts_array.at(2)->easingCounter);
		if (parts_array.at(2)->easingCounter < easingMax)
		{
			parts_array.at(2)->easingCounter += 0.5f;
		}
		parts_array.at(2)->obj->SetPosition(posB);
		parts_array.at(3)->obj->SetPosition(posB);
		parts_array.at(4)->obj->SetPosition(posB);


		//合体完了
		if (parts_array.at(0)->easingCounter >= easingMax ||
			parts_array.at(2)->easingCounter >= easingMax)
		{
			unionAfterCounter++;
		}
		//次の合体までの余韻
		if (unionAfterCounter >= aftertime)
		{
			parts_array.at(0)->easingCounter = 0.0f;
			parts_array.at(2)->easingCounter = 0.0f;
			unionAfterCounter = 0;
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}

bool Title::poseObjects()
{
	const float easingMax = 60.0f;
	const float aftertime = 60.0f;
	Vector3 targetPosition = Vector3{ 0,100,0 };

	//初期設定
	if (parts_array.at(0)->easingCounter <= 0.0f)
	{
		for (int i = 0; i < parts_array.size(); i++)
		{
			parts_array.at(i)->obj->SetPosition(Vector3{ 0,100,200 });
		}
		parts_array.at(0)->startPosition = parts_array.at(0)->obj->GetPosition();
		parts_array.at(0)->startRotation = parts_array.at(0)->obj->GetRotation();
	}

	if (parts_array.at(0)->easingCounter <= easingMax)
	{
		//位置
		Vector3 pos = parts_array.at(0)->obj->GetPosition();
		pos.z = Easing::EaseInQuint(parts_array.at(0)->startPosition.z, targetPosition.z, easingMax, parts_array.at(0)->easingCounter);
		for (int i = 0; i < parts_array.size(); i++)
		{
			parts_array.at(i)->obj->SetPosition(pos);
		}

		//角度
		Vector3 rota = parts_array.at(0)->obj->GetRotation();
		rota.y = Easing::EaseInQuint(parts_array.at(0)->startRotation.y, 360.0f * 10.0f, easingMax, parts_array.at(0)->easingCounter);
		for (int i = 0; i < parts_array.size(); i++)
		{
			parts_array.at(i)->obj->SetRotation(rota);
		}

		parts_array.at(0)->easingCounter += 0.5f;
	}
	else
	{
		parts_array.at(0)->easingCounter++;
		if (parts_array.at(0)->easingCounter > aftertime + easingMax)
		{
			return true;
		}
	}

	return false;
}
