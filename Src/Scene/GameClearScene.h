#ifndef INCLUDED_H_GAMECLEARSCENE
#define INCLUDED_H_GAMECLEARSCENE
#include "../Scene.h"
#include "../Sprite.h"
#include "../Font.h"
#include "../Audio/Audio.h"
#include <vector>

/**
*	ÉQÅ[ÉÄÉNÉäÉAâÊñ 
*/

class GameClearScene : public Scene {
public:
	GameClearScene() :Scene("GameClearScene") {}
	virtual ~GameClearScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

private:
	bool flag = false;
	std::vector<Sprite>sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Audio::SoundPtr bgm;
	float timer = 0;

};



#endif // INCLUDED_H_GAMECLEARSCENE