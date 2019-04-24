/**
*	@file Scene.cpp
*/

#include "Scene.h"
#include <iostream>

/**
*	コンストラクタ
*
*	@param name シーン名
*/

Scene::Scene(const char* name) : name(name)
{
}

/**
*	デストラクタ
*/
Scene::~Scene() {
	Finalize();
}

/**
*	シーンを活動状態にする
*/
void Scene::Play() {
	isActive = true;
}


/**
*	シーンを停止状態にする
*/

void Scene::Stop() {
	isActive = false;
}

/**
*	シーンを表示する
*/
void Scene::Show() {
	isVisible = true;
}

/**
*	シーンを非表示する
*/
void Scene::Hide() {
	isVisible = false;
}



