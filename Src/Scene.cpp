/**
*	@file Scene.cpp
*/

#include "Scene.h"
#include <iostream>

/**
*	�R���X�g���N�^
*
*	@param name �V�[����
*/

Scene::Scene(const char* name) : name(name)
{
}

/**
*	�f�X�g���N�^
*/
Scene::~Scene() {
	Finalize();
}

/**
*	�V�[����������Ԃɂ���
*/
void Scene::Play() {
	isActive = true;
}


/**
*	�V�[�����~��Ԃɂ���
*/

void Scene::Stop() {
	isActive = false;
}

/**
*	�V�[����\������
*/
void Scene::Show() {
	isVisible = true;
}

/**
*	�V�[�����\������
*/
void Scene::Hide() {
	isVisible = false;
}



