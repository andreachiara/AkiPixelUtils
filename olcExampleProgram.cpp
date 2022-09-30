#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "string"
#include "akiPixelUtils.hpp"
#include "akiJsonConvos.hpp"
#include "nlohmann/json.hpp"

class Example : public olc::PixelGameEngine
{

	std::unique_ptr<olc::Sprite> front_buildings, back_buildings, far_buildings, character;

	olc::vf2d vBlockSize = { 16,16 };
	float moveratio = 0.7;
	float backratio = 0.4;
	float last_animation_update = 0;
	olc::vf2d viewmove  = olc::vf2d(0,0);
	olc::vf2d char_frame = olc::vf2d(10,0);
	olc::vf2d charmove = olc::vi2d(0,150); 
	bool drawMsg = false;

	std::unique_ptr<akiPixelUtils::AnimatedSprite> animatedchar;
	akiPixelUtils::Entity player;

	akiJsonConvos::ConvoDB all_conversations;


public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		front_buildings = std::make_unique<olc::Sprite>("./Assets/cyberpunk-street-files/cyberpunk-street-files/PNG/layers/foreground.png");
		back_buildings = std::make_unique<olc::Sprite>("./Assets/cyberpunk-street-files/cyberpunk-street-files/PNG/layers/back-buildings.png");
		far_buildings = std::make_unique<olc::Sprite>("./Assets/cyberpunk-street-files/cyberpunk-street-files/PNG/layers/far-buildings.png");
		character = std::make_unique<olc::Sprite>("./Assets/Bot Wheel/move with FX.png");
		animatedchar = std::make_unique<akiPixelUtils::AnimatedSprite>("./Assets/Bot Wheel/move with FX.png", olc::vi2d(37, 26), olc::vi2d{}, 8, olc::vi2d(1, 8), 0.15);
		animatedchar.get()->set_scale(1);
		player = akiPixelUtils::Entity(std::move(animatedchar));
		player.set_baseheight(150);
		player.set_position(charmove);

		std::ifstream ifs("test.json");
		nlohmann::json jf = nlohmann::json::parse(ifs);

		std::string str(R"({"json": "beta"})");
		nlohmann::json all_conversations = nlohmann::json::parse(str);

		return true;
	}

	void handleUserInput () {
	}

	void dialogBox (const std::string title, const std::string text) {
		FillRect(olc::vi2d(10, 5), olc::vi2d(220, 50), olc::BLACK);
		DrawString(olc::vi2d(13, 8), title, olc::MAGENTA, 1);
		DrawString(olc::vi2d(13, 21), text, olc::WHITE, 1);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(0);
		// called once per frame
		/*for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				//Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand()% 255));
				*/

		//olc::vf2d mousemove = olc::vf2d(GetMouseX(), GetMouseY());

		if (GetKey(olc::Key::A).bHeld) player.accelerate_to(olc::vf2d(-300, 0), olc::vf2d(-70, 0));
		else if (GetKey(olc::Key::D).bHeld) player.accelerate_to(olc::vf2d(300, 0), olc::vf2d(70, 0));
		else player.set_acceleration(olc::vf2d(0,0));
		if (GetKey(olc::Key::LEFT).bHeld) {
			viewmove -= olc::vf2d(moveratio, 0);
			if (viewmove.x < 0) viewmove.x = 0;
			else {
				player.add_speed(olc::vf2d(moveratio, 0));
			}
		} 
		if (GetKey(olc::Key::RIGHT).bHeld) {
			viewmove += olc::vf2d(moveratio, 0);
			if (viewmove.x > 112)
			{ 
				viewmove.x = 112;
			}
			else {
				player.add_speed(olc::vf2d(-moveratio, 0));
			}
		}

		if(GetKey(olc::Key::SPACE).bHeld) player.startJump();

		olc::vf2d viewhor = olc::vf2d(viewmove.x, 0);

		

		SetPixelMode(olc::Pixel::ALPHA); 
		DrawSprite(olc::vf2d(0,0) * vBlockSize, far_buildings.get(), 1);
		DrawPartialSprite(olc::vf2d(0,0) * vBlockSize, back_buildings.get(), viewhor*backratio, olc::vf2d(240, 192), 1);
		DrawPartialSprite(olc::vf2d(0,0) * vBlockSize, front_buildings.get(), viewhor, olc::vf2d(240, 192), 1);
		player.update_character(fElapsedTime);
		player.draw_entity(this, true);
		if (drawMsg) {
			dialogBox("Test Text Tesxt", all_conversations.get_convo_by_id(0).get_exchange_by_id(0).get_sentence());
		}
		
		//+y:27px for next sprite
		return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(240, 192, 3, 3))
		demo.Start();

	return 0;
}
