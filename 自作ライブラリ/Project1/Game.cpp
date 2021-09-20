#include "Game.h"
#include"FPS.h"
#include"OBJLoader.h"
#include"PtrDelete.h"
#include"Audio.h"
#include"ParticleEmitter.h"
#include"DebugText.h"
#include"FbxLoader.h"
#include"Object3D.h"
#include<d3dx12.h>
#include"Create3DObject.h"
#include"Texture.h"
#include"Scene.h"
#include"Title.h"
#include"Play.h"
#include"Alpha.h"
#include"Ending.h"
#include"GameClear.h"
#include"GameOver.h"
#include"PipelineState.h"
#include"FBXManager.h"
#include"DrawMode.h"
#include "ComputeShade.h"
#include "Player.h"
#include "SelectUI.h"
#include "UnionPartsMotion.h"
DrawMode::MODE DrawMode::mode = DrawMode::NormalMap;

Game::Game()
{
	win = new Window(1920,1080);
	directX = DirectXLib::GetInstance();
}



Game * Game::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new Game();
	}
	return instance;
}

Game::~Game()
{
}

void Game::RoadAsset()
{
	switch (loadAssetLevel)
	{
	case 0:
		//テクスチャの初期化
		Texture::Initialize();
		//テクスチャの読み込み
		Texture::LoadTexture("LoadDot", "LoadDot.png");
		Texture::LoadTexture("LoadPicture", "LoadPicture.png");
		break;
	case 1:
		Texture::LoadTexture("Debug", "ASCII.png");
		Texture::LoadTexture("particle", "particle1.png");
		Texture::LoadTexture("shock", "shock.png");
		Texture::LoadTexture("dust", "dust.png");
		Texture::LoadTexture("white1x1", "white1x1.png");
		Texture::LoadTexture("SampleNormalMap", "SampleNormalMap.png");
		Texture::LoadTexture("StainedGlass", "StainedGlass.png");
		Texture::LoadTexture("StainedGlassNormal", "StainedGlassNormal.png");

		Texture::LoadTexture("title_logo", "Title/title_logo.png");
		Texture::LoadTexture("title_start", "Title/title_start.png");
		Texture::LoadTexture("title_end", "Title/title_end.png");
		Texture::LoadTexture("TitleBackground_1", "Title/TitleBackground_1.png");
		Texture::LoadTexture("TitleBackground_2", "Title/TitleBackground_2.png");
		Texture::LoadTexture("TitleBackground_3", "Title/TitleBackground_3.png");

		Texture::LoadTexture("result_restart", "LogoAndChoices/result_restart.png");
		Texture::LoadTexture("result_totitle", "LogoAndChoices/result_totitle.png");

		Texture::LoadTexture("gameover_bg", "Gameover/OverBackground_1.png");
		Texture::LoadTexture("gameover_main", "Gameover/result_gameover.png");
		Texture::LoadTexture("gameover_restart", "Gameover/result_restart.png");
		Texture::LoadTexture("gameover_totitle", "Gameover/result_totitle.png");

		Texture::LoadTexture("result_gameclear", "LogoAndChoices/result_gameclear.png");

		Texture::LoadTexture("player_life", "player_life.png");
		Texture::LoadTexture("hidari_sceneChange", "hidari_sceneChange.png");
		break;
	case 2:
		//Objモデルの読み込み
		OBJLoader::LoadModelFile("box", "box.obj", false);
		OBJLoader::LoadModelFile("sphere", "sphere.obj", true);
		OBJLoader::LoadModelFile("ground", "ground.obj", false);
		OBJLoader::LoadModelFile("plain", "plain.obj", false);
		OBJLoader::LoadModelFile("chr_sword", "chr_sword.obj", true);

		break;
	case 3:

		break;

	case 4:
		//FBXファイルの読み込み
		FBXManager::LoadModelFile("cube", "cube", true);
		FBXManager::LoadModelFile("boneTest", "boneTest", true);
		FBXManager::LoadModelFile("Hidari1", "Hidari1", true);
		FBXManager::LoadModelFile("Hidari2", "Hidari2", true);
		FBXManager::LoadModelFile("Hidari1_1", "Hidari1_1", true);
		FBXManager::LoadModelFile("Hidari1_2", "Hidari1_2", true);
		FBXManager::LoadModelFile("Hidari2_1", "Hidari2_1", true);
		FBXManager::LoadModelFile("Hidari2_2", "Hidari2_2", true);
		FBXManager::LoadModelFile("Hidari2_3", "Hidari2_3", true);

		//FBXManager::LoadModelFile("cleaningToolStorage", "cleaningToolStorage", true);

		break;

	default:
		loadAssetFinish = true;
		break;
	}
	
	loadAssetLevel++;
}

void Game::CreatePipeline()
{
	switch (createPipelineLevel)
	{
	case 0:
		PipelineState::CreatePipeline("Sprite", SPRITE);
		break;
	case 1:
		PipelineState::CreatePipeline("FBX", FBX);
		PipelineState::CreatePipeline("BasicObj", BasicObj);
		break;
	case 2:
		PipelineState::CreatePipeline("Particle", PARTICLE, ADD);
		PipelineState::CreatePipeline("PostEffect", BasePostEffect);
		break;
	case 3:
		PipelineState::CreatePipeline("Bloom", Bloom);
		PipelineState::CreatePipeline("NoShade", NoShade);
		break;
	case 4:
		PipelineState::CreatePipeline("NormalMap", NormalMap);
		break;
	case 5:
		PipelineState::CreatePipeline("PolygonBreak", PolygonBreak);
		break;
	case 6:
		PipelineState::CreatePipeline("FBXShadowMap", FBXShadowMap);
		break;
	case 7:
		PipelineState::CreatePipeline("GrassShadowMap", GrassShadowMap);
		break;
	case 8:
		PipelineState::CreatePipeline("Grass", GRASS);
		break;
	case 9:
		PipelineState::CreatePipeline("ShadowMap", NormalShadowMap);
		break;
	case 10:
		PipelineState::CreatePipeline("PBShadowMap", PBShadowMap);
		break;
	case 11:
		PipelineState::CreatePipeline("DrawShadowOBJ", DrawShadowOBJ);
		break;
	//case 12:
	//	break;
	default:
		createPipelineFinish = true;

		break;
	}

	createPipelineLevel++;
}

void Game::LoadFinish()
{
	//DirectInputオブジェクトの生成
	Input::Initialize(win->GetHwnd());

	lightCamera = new LightCamera();
	lightCamera->SetLightDir({ dir[0],dir[1],dir[2] });
	lightCamera->SetDistance(100);
	Object3D::SetLightCamera(lightCamera);
	Player::SetLightCamera(lightCamera);

	
	CollisionManager::GetInstance()->Initialize(0,20,500,0);
	
	sceneManeger = SceneManager::GetInstance();
	sceneManeger->Add(Scene::SCENE::Title, new Title());
	sceneManeger->Add(Scene::SCENE::Play, new Play());
	sceneManeger->Add(Scene::SCENE::Ending, new Ending());
	sceneManeger->Add(Scene::SCENE::GameClear, new GameClear());
	sceneManeger->Add(Scene::SCENE::GameOver, new GameOver());
	sceneManeger->Change(Scene::SCENE::Title);

	postEffect = new PostEffect();


	shadowMap = new ShadowMap();
	shadowMap->SetWindow(win);

	nowLoading = false;
}

void Game::DrawLoadTex()
{
	loadDot->SpriteSetTextureRect("LoadDot", 0, 0, 42 * (createPipelineLevel % 8), 25);
	loadDot->DrawSprite("LoadDot", { 1560,1010 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
	loadTex->DrawSprite("LoadPicture", { 0,0 }, 0, { 1,1 }, { 1,1,1,1 }, { 0,0 });
}

void Game::Initialize()
{
	win->CreateWidow(Window::WINDOW);

	// DirectX 初期化処理 ここから
	directX->Initialize(win);
	//ComputeWrapper::GetInstance()->Initialize();

	// DirectX 初期化処理 ここまで
	FPS::SetWindow(win);
	FbxLoader::GetInstance()->Initialize();
	PostEffect::SetWindow(win);
	Sprite::StaticInitialize(win);
	Audio::Initialize();
	//ShowCursor(false);
	nowLoading = true;

	//ComputeShade::StaticInitialize();
	
	FPS::Initialize();
	
#ifdef _DEBUG
	DebugText::Initialize();
#endif // _DEBUG
	camera = new DebugCamera();
	//camera->SetMatrixView({320,300,320}, { 320, 0, 320 }, { 0,1,0 });
	//camera->SetDistance(500);
	//PipelineState::Cre3tePipeline();
	Object3D::SetCamera(camera);
	ParticleEmitter::Initialize(camera);
	Player::SetDebugCamera(camera);
	SelectUI::SetWindowSize({ (float)win->GetWindowWidth(), (float)win->GetWindowHeight() });
	UnionPartsMotion::SetDebugCamera(camera);

	loadTex = new Sprite();
	loadDot = new Sprite();

}

void Game::Run()
{
	MSG msg{}; // メッセージ
	while (true)
	{

		FPS::StartMeasure();
		// メッセージがある?
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg); // キー入力メッセージの処理
			DispatchMessage(&msg); // プロシージャにメッセージを送る
		}

		// 終了メッセージが来たらループを抜ける
		if (msg.message == WM_QUIT) {
			break;
		}

		if (nowLoading)
		{
			//アセットロード
			RoadAsset();
			//パイプラインの生成
			CreatePipeline();
			if (createPipelineFinish && loadAssetFinish)
				LoadFinish();
			camera->Update();
			directX->BeginDraw();
			directX->ImguiDraw();
			directX->Depth();
			DrawLoadTex();

		}

		else if (!nowLoading)
		{
			if (sceneManeger->GetIsAllEnd()) {
				break;
			}

			Input::Update();
			Alpha::Update();
			lightCamera->Update();
			sceneManeger->Update();
			camera->Update();
			ParticleEmitter::Update();
			//2.画面クリアコマンドここまで
			//Object3D::SetDrawShadow(true);
			//shadowMap->PreDraw();
			//directX->ImguiDraw();
			//sceneManeger->PreDraw();
			//directX->Depth();
			//shadowMap->PostDraw();
			//Object3D::SetDrawShadow(false);

			directX->ImguiDraw();
			//if (DrawMode::GetMode() == DrawMode::NormalMap)
			//{
			directX->BeginDraw();
			//}
			//else
			//{
			//	postEffect->PreDraw();
			//}
			//3.描画コマンドここから
			sceneManeger->PreDraw();
			//背面描画ここまで
#ifdef _DEBUG
			DebugText::Draw();
#endif // _DEBUG
			directX->Depth();

			sceneManeger->PostDraw();
			ParticleEmitter::Draw();
			//if (DrawMode::GetMode() == DrawMode::Bloom)
			//{
			//	postEffect->PostDraw();
			//	//3.描画コマンドここまで
			//	directX->BeginDraw();
			//	postEffect->Draw();
			//}
		}
		directX->EndDraw();
		FPS::FPSFixed();

	}

}

void Game::End()
{
	//while (1)
	//{
	//	if (ShowCursor(true) >= 0)
	//		break;
	//}
	PtrDelete(shadowMap);
	PtrDelete(postEffect);
	PtrDelete(loadTex);
	PtrDelete(loadDot);
	PtrDelete(lightCamera);

	ParticleEmitter::End();
	OBJLoader::DeleteModels();
#ifdef _DEBUG
	DebugText::End();
#endif // _DEBUG
	FbxLoader::GetInstance()->Finalize();
	FBXManager::DeleteModels();
	PtrDelete(camera);
	sceneManeger->End();
	//デリートはここまでに終わらせる
	directX->End();
	win->End();
	PtrDelete(win);
}
