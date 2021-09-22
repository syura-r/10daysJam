#include "Boss.h"
#include "BoxCollider.h"
#include "CollisionAttribute.h"
#include "FBXManager.h"
#include "Input.h"
#include"OBJLoader.h"
#include "Player.h"
#include "PtrDelete.h"
#include "RaycastHit.h"
#include"CollisionManager.h"
#include "Easing.h"
#include "Enemy.h"
#include "ObjectManager.h"
#define SIZE  1.0f

Player* Boss::player = nullptr;
Boss::Boss(const Vector3& pos)
{
	Create(FBXManager::GetModel("Migi2"));
	InitPos = pos;
	CreateConstBuff();
	naObject = new Object();
	naObject->Create(FBXManager::GetModel("Migi1_1"));
	Initialize();
	BoxCollider* boxCollider = new BoxCollider({ 0,2.25f * scale.y,0,0 });
	boxCollider->SetScale(scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_ENEMYS);
	collider->SetMove(true);

	for(int i = 0;i<2;i++)
	{
		alerts[i].alertTex = new Sprite();
	}
#ifdef _DEBUG
	hitBox = new Object();
	hitBox->Create(OBJLoader::GetModel("box"));
	hitBox->SetScale(scale * 4);
	hitBox->SetColor({ 1,1,1,0.4f });
	hitBox->SetPosition(position + Vector3{ 0, 2.25f * scale.y, 0 });
#endif

}

Boss::~Boss()
{
	PtrDelete(naObject);
	for (int i = 0; i < 2; i++)
	{
		PtrDelete(alerts[i].alertTex);
	}
#ifdef _DEBUG
	PtrDelete(hitBox);
#endif
}

void Boss::Initialize()
{
	position = InitPos;
	destruction = 1.0f;
	scaleFactor = 1.0f;
	positionFactor = 2.0f;
	rotationFactor = 0.79f;
	tessellation = 1;
	color = { 0,0,0,1 };
	scale = { 0.7f,0.7f,0.7f };

	hp = MaxHP;
	damageCounter = 0;
	naPos = position;
	whiteCounter = 0;
	for (int i = 0; i < 2; i++)
	{
		alerts[i].drawAlert = false;
	}
	nowState = WAIT;
	appear = false;
	appearAnimation = false;
	onEasing = true;
	//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
	const float randX = std::rand() % 300 * 0.01f;
	AfterPos.x = position.x + randX;
	const float randY = std::rand() % 100 * 0.01f;
	AfterPos.y = position.y + randY;
	//基本位置を登録
	BasePos = position;
	aceel = { 0.001f,0.001f,0 };
	naObject->SetScale(scale);
	naObject->SetColor(color);
	attackCounter =0;
	attack = false;
}

void Boss::Update()
{
	if(Input::TriggerKey(DIK_K))
	{
		destination = Vector3{ 50,15,0 };
		toDestinationVel = (destination - position) / 40;
		attack = true;
		nowState = MOVE;
		attackType = BirthChildren;
	}
	
	if (player->GetWaitFight())
	{
		appearAnimation = true;
	}
	if(appearAnimation)
	{
		
		if (destruction <= 0.0f)
		{
			if (whiteCounter >= 30)
			{
				appearAnimation = false;
				appear = true;
				player->StartFight();
				onEasing = false;
			}
			else
			{
				float col = 0;
				if(whiteCounter<15)
				{
					col = Easing::EaseOutCirc(0, 1, 15, whiteCounter);
				}
				else
				{
					col = Easing::EaseOutCirc(1, 0, 15, whiteCounter - 15);
				}
				color = { col,col,col,1 };
				whiteCounter++;
			}
		}
		else
			destruction -= createSpeed;
		
		naPos = position;
		naObject->SetColor(color);
		naObject->SetPosition(naPos);
		naObject->Update();

		Object::Update();
		hitBox->SetPosition(position + Vector3{ 0, 2.25f * scale.y, 0 });
		hitBox->Update();

	}
	if (!appear)
		return;
	
	Move();
	Attack();
	//if(Input::TriggerKey(DIK_K))
	//{
	//	player->StartFight();
	//}
	if (dead)
		return;

	if (isDamage)
	{
		if (damageCounter >= invTime)
		{
			isDamage = false;
			damageCounter = 0;
			color = { 0, 0, 0, 1 };
			naObject->SetColor(color);
			tessellation = 3;

		}
		damageCounter++;
	}
	if (playBreakAnimation)
	{
		destruction += breakSpeed;
		if (destruction >= 1.0f)
			dead = true;
	}

	//position.x += speed;


	naPos = position;
	
	naObject->SetPosition(naPos);
	naObject->Update();

	
	Object::Update();
#ifdef _DEBUG
	hitBox->SetPosition(position + Vector3{ 0, 2.25f * scale.y, 0 });
	hitBox->Update();
#endif
}

void Boss::Draw()
{
	if (!appear && !appearAnimation)
		return;
#ifdef _DEBUG
	ImGui::Begin("Status");
	ImGui::SliderInt("HP", &hp, 0, MaxHP);
	ImGui::SliderFloat("destruction", &destruction, 0, 1.0f);
	ImGui::SliderFloat("scaleFactor", &scaleFactor, 0, 1.0f);
	ImGui::SliderFloat("positionFactor", &positionFactor, 0, 2.0f);
	ImGui::SliderFloat("rotationFactor", &rotationFactor, 0, 30.0f);
	ImGui::SliderInt("tessellation", &tessellation, 1, 32);
	ImGui::RadioButton("_OnEasing", &onEasing);
	ImGui::End();
#endif
	
	// 定数バッファへデータ転送
	ConstBuffData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->_Destruction = destruction;
	constMap->_ScaleFactor = scaleFactor;
	constMap->_PositionFactor = positionFactor;
	constMap->_RotationFactor = rotationFactor;
	constMap->_Tessellation = tessellation;
	constMap->_OnEasing = onEasing;


	constBuff->Unmap(0, nullptr);

	//floor->Draw();
	DirectXLib::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	if (Object3D::GetDrawShadow())
	{
		// パイプラインステートの設定
		PipelineState::SetPipeline("PBShadowMap");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());

	}
	else
	{
		// パイプラインステートの設定
		PipelineState::SetPipeline("PolygonBreak");
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(5, constBuff->GetGPUVirtualAddress());
	}
	naObject->CustomDraw(false, true, ALPHA, true);

	CustomDraw(false, true, ALPHA, true);

	for(int i = 0;i<2;i++)
	{
		if (alerts[i].drawAlert)
		{
			alerts[i].alertTex->DrawSprite("alert",alerts[i].pos,0,{1,1},{1,1,1,alerts[i].alpha});
		}
	}
#ifdef _DEBUG
	hitBox->Draw();
#endif

}

void Boss::OnCollision(const CollisionInfo& info)
{
	BaseCollider* col = info.collider;
	if (isDamage || col->GetAttribute() != COLLISION_ATTR_ATTACK)
		return;

	int damage = 0;
	switch (player->GetAttackState())
	{
	case Player::Boomerang:
	{
		invTime = 15;
		damage = 25;
		break;
	}
	case Player::MeleeAttack:
	{
		invTime = 13;
		damage = 50;
		break;
	}
	case Player::JumpAttack:
	{
		invTime = 26;
		damage = 80;
		break;
	}
	case Player::ULT:
	{
		invTime = 26;
		damage = 700;
		break;
	}
	default:
		return;
	}

	color = { 0.5f,0,0,1 };
	naObject->SetColor(color);
	hp -= damage;
	if (hp <= 0)
		playBreakAnimation = true;
	isDamage = true;
}

void Boss::Move()
{
	switch (nowState)
	{
	case WAIT:
	{
		if (aceel.x > 0)
		{
			//位置が特定の位置を過ぎたら
			if (position.x >= AfterPos.x)
			{
				//加速度反転
				aceel.x *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randX = std::rand() % 100 * 0.0001f;
				AfterPos.x = BasePos.x - randX;
			}
		}
		else
		{
			//位置が特定の位置を過ぎたら
			if (position.x <= AfterPos.x)
			{
				//加速度反転
				aceel.x *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randX = std::rand() % 100 * 0.0001f;
				AfterPos.x = BasePos.x + randX;
			}
		}
		if (aceel.y > 0)
		{
			//位置が特定の位置を過ぎたら
			if (position.y >= AfterPos.y)
			{
				//加速度反転
				aceel.y *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randY = std::rand() % 80 * 0.0001f;
				AfterPos.y = BasePos.y - randY;
			}
		}
		else
		{
			//位置が特定の位置を
			if (position.y <= AfterPos.y)
			{
				//加速度反転
				aceel.y *= -1;
				//ランダムで基本位置からどのくらい離れた位置を特定位置にするか決定
				const float randY = std::rand() % 80 * 0.0001f;
				AfterPos.y = BasePos.y + randY;
			}
		}

		velocity += aceel;

		//速度の最高値を設定
		if (velocity.x >= abs(0.05f))
		{
			if (velocity.x > 0)
				velocity.x = 0.05f;
			else
				velocity.x = -0.05f;
		}
		if (velocity.y >= abs(0.05f))
		{
			if (velocity.y > 0)
				velocity.y = 0.05f;
			else
				velocity.y = -0.05f;
		}
		position += velocity;

		break;
	}
	case MOVE:
	{
		switch (attackType)
		{
		case BirthChildren:
		{
			position += toDestinationVel;
			if(position.x <= destination.x + 0.01f && position.x >= destination.x - 0.01f)
			{
				if (attack)
					nowState = ATTACK;
				else
					nowState = WAIT;
			}
				
			break;
		}
		case SideRush:
		{
			break;
		}
		case VerticallyRush:
		{
			break;
		}
		case WaveRush:
		{
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	
}

void Boss::Attack()
{
	if (nowState != ATTACK)
		return;
	switch (attackType)
	{
	case BirthChildren:
	{
		if (attackCounter >= 120)
		{
			attackCounter = 0;
			attack = false;
			nowState = MOVE;
			destination = BasePos;
			toDestinationVel = (destination - position) / 40;
		}
		else
			attackCounter++;

		if (attackCounter == 50 || attackCounter == 100)
		{
			for (int i = 0; i < 2; i++)
			{
				Vector3 enemyPosition = position + Vector3{ 0, 2.25f * scale.y, 0 };
				Vector3 enemyVel = { 0,0.3f,0 };
				enemyVel.x = rand() % 601 * 0.001f - 0.3f;
				Enemy* enemy = new Enemy(enemyPosition, enemyVel);
				ObjectManager::GetInstance()->Add(enemy);
			}
		}
		break;
	}
	case SideRush:
	{
		break;
	}
	case VerticallyRush:
	{
		break;
	}
	case WaveRush:
	{
		break;
	}
	}
}

void Boss::CreateConstBuff()
{
	HRESULT result = DirectXLib::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 	// アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
}
