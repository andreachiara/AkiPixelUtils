#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "string"
#include "akiPixelUtils.hpp"
#include "akiJsonConvos.hpp"
#include "akiConvoTrees.hpp"
#include "nlohmann/json.hpp"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_thread.h"

#define OLC_PGEX_TTF
#include "olcPGEX_TTF.h"

typedef std::string u8string;

u8string To_UTF8(const std::u16string &s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.to_bytes(s);
}

u8string To_UTF8(const std::u32string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.to_bytes(s);
}

std::u16string To_UTF16(const u8string &s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.from_bytes(s);
}

std::u16string To_UTF16(const std::u32string &s)
{
    std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
    std::string bytes = conv.to_bytes(s);
    return std::u16string(reinterpret_cast<const char16_t*>(bytes.c_str()), bytes.length()/sizeof(char16_t));
}

std::u32string To_UTF32(const u8string &s)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv.from_bytes(s);
}

std::u32string To_UTF32(const std::u16string &s)
{
    const char16_t *pData = s.c_str();
    std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
    return conv.from_bytes(reinterpret_cast<const char*>(pData), reinterpret_cast<const char*>(pData+s.length()));
}

class Example : public olc::PixelGameEngine
{

	std::unique_ptr<olc::Sprite> front_buildings, back_buildings, far_buildings, character;
	std::unique_ptr<olc::Decal> front_buildings_d, back_buildings_d, far_buildings_d, character_d;

	olc::vf2d vBlockSize = { 16,16 };
	float moveratio = 0.7;
	float backratio = 0.4;
	float last_animation_update = 0;
	olc::vf2d viewmove  = olc::vf2d(0,0);
	olc::vf2d char_frame = olc::vf2d(10,0);
	olc::vf2d charmove = olc::vi2d(100,2480); 
	bool drawMsg = true;
	olc::vi2d lastmousepos;

	float maxjumpspeed {800.f}, jumpincrement {400.f}, jumpbasespeed {300.f};
	float jumpspeed {jumpbasespeed};

	akiPixelUtils::Camera maincamera = akiPixelUtils::Camera({0,2080}, {1280,720}, {0, 0}, {0,2080});
	

	std::unique_ptr<akiPixelUtils::AnimatedSprite> animatedchar;
	std::unique_ptr<akiPixelUtils::Character> player;
	akiPixelUtils::Level defaultlevel;


	akiJsonConvos::ConvoDB all_conversations;
	nlohmann::json jf;
	olc::Font font;

	int playerindex = 0;
	olc::Sprite backg;

	SoLoud::Soloud gSoloud; // SoLoud engine
	SoLoud::Wav currentMusic;      // One wave file
	SoLoud::Wav jump_sfx;      // One wave file


public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		front_buildings = std::make_unique<olc::Sprite>("../Assets/graphics/cyberpunk-street-files/cyberpunk-street-files/PNG/layers/foreground.png");
		back_buildings = std::make_unique<olc::Sprite>("../Assets/graphics/cyberpunk-street-files/cyberpunk-street-files/PNG/layers/back-buildings.png");
		far_buildings = std::make_unique<olc::Sprite>("../Assets/graphics/cyberpunk-street-files/cyberpunk-street-files/PNG/layers/far-buildings.png");
		
		this->back_buildings_d = std::make_unique<olc::Decal>(this->back_buildings.get(), false, false);
		this->far_buildings_d = std::make_unique<olc::Decal>(this->far_buildings.get(), false, false);
		this->front_buildings_d = std::make_unique<olc::Decal>(this->front_buildings.get(), false, false);

		
		
		character = std::make_unique<olc::Sprite>("../Assets/graphics/Bot Wheel/move with FX.png");
		animatedchar = std::make_unique<akiPixelUtils::AnimatedSprite>("../Assets/graphics/Bot Wheel/move with FX.png", olc::vi2d(37, 26), olc::vi2d{}, 8, olc::vi2d(1, 8), 0.15);
		animatedchar.get()->set_scale(4);
		player = std::make_unique<akiPixelUtils::Character>(std::move(animatedchar));
		player.get()->set_baseheight(600);
		player.get()->set_position(charmove);
		player.get()->set_phys(true);
		olc::vf2d playersize {olc::vf2d(37, 26) * 4};
		olc::vf2d p_offset {54, 0};
		playersize.x -= 22.f;
		player.get()->set_size(playersize - p_offset);
		player.get()->set_offset(p_offset);
		akiPixelUtils::Rectangle headhitbox {{75, 15}, {40,20}};
		akiPixelUtils::Rectangle feethitbox {{75, 90}, {40,14}};
		akiPixelUtils::Rectangle leffthitbox {{55, 30}, {15,69}};
		akiPixelUtils::Rectangle righthitbox {{110, 30}, {15,69}};
		akiPixelUtils::Rectangle generic {{30, 50}, {2,2}};



		player.get()->set_hitbox(headhitbox, akiPixelUtils::HitboxTypes::TOP);
		player.get()->set_hitbox(feethitbox, akiPixelUtils::HitboxTypes::BOTTOM);
		player.get()->set_hitbox(leffthitbox, akiPixelUtils::HitboxTypes::LEFT);
		player.get()->set_hitbox(righthitbox, akiPixelUtils::HitboxTypes::RIGHT);
		player.get()->set_hitbox(generic, akiPixelUtils::HitboxTypes::GENERIC);
		
		std::ifstream ifs("../exampleconvo.json");
		jf = nlohmann::json::parse(ifs);
		all_conversations = jf;

		this->defaultlevel = akiPixelUtils::Level("../Assets/terrains/test_terrain_s.png", "../Assets/terrains/test_terrain_s.png");
		this->defaultlevel.set_camera(this->maincamera);
		this->defaultlevel.set_mapsize({1280, 720});

		backg = olc::Sprite("../Assets/terrains/test_terrain_s.png");

		olc::Font::init();
        font = olc::Font{"../Assets/fonts/PlayfairDisplay-Regular.ttf", 34};
        font.AddFallbackFont("../Assets/fonts/NotoSerifJP-Regular.otf");

		std::vector<akiPixelUtils::Rectangle> newcollisors {
			{{-100,2680}, {1480, 10}},
			{{416, 2445},{366, 76}},
			{{842, 2366},{171, 23}},
			{{494, 2278},{245, 31}},
			{{526, 1988},{202, 29}},
			{{786, 1836},{188, 30}},
			{{1009, 1662},{162, 43}},
			{{399, 1736},{281, 25}},
			{{604, 1497},{386, 25}},
			{{42, 1255},{583, 46}},
			{{649, 1050},{365, 26}},
			{{364, 937},{371, 24}},
			{{493, 743},{324, 14}},
			{{600, 606},{353, 18}}
		

		};

		akiPixelUtils::MapCollisors collisors = akiPixelUtils::MapCollisors(newcollisors);

		defaultlevel.set_collisors(collisors);

		playerindex = defaultlevel.add_character(std::move(player));

		gSoloud.init(); // Initialize SoLoud
		uint32_t res = currentMusic.load("../Assets/audio/Ludum Dare 38 - Track 10.wav"); // Load a wave
		jump_sfx.load("../Assets/audio/Jump 1.wav"); // Load a wave
		printf("res: %d", res);
		int x = gSoloud.play(currentMusic); // Play the wave &  Grab the handle
		gSoloud.setLooping(x, true);
		gSoloud.setPan(x, -0.2f);    // Use handle to adjust panning


        return true;

	}

	void handleUserInput () {
	}

	void dialogBox (const std::u32string title, const std::u32string text) {
		FillRectDecal(olc::vi2d(10, 5), olc::vi2d(940, 200), olc::BLACK);
		DrawLineDecal({10, 5}, {950, 5}, olc::CYAN);
		DrawLineDecal({10, 5}, {10, 205}, olc::CYAN);
		DrawLineDecal({950, 5}, {950, 205}, olc::CYAN);
		DrawLineDecal({10, 205}, {950, 205}, olc::CYAN);
		olc::Decal *titledec = font.RenderStringToDecal(title, olc::MAGENTA); // olc::vi2d(13, 17)
		olc::Decal *textdec =  font.RenderStringToDecal(text, olc::WHITE); // olc::vi2d(13, 34)
		DrawDecal(olc::vi2d(13, 17), titledec);
		DrawDecal(olc::vi2d(16, 50), textdec);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(0);
		olc::Decal backgd = {olc::Decal(&backg)};

		//DrawPartialDecal(olc::vi2d(0, 0), olc::vi2d(1280, 720), &backgd, olc::vi2d(100, 200), olc::vi2d(1280, 720));

		// called once per frame
		/*for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				//Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand()% 255));
				*/

		//olc::vf2d mousemove = olc::vf2d(GetMouseX(), GetMouseY());
		olc::vf2d viewhor = olc::vf2d(viewmove.x, 0);
		akiPixelUtils::Character* playerptr;
		defaultlevel.get_characterptr(playerindex, &playerptr);
		if (GetKey(olc::Key::A).bHeld) playerptr->accelerate_to(olc::vf2d(-1400, 0), olc::vf2d(-510, 0));
		else if (GetKey(olc::Key::D).bHeld) playerptr->accelerate_to(olc::vf2d(1400, 0), olc::vf2d(510, 0));
		else playerptr->set_acceleration(olc::vf2d(0,0));

		SetPixelMode(olc::Pixel::ALPHA); 
		//DrawDecal(olc::vf2d(0,0), far_buildings_d.get(),olc::vf2d(4,4));
		//DrawPartialDecal(olc::vf2d(0,0), olc::vf2d(240, 192)*4, back_buildings_d.get(), viewhor*backratio, olc::vf2d(240, 192));
		//DrawPartialDecal(olc::vf2d(0,0), olc::vf2d(240, 192)*4, front_buildings_d.get(), viewhor, olc::vf2d(240, 192));
		
		/*if (GetKey(olc::Key::LEFT).bHeld) {
			viewmove -= olc::vf2d(moveratio, 0);
			if (viewmove.x < 0) viewmove.x = 0;
			else {
				playerptr->add_speed(olc::vf2d(moveratio, 0));
			}
		} 
		if (GetKey(olc::Key::RIGHT).bHeld) {
			viewmove += olc::vf2d(moveratio, 0);
			if (viewmove.x > 112)
			{ 
				viewmove.x = 112;
			}
			else {
				playerptr->add_speed(olc::vf2d(-moveratio, 0));
			}
		}*/

		if(GetKey(olc::Key::SPACE).bHeld) {
			jumpspeed = jumpspeed <= maxjumpspeed ? jumpspeed + (jumpincrement * fElapsedTime) : maxjumpspeed;
			printf("speed accumulator : %f\n", jumpspeed);
		}
		if(GetKey(olc::Key::SPACE).bReleased) {
			if (playerptr->startJump(jumpspeed)){
				gSoloud.play(jump_sfx);
			}
			jumpspeed = jumpbasespeed;
		}
			
		//if(GetKey(olc::Key::SPACE).bHeld) playerptr->startJump();

		if(GetKey(olc::Key::UP).bHeld) defaultlevel.get_camera()->add_position({0, -10});
		if(GetKey(olc::Key::DOWN).bHeld) defaultlevel.get_camera()->add_position({0, 10});
		if(GetKey(olc::Key::LEFT).bHeld) defaultlevel.get_camera()->add_position({-10, 0});
		if(GetKey(olc::Key::RIGHT).bHeld) defaultlevel.get_camera()->add_position({10, 0});

		if(GetKey(olc::Key::ENTER).bPressed) drawMsg = !drawMsg;

		if(GetMouse(olc::Mouse::LEFT).bPressed){
			this->lastmousepos = defaultlevel.get_camera()->convert_local_to_world(GetMousePos());
			printf("{{%d, %d},", this->lastmousepos.x, this->lastmousepos.y);
		}

		if (GetMouse(olc::Mouse::RIGHT).bPressed) {
			olc::vi2d rect_size = defaultlevel.get_camera()->convert_local_to_world(GetMousePos()) - this->lastmousepos;
			printf("{%d, %d}},\n", rect_size.x, rect_size.y);
		}

		defaultlevel.get_camera()->set_centerposition(playerptr->get_position());

		//playerptr->DBGDRAW(this);

		defaultlevel.draw_level(this, true);
		defaultlevel.update_entities(fElapsedTime);
		defaultlevel.draw_entities(this, true);

		defaultlevel.DBGDRAW(this);
		std::u32string test = U"this is a test";
  		std::u32string utf32 {To_UTF32(all_conversations.get_convo_by_id(0).get_exchange_by_id(1).get_sentence())};

		if (drawMsg) {
			dialogBox(test, utf32);
		}

		if (gSoloud.getActiveVoiceCount() <= 0)
		{
			// ENDED sleep for a bit then kill
			SoLoud::Thread::sleep(100);
			gSoloud.deinit(); // Clean up!
		}
		
		//+y:27px for next sprite
		return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(1280, 720, 1, 1))
		demo.Start();

	return 0;
}
