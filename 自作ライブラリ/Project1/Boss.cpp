#include"Boss.h"

#include "Audio.h"
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
#include "ParticleEmitter.h"
#include "Audio.h"
#define SIZE  1.0f

Player* Boss::player = nullptr;
Boss::Boss(const Vector3& pos)
{
	Create(FBXManager::GetModel("Boss2"));
	InitPos = pos;
	CreateConstBuff();
	naObject = new Object();
	naObject->Create(FBXManager::GetModel("Boss1"));
	Initialize();
	BoxCollider* boxCollider = new BoxCollider();
	boxCollider->SetScale(scale * 2);
	SetCollider(boxCollider);
	collider->SetAttribute(COLLISION_ATTR_BOSS);//ボスとプレイヤーとで直接当たり判定を取らない(ボスの攻撃のみ判定)
	collider->SetMove(true);

	lockOnObj = new Object();
	lockOnObj->Create(FBXManager::GetModel("lockOn"));
	lockOnObj->SetRotation({ 90,0,0 });
	lockOnObj->SetScale({ scale.x * 4,scale.y * 2,scale.z * 2 });
	lockOnObj->SetColor({ 0.6f,0,0,1 });
	
	for(int i = 0;i<2;i++)
	{
		alert.alertTex = new Sprite();
	}
	for(int i = 0;i<3;i++)
	{
		hpTex[i] = new Sprite();
	}
#ifdef _DEBUG
	hitBox = new Object();
	hitBox->Create(OBJLoader::GetModel("box"));
	hitBox->SetScale(scale * 4);
	hitBox->SetColor({ 1,1,1,0.4f });
	hitBox->SetPosition(position );
#endif
	flashTex = new Sprite();
}

Boss::~Boss()
{
	PtrDelete(flashTex);
	PtrDelete(naObject);
	PtrDelete(alert.alertTex);
	for (int i = 0; i < 3; i++)
	{
		PtrDelete(hpTex[i]);
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
	rotation = {};
	naObject->SetRotation(rotation);
	hp = MaxHP;
	damageCounter = 0;
	naPos = position;
	whiteCounter = 0;
	for (int i = 0; i < 2; i++)
	{
		alert.drawAlert = false;
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
	rotVel = 0;
	for (int i = 0; i < 4; i++)
		selectAttack[i] = false;
	moveCounter = 0;
	end = false;
	drawFlash = false;
	flashAlpha = 0;
	flashCounter = 0;
	dead = false;
	playBreakAnimation = false;
}

void Boss::Update()
{
#ifdef _DEBUG
	if(Input::TriggerKey(DIK_K))
	{
		destination = Vector3{ 50,14,0 };
		toDestinationVel = (destination - position) / 40;
		attack = true;
		nowState = MOVE;
		attackType = BirthChildren;
		attackCounter = 0;
	}
	if (Input::TriggerKey(DIK_L))
	{
		destination = Vector3{ 70,14,0 };
		toDestinationVel = (destination - position) / 40;
		attack = true;
		nowState = MOVE;
		attackType = SideRush;
		attackCounter = 0;
	}
	if (Input::TriggerKey(DIK_P))
	{
		attack = true;
		nowState = MOVE;
		attackType = RollingRush;
		rotVel = 0;
		attackCounter = 0;
	}
	if (Input::TriggerKey(DIK_O))
	{
		attack = true;
		nowState = MOVE;
		attackType = Rocket;
		rotVel = 0;
		attackCounter = 0;
	}

#endif
	if(end)
	{
		if (flashCounter < 31)
			flashCounter++;
		if (flashCounter <= 10)
			flashAlpha = Easing::EaseInOutQuart(0, 1, 10, flashCounter);
		else if(flashCounter <= 20)
			flashAlpha = Easing::EaseInOutQuart(1, 0, 20, flashCounter - 10);
		if (flashCounter == 10)
		{
			player->EndPosition();
			position = { 50,15,1 };
			rotation = { 0,0,0 };
			naObject->SetPosition(position);
			naObject->SetRotation(rotation);
			Object::Update();
			naObject->Update();
			ObjectManager::GetInstance()->Initialize();

		}
		else if (flashCounter == 20)
			drawFlash = true;
		if (flashCounter == 30)
		{
			player->End();
		}
		if (player->GetFinish())
		{
			if (!playBreakAnimation)
			{
				Audio::PlayWave("Boss_dead");
			}
			playBreakAnimation = true;
			if(destruction == 0.0f)
				ParticleEmitter::CreateShock(position);
		}
		if (playBreakAnimation)
		{
			destruction += breakSpeed;
			if (destruction >= 1.0f)
				dead = true;
			return;
		}

		return;
	}

	
	if (player->GetWaitFight())
	{
		if (!appearAnimation)
		{
			Audio::PlayWave("Boss_spawn_1");
		}
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
				Audio::StopWave("playBGM");
				Audio::PlayWave("bossBGM", 0.2f, true);
			}
			else
			{
				if (whiteCounter == 0)
				{
					ParticleEmitter::CreateShock(position);
					Audio::PlayWave("Boss_spawn_2");
				}
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
#ifdef _DEBUG
		hitBox->SetPosition(position);
		hitBox->Update();
#endif
	}
	if (!appear)
		return;
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

	if (nowState == WAIT )
	{
		attackCounter++;
		CauseAttack();
		moveCounter++;
		NextMovePoint();
	}
	Move();
	Attack();
	//if(Input::TriggerKey(DIK_K))
	//{
	//	player->StartFight();
	//}

	//position.x += speed;


	naPos = position;
	naObject->SetRotation(rotation);
	naObject->SetPosition(naPos);
	naObject->Update();
	
	Object::Update();
#ifdef _DEBUG
	hitBox->SetPosition(position );
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

	if(drawFlash)
	{
		flashTex->DrawSprite("white1x1", { 960,540 }, 0, { 1920,1080 }, { 1,1,1,flashAlpha });
	}
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
		DirectXLib::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, constBuff->GetGPUVirtualAddress());
	}
	naObject->CustomDraw(false, false, ALPHA, true);

	CustomDraw(false, false, ALPHA, true);

	if(attackType == Rocket && attack && nowState == ATTACK)
	{
		lockOnObj->CustomDraw(true);
	}
	if(alert.drawAlert)
		alert.alertTex->DrawSprite("alert",alert.pos,0,{0.1f,0.1f},{1,1,1,alert.alpha});

	hpTex[1]->SpriteSetTextureRect("bossHp02", 0, 0, 692 * hp / MaxHP, 51);

	for(int i = 0;i<3;i++)
	{
		std::string texName = "bossHp0" + std::to_string(i + 1);
		hpTex[i]->DrawSprite(texName,{614,100},0,{1,1},{1,1,1,1},{0,0});
	}
#ifdef _DEBUG
	//hitBox->Draw();
#endif

}

void Boss::OnCollision(const CollisionInfo& info)
{
	BaseCollider* col = info.collider;
	if (isDamage || col->GetAttribute() != COLLISION_ATTR_ATTACK)
		return;

	int damage = 0;
	Vector3 effectPos = position + Vector3{ 0.0f,0.0f,-0.6f };
	switch (player->GetAttackState())
	{
	case Player::Boomerang:
	{
		invTime = 15;
		damage = 30;
		ParticleEmitter::CreateSlashEffects(effectPos, std::rand() % 180, { 0.5f,0.5f,1 });
		break;
	}
	case Player::MeleeAttack:
	{
		invTime = 13;
		damage = 55;
		ParticleEmitter::CreateSlashEffects(effectPos, std::rand() % 180, { 0.5f,0.5f,1 });
		break;
	}
	case Player::JumpAttack:
	{
		invTime = 26;
		damage = 80;
		ParticleEmitter::CreateWindEffects(effectPos, { 0.5f,0.5f,1 });
		break;
	}
	case Player::ULT:
	{
		invTime = 26;
		damage = 700;
		ParticleEmitter::CreateSlashPerfect(effectPos, std::rand() % 180, { 1,1,0.5f });
		break;
	}
	default:
		return;
	}

	color = { 0.6f,0,0,1 };
	naObject->SetColor(color);
	hp -= damage;
	if (hp <= 0)
	{
		end = true;
		drawFlash = true;
		hp = 0;
		player->EndFight();
	}
	ParticleEmitter::CreateRiseEffects(effectPos, { 1,0,0 });
	isDamage = true;
	Audio::PlayWave("hit2", 0.3f);

}


void Boss::NextMovePoint()
{
	if (moveCounter < 240)
		return;
	if(nowState != WAIT)
		return;
	int random = rand() % 4;
	switch (random)
	{
	case 0:
	{
		BasePos = { 55,11,0 };
		break;
	}
	case 1:
	{
		BasePos = { 55,15,0 };
		break;
	}
	case 2:
	{
		BasePos = { 45,11,0 };
		break;
	}
	case 3:
	{
		BasePos = { 45,15,0 };
		break;
	}
	default:
		break;
	}
	destination = BasePos;
	toDestinationVel = (destination - position) / 40;
	nowState = MOVE;
	attackType = NOAttack;
	moveCounter = 0;
}

void Boss::CauseAttack()
{
	if (attackCounter < 300)
		return;
	while (1)
	{
		int random = rand() % 4;
		if (!selectAttack[random])
		{
			switch (random)
			{
			case 0:
			{
				destination = Vector3{ 70,14,0 };
				toDestinationVel = (destination - position) / 40;
				attack = true;
				nowState = MOVE;
				attackType = SideRush;
				attackCounter = 0;
				break;
			}
			case 1:
			{
				attack = true;
				nowState = MOVE;
				attackType = Rocket;
				rotVel = 0;
				attackCounter = 0;
				break;
			}
			case 2:
			{
				destination = Vector3{ 50,14,0 };
				toDestinationVel = (destination - position) / 40;
				attack = true;
				nowState = MOVE;
				attackType = BirthChildren;
				attackCounter = 0;
				break;
			}
			case 3:
			{
				attack = true;
				nowState = MOVE;
				attackType = RollingRush;
				rotVel = 0;
				attackCounter = 0;
				break;
			}
			default:
				break;
			}
			selectAttack[random] = true;

			if (selectAttack[0] && selectAttack[1] && selectAttack[2] && selectAttack[3])
			{
				for (int i = 0; i < 4; i++)
				{
					selectAttack[i] = false;
				}
			}
			break;
		}
	}
	attackCounter = 0;

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
				const float randX = std::rand() % 100 * 0.00003f;
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
				const float randX = std::rand() % 100 * 0.00003f;
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
				const float randY = std::rand() % 80 * 0.00003f;
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
				const float randY = std::rand() % 80 * 0.00003f;
				AfterPos.y = BasePos.y + randY;
			}
		}

		velocity += aceel;

		//速度の最高値を設定
		if (velocity.x >= abs(0.03f))
		{
			if (velocity.x > 0)
				velocity.x = 0.03f;
			else
				velocity.x = -0.03f;
		}
		if (velocity.y >= abs(0.03f))
		{
			if (velocity.y > 0)
				velocity.y = 0.03f;
			else
				velocity.y = -0.03f;
		}
		position += velocity;
			
		if (position.x - scale.y * 2.25f < player->WallLeft)
		{
			position.x = player->WallLeft + scale.y * 2.25f;
		}
		if (position.x + scale.x * 2.25f > player->WallRight)
		{
			position.x = player->WallRight - scale.x * 2.25f;
		}
		if (position.y + scale.y * 2.25f > 20)
		{
			position.y = 20 - scale.y * 2.25f;
		}
		if (position.y - scale.y * 2.25f < 8)
		{
			position.y = 8 + scale.y * 2.25f;
		}

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
			position += toDestinationVel;
			if (position.x <= destination.x + 0.01f && position.x >= destination.x - 0.01f)
			{
				if (attack)
				{
					alert.drawAlert = true;
					alert.pos = { 1770,250 };
					nowState = ATTACK;
					Audio::PlayWave("Boss_SideRush_1");
				}
				else
					nowState = WAIT;
			}
			break;
		}
		case Rocket:
		{
			if (attack)
			{
				if (rotVel > RotMaxVel)
				{
					rotVel = RotMaxVel;
					nowState = ATTACK;
					fallV = -0.25f;
				}
				else
					rotVel += 0.13f;
				rotation.y += rotVel;
			}
			else
			{
				position += toDestinationVel;
				if (position.x <= destination.x + 0.01f && position.x >= destination.x - 0.01f)
				{
					nowState = WAIT;
				}
			}
			break;
		}
		case RollingRush:
		{
			if (attack)
			{

				if (rotVel > RotMaxVel)
				{
					rotVel = RotMaxVel;
					nowState = ATTACK;
					fallV = 0.5f;
					nowRollingDirection = LEFT;
				}
				else
					rotVel += 0.1f;
				rotation.z += rotVel;

			}
			else
			{
				if (rotVel < 3.0f)
				{
					if (attackCounter == 0)
					{
						if (rotation.x > 80 + 0.01f || rotation.x < 80-0.01f)
						{
							rotation += slipRotVel;
						}
						if (position.y <= 9.0f)
						{
							position.y = 9.0f;
							if (rotation.x <= 80 + 0.01f && rotation.x >= 80 - 0.01f)
							{
								attackCounter++;
								slipRotVel = (Vector3{ 0,0,0 } - rotation) / 30;
								destination = BasePos;
								toDestinationVel = (destination - position) / 40;
							}
						}
						else
						{
							fallV = max(fallV + fallAcc, fallVYMin);
							position.y += fallV;
						}
					}
					else if (attackCounter >= 210)
					{
						position += toDestinationVel;
						if (rotation.x > 0.01f || rotation.x < - 0.01f)
						{
							rotation += slipRotVel;
						}

						if (position.x <= destination.x + 0.01f && position.x >= destination.x - 0.01f)
						{
							nowState = WAIT;
							attackCounter = 0;
						}
					}
					else
						attackCounter++;
				}
				else
				{
					BoxCollider* boxCollider = new BoxCollider({}, scale * 2);
					boxCollider->SetAttribute(COLLISION_ATTR_ENEMYS);
					boxCollider->SetObject(this);
					boxCollider->Update();
					//クエリーコールバックの関数オブジェクト
					BossQueryCallBack callback(boxCollider);
					CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ALLIES, COLLISION_ATTR_ALLIES, boxCollider);

					RollingMove();
					rotVel -= 0.1f;
					if (rotVel < 3.0f)
					{
						rotation.z = (int)rotation.z % 360;
						slipRotVel = (Vector3{ 80,0,0 } - rotation) / 10;
					}
					rotation.z += rotVel;
				}
			}
			break;
		}
		case NOAttack:
		{
			position += toDestinationVel;
			if (position.x <= destination.x + 0.01f && position.x >= destination.x - 0.01f)
			{
				nowState = WAIT;
			}
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
		if (attackCounter >= 240)
		{
			attackCounter = 0;
			attack = false;
			nowState = MOVE;
			destination = BasePos;
			toDestinationVel = (destination - position) / 40;
		}
		else
			attackCounter++;

		if(attackCounter > 30 && attackCounter <= 50)
		{
			float scaleVal = Easing::EaseInOutQuart(0.7f, 0.4f, 20, attackCounter - 30);
			scale = { scaleVal,scaleVal ,scaleVal };
		}
		else if (attackCounter > 50 && attackCounter <= 60)
		{
			float scaleVal = Easing::EaseInOutQuart(0.4f, 1.0f, 10, attackCounter - 50);
			scale = { scaleVal,scaleVal ,scaleVal };
		}
		else if (attackCounter > 60 && attackCounter <= 65)
		{
			float scaleVal = Easing::EaseInOutQuart(1.0f, 0.7f, 5, attackCounter - 60);
			scale = { scaleVal,scaleVal ,scaleVal };
		}
		if (attackCounter > 150 && attackCounter <= 170)
		{
			float scaleVal = Easing::EaseInOutQuart(0.7f, 0.4f, 20, attackCounter - 150);
			scale = { scaleVal,scaleVal ,scaleVal };
		}
		else if (attackCounter > 170 && attackCounter <= 180)
		{
			float scaleVal = Easing::EaseInOutQuart(0.4f, 1.0f, 10, attackCounter - 170);
			scale = { scaleVal,scaleVal ,scaleVal };
		}
		else if (attackCounter >180 && attackCounter <= 185)
		{
			float scaleVal = Easing::EaseInOutQuart(1.0f, 0.7f, 5, attackCounter - 180);
			scale = { scaleVal,scaleVal ,scaleVal };
		}

		if (attackCounter == 60 || attackCounter == 180)
		{
			for (int i = 0; i < 2; i++)
			{
				Vector3 enemyPosition = position ;
				Vector3 enemyVel = { 0,0.3f,0 };
				enemyVel.x = rand() % 601 * 0.001f - 0.3f;
				Enemy* enemy = new Enemy(enemyPosition, enemyVel);
				enemy->Update();
				ObjectManager::GetInstance()->Add(enemy,false);
			}
			Audio::PlayWave("Boss_BirthChildren");
		}
		naObject->SetScale(scale);
		break;
	}
	case SideRush:
	{
		if (attackCounter >= 120 && attackCounter < 150)
		{
			if (position.x == 70)
			{
				Audio::PlayWave("Boss_SideRush_2");
			}
			position.x -= 2;
			BoxCollider* boxCollider = new BoxCollider({}, scale * 2);
			boxCollider->SetAttribute(COLLISION_ATTR_ENEMYS);
			boxCollider->SetObject(this);
			boxCollider->Update();
			//クエリーコールバックの関数オブジェクト
			BossQueryCallBack callback(boxCollider);
			CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ALLIES, COLLISION_ATTR_ALLIES, boxCollider);
			//rotation.z -= rotVel;
		}
		else if(attackCounter == 150)
		{
			position = {30,10,0};
			alert.pos = { 100,850 };
			Audio::PlayWave("Boss_SideRush_1");
		}
		else if (attackCounter < 30 || (attackCounter > 60 && attackCounter < 90) || 
			(attackCounter > 150 && attackCounter < 180) || (attackCounter > 210 && attackCounter < 240))
		{
			for (int i = 0; i < 2; i++)
			{
				alert.alpha = Easing::EaseInOutQuart(0, 1, 30, attackCounter % 30);
			}
		}
		else if(attackCounter > 90 && attackCounter < 120 || (attackCounter > 30 && attackCounter < 60) || 
			(attackCounter > 180 && attackCounter < 210) || (attackCounter > 240 && attackCounter < 270))
		{
			for (int i = 0; i < 2; i++)
			{
				alert.alpha = Easing::EaseInOutQuart(1, 0, 30, attackCounter % 30);
			}
		}
		else if (attackCounter >= 270 && attackCounter < 300)
		{
			if (position.x == 30)
			{
				Audio::PlayWave("Boss_SideRush_2");
			}
			position.x += 2;
			BoxCollider* boxCollider = new BoxCollider({}, scale * 2);
			boxCollider->SetAttribute(COLLISION_ATTR_ENEMYS);
			boxCollider->SetObject(this);
			boxCollider->Update();
			//クエリーコールバックの関数オブジェクト
			BossQueryCallBack callback(boxCollider);
			CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ALLIES, COLLISION_ATTR_ALLIES, boxCollider);
			//rotation.z += rotVel;
		}
		if (attackCounter >= 300)
		{
			attackCounter = 0;
			attack = false;
			nowState = MOVE;
			destination = BasePos;
			toDestinationVel = (destination - position) / 40;
			rotation.z = 0;
		}
		attackCounter++;
		break;
	}
	case Rocket:
	{
		const float speed = 0.5f;
		if (attackCounter == 0)
		{
			//加速
			fallV = max(fallV + 0.05f, 0.5f);
			position.y += fallV;
			if (position.y - scale.y * 2.25f > 25)
			{
				position = { 55,25,20 };
				attackCounter++;
				rotation = { 90,0,0 };
			}
		}
		else if (attackCounter <= 90)
		{
			destination = player->GetPosition();
			destination.y += 0.5f;
			Vector3 dir = Vector3{ -cosf(attackCounter * 3.1415f / 180 * 5),-sinf(attackCounter * 3.1415f / 180 * 5),-0.3f };
			dir.Normalize();
			dir *= speed;
			position += dir;
			if (attackCounter == 90)
			{
				toDestinationVel = (destination - position) / 30;
				Audio::PlayWave("Boss_Rocket_1");
				Audio::PlayWave("Boss_Rocket_2");
			}
		}
		else if (attackCounter <= 120)
		{
			position += toDestinationVel;
			if (attackCounter == 120)
			{
				BoxCollider* boxCollider = new BoxCollider({}, { scale.x * 2,scale.y,scale.z });
				boxCollider->SetAttribute(COLLISION_ATTR_ENEMYS);
				boxCollider->SetObject(this);
				boxCollider->Update();
				//クエリーコールバックの関数オブジェクト
				BossQueryCallBack callback(boxCollider);
				CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ALLIES, COLLISION_ATTR_ALLIES, boxCollider);
			}
		}
		else
		{
			Vector3 dir = Vector3{ 0,2,-1 };
			dir.Normalize();
			dir *= speed;
			position += dir;
			if (position.y > 25)
			{
				attackCounter = 0;
				attack = false;
				nowState = MOVE;
				destination = BasePos;
				toDestinationVel = (destination - position) / 40;
				rotation = {};
				return;
			}
		}
		if (attackCounter != 0)
		{
			attackCounter++;
		}
		if (attackCounter != 0 && attackCounter <= 120)
		{
			lockOnObj->SetPosition(destination);
			lockOnObj->Update();
		}
		break;
	}
	case RollingRush:
	{
		if (attackCounter == 0)
		{
			BoxCollider* boxCollider = new BoxCollider({}, scale * 2);
			boxCollider->SetAttribute(COLLISION_ATTR_ENEMYS);
			boxCollider->SetObject(this);
			boxCollider->Update();
			//クエリーコールバックの関数オブジェクト
			BossQueryCallBack callback(boxCollider);
			CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ALLIES, COLLISION_ATTR_ALLIES, boxCollider);

			//加速
			fallV = max(fallV + fallAcc, fallVYMin);

			position.y += fallV;
			if (position.y - scale.y * 2.25f < 8)
			{
				position.y = 8 + scale.y * 2.25f;
				attackCounter++;
			}
		}
		else if (attackCounter >= 20)
		{
			BoxCollider* boxCollider = new BoxCollider({}, scale * 2);
			boxCollider->SetAttribute(COLLISION_ATTR_ENEMYS);
			boxCollider->SetObject(this);
			boxCollider->Update();
			//クエリーコールバックの関数オブジェクト
			BossQueryCallBack callback(boxCollider);
			CollisionManager::GetInstance()->QueryBox(*boxCollider, &callback, COLLISION_ATTR_ALLIES, COLLISION_ATTR_ALLIES, boxCollider);

			RollingMove();
			if (attackCounter >= 440)
			{
				attackCounter = 0;
				attack = false;
				nowState = MOVE;
				fallV = 0;
				return;
			}
			attackCounter++;
		}
		else
			attackCounter++;
			
		rotation.z += rotVel;
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

void Boss::RollingMove()
{
	const float rollingVel = 0.3333f * rotVel / RotMaxVel;

	switch (nowRollingDirection)
	{
	case LEFT:
	{
		position.x -= rollingVel;
		if (position.x - scale.y * 2.25f < player->WallLeft)
		{
			position.x = player->WallLeft + scale.y * 2.25f;
			nowRollingDirection = UP;
		}
		break;
	}
	case RIGHT:
	{
		position.x += rollingVel;
		if (position.x + scale.x * 2.25f > player->WallRight)
		{
			position.x = player->WallRight - scale.x * 2.25f;
			nowRollingDirection = DOWN;
		}

		break;
	}
	case UP:
	{
		position.y += rollingVel;
		if (position.y + scale.y * 2.25f > 20)
		{
			position.y = 20 - scale.y * 2.25f;
			nowRollingDirection = RIGHT;
		}

		break;
	}
	case DOWN:
	{
		position.y -= rollingVel;
		if (position.y - scale.y * 2.25f < 8)
		{
			position.y = 8 + scale.y * 2.25f;
			nowRollingDirection = LEFT;
		}

		break;
	}
	}

}
