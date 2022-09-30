#include "olcPixelGameEngine.h"
#include <vector>

namespace akiPixelUtils {

    class UserInput {

    };

    class ParallaxBg {

    };

    class TextBox {

    };

    class DialogBox : TextBox {

    };

    class AnimatedSprite {
        bool by_spritesheet {true}, decal{false};
        //std::vector<olc::Sprite> frames {};
        std::unique_ptr<olc::Sprite> spritesheet {};
        std::unique_ptr<olc::Decal> decalsheet {};

        olc::vi2d sprite_size {};
        olc::vi2d spritesheet_offset {};
        olc::vi2d grid_composition {8, 8};
        float steptime {1};
        float elapsed_time {};
        int current_frame {0}, numframes {0};
        uint sprite_scale = 1U;


        public:
            /*AnimatedSprite(std::vector<olc::Sprite> spriteframes, olc::vi2d sprite_s, float steptime) {
                this->frames = spriteframes;
                this->sprite_size = sprite_s;
                this->steptime = steptime;
                this->by_spritesheet = false;
            }*/
            /*AnimatedSprite(olc::Sprite &sheet, olc::vi2d sprite_s, olc::vi2d offset, int frames, olc::vi2d grid, float steptime) {
                this->spritesheet = std::make_unique<olc::Sprite>(sheet);
                this->sprite_size = sprite_s;
                this->spritesheet_offset = offset;
                this->steptime = steptime;
                this->by_spritesheet = true;
                this->numframes = frames;
                this->grid_composition = grid;
            }*/
            AnimatedSprite(std::string sheet, olc::vi2d sprite_s, olc::vi2d offset, int frames, olc::vi2d grid, float steptime) {
                this->spritesheet = std::make_unique<olc::Sprite>(sheet);
                this->decalsheet = std::make_unique<olc::Decal>(this->spritesheet.get(), false, false);
                this->sprite_size = sprite_s;
                this->spritesheet_offset = offset;
                this->steptime = steptime;
                this->by_spritesheet = true;
                this->numframes = frames;
                this->grid_composition = grid;
            }
            AnimatedSprite() {
                this->spritesheet = {std::make_unique<olc::Sprite>(32,32)};
                //this->decalsheet = {std::make_unique<olc::Decal>()};
                
            }

        private:

            void goto_frame(int frame) {
                this->current_frame = frame % numframes;
            }

            int get_numframe() {
                return this->current_frame;
            }

            void skip_frames(int frames) {
                this->goto_frame(this->get_numframe() + frames);
            }

            void next_frame() {
                this->skip_frames(1);
            }

            olc::vi2d get_currentframe_coords() {
                int x {0}, y {0};
                y = this->current_frame % (this->grid_composition.y);
                x = this->current_frame / this->grid_composition.y;
                return {olc::vi2d(x * this->sprite_size.x, y * this->sprite_size.y)};
            }


        public:

            void set_spritesheet (std::unique_ptr<olc::Sprite> sheet) {
                this->spritesheet = std::move(sheet);
            }

            void set_spritesheet (std::unique_ptr<olc::Sprite> sheet, olc::vi2d s_size, olc::vi2d offset, int frames, olc::vi2d grid) {
                this->spritesheet = std::move(sheet);
                this->sprite_size = s_size;
                this->spritesheet_offset = offset;
                this->numframes = frames;
                this->grid_composition = grid;
            }
            /*void set_spritesheet (olc::Sprite sheet, olc::vi2d s_size, olc::vi2d offset, int frames, olc::vi2d grid) {
                this->spritesheet = std::make_unique<olc::Sprite>(sheet);
                this->sprite_size = s_size;
                this->spritesheet_offset = offset;
                this->numframes = frames;
                this->grid_composition = grid;
            }*/

            void set_steptime(float time) {
                this->steptime = time;
            }

            void set_scale(uint scale) {
                this->sprite_scale = scale;
            }

            void update_animation(float elapsed) {
                this->elapsed_time += elapsed;
                if (this->elapsed_time > this->steptime) {
                    this->next_frame();
                    this->elapsed_time = 0;
                }
            }

            void fastupdate_animation(float elapsed) {
                this->elapsed_time += elapsed;
                if (this->elapsed_time > this->steptime) {
                    this->skip_frames(static_cast<int>(this->elapsed_time / this->steptime));
                    this->elapsed_time = 0;
                }
            }

            void draw_current_frame(olc::vi2d pos, olc::PixelGameEngine *engine, bool as_decal) {
                olc::vi2d framecoords = this->get_currentframe_coords();
                printf("frame %d x=%d y=%d\n", this->current_frame, framecoords.x, framecoords.y);
                if (as_decal) {
                    engine->olc::PixelGameEngine::DrawPartialDecal(pos, this->sprite_size, this->decalsheet.get(), framecoords, this->sprite_size);
                } else {
                    engine->olc::PixelGameEngine::DrawPartialSprite(pos, this->spritesheet.get(), framecoords, this->sprite_size, this->sprite_scale);
                }
            }
            
    };

    class Entity {
        std::unique_ptr<AnimatedSprite> animsprite;
        olc::vf2d position;
        olc::vf2d movingspeed, movingacceleration, acc_speed_cap, friction {olc::vf2d(150, 250)};
        bool jumping {false}, enable_friction{true}, jumping_friction{false};
        float jumpingtime{0}, jumpedtime{0}, starting_jumpingspeed{100}, gravity{250}, current_jumpingspeed{0};
        int baseheight {0};

    public:

        Entity() = default;
        Entity(std::unique_ptr<AnimatedSprite> sprite) {
            this->animsprite = std::move(sprite);
        }

        void update_character(float elapsed) {
            this->update_position(elapsed);
            this->animsprite.get()->fastupdate_animation(elapsed);
        }
    
    private:

        void update_position (float elapsed) {
            if (this->jumping) {
                this->position -= olc::vf2d(0, this->current_jumpingspeed*elapsed);
                this->current_jumpingspeed -= this->gravity*elapsed;
                if (this->position.y >= baseheight) {
                    this->jumping = false;
                }
            }
            this->position += this->movingspeed * elapsed;
            this->movingspeed += (this->movingacceleration) * elapsed;
            if (this->movingspeed.mag2() >= this->acc_speed_cap.mag2()) this->movingspeed = this->acc_speed_cap;

            if (this->enable_friction && (!this->jumping || this->jumping_friction)) {
                this->movingspeed.x -= (this->movingspeed.x == 0 ? 0 : (this->movingspeed.x > 0 ? this->friction.x : (-this->friction.x))) * elapsed;
                this->movingspeed.y -= (this->movingspeed.y == 0 ? 0 : (this->movingspeed.y > 0 ? this->friction.y : (-this->friction.y))) * elapsed;
            }
        }

    public:

        void set_position(olc::vf2d pos) {
            this->position = pos;
        }

        void set_posx(float posx) {
            this->position.x = posx;
        }

        void set_posy(float posy) {
            this->position.y = posy;
        }

        void accelerate_to(olc::vf2d acceleration, olc::vf2d maxspeed) {
            this->acc_speed_cap = maxspeed;
            this->movingacceleration = acceleration;
        }

        void set_acceleration(olc::vf2d acceleration) {
            this->movingacceleration = acceleration;
        }

        void set_accelerated_speed_cap(olc::vf2d maxspeed) {
            this->acc_speed_cap = maxspeed;
        }

        void draw_entity(olc::PixelGameEngine *engine, bool as_decal) {
            this->animsprite.get()->draw_current_frame(position, engine, as_decal);
        }

        void set_baseheight(int height) {
            this->baseheight = height;
        }

        void set_jumpingspeed(float speed) {
            this->jumpingtime = speed;
        }

        void set_jumpingfriction(bool onoff) {
            this->jumping_friction = onoff;
        }

        void set_gravity(float newgravity) {
            this->gravity = newgravity;
        }

        void set_friction(olc::vf2d friction) {
            this->friction = friction;
        }

        void set_enable_friction(bool enable) {
            this->enable_friction = enable;
        }

        void startJump() {
            if (!jumping){
                jumping = true;
                this->current_jumpingspeed = this->starting_jumpingspeed;
            }

        }

        void set_speed(olc::vf2d speed) {
            this->movingspeed = speed;
        }

        void add_speed(olc::vf2d speed) {
            this->movingspeed += speed;
        }
    };

    class Player : Entity {

    };
}