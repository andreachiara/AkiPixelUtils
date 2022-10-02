#include "olcPixelGameEngine.h"
#include <vector>

namespace akiPixelUtils {

    struct Line{};

    struct Rectangle {
        olc::vf2d pos;
        olc::vf2d size;

        olc::vf2d endpos() {
            return this->pos + this->size;
        }
        olc::vf2d topleft() {
            return olc::vf2d(this->pos.x, this->pos.y);
        }
        olc::vf2d topright() {
            return olc::vf2d(this->pos.x+this->size.x, this->pos.y);
        }
        olc::vf2d bottomleft() {
            return olc::vf2d(this->pos.x, this->pos.y+this->size.y);
        }
        olc::vf2d bottomright() {
            return olc::vf2d(this->pos.x+this->size.x, this->pos.y+this->size.y);
        }



    };

    class Hitbox {
        Rectangle borders;
    };

    bool check_intersect(Rectangle rect1, Rectangle rect2) { //rect2 is moving
        if (
            rect1.pos.x < rect2.endpos().x &&
            rect1.endpos().x > rect2.pos.x &&
            rect1.pos.y < rect2.endpos().y &&
            rect1.endpos().y > rect2.pos.y
            ) {
                return true;
            }
            return false;
    }

    class MapCollisors {
        std::vector<Rectangle> collisors;

        public:

        MapCollisors() = default;
        MapCollisors(std::vector<Rectangle> collisorslist) {
            this->collisors = collisorslist;
        }

            void add_collisor(Rectangle newcollisor) {
                this->collisors.push_back(newcollisor);
            }

            void add_collisors(std::vector<Rectangle> collisorslist) {
                for (auto& collisor : collisorslist) {
                    this->add_collisor(collisor);
                }
            }

            std::vector<Rectangle> get_allcollisors() {
                return this->collisors;
            }

            void DBG_draw_terrainhitboxes(olc::PixelGameEngine *engine) {
                for (auto& collisor : this->collisors) {
                    engine->DrawRect(collisor.pos, collisor.size, olc::RED);
                }
            }
    };

    class UserInput {

    };

    class ParallaxBg {

    };

    class TextBox {
        std::string title{}, text{};
        olc::vf2d position{};

    public:

        TextBox() = default;
        TextBox(std::string title, std::string text) {
            this->title = title;
            this->text = text;
        }

        void set_text(std::string text) {
            this->text = text;
        }

        void set_title(std::string title) {
            this->title = title;
        }

        void set_pos(olc::vf2d position) {
            this->position = position;
        }

        void draw_textbox_atpos(olc::vi2d pos, olc::PixelGameEngine *engine) {

        }

        void draw_textbox(olc::PixelGameEngine *engine) {
            this->draw_textbox_atpos(this->position, engine);
        }



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
                //printf("frame %d x=%d y=%d\n", this->current_frame, framecoords.x, framecoords.y);
                if (as_decal) {
                    engine->olc::PixelGameEngine::DrawPartialDecal(pos, this->sprite_size*this->sprite_scale, this->decalsheet.get(), framecoords, this->sprite_size);
                } else {
                    engine->olc::PixelGameEngine::DrawPartialSprite(pos, this->spritesheet.get(), framecoords, this->sprite_size, this->sprite_scale);
                }
            }
            
    };
    
    enum MovingY {
        GROUND,
        JUMP,
        FALL
    };

    class Entity {
    protected:
        std::unique_ptr<AnimatedSprite> idleanimation;
        olc::vf2d position, size, offset;
        olc::vf2d movingspeed, movingacceleration, acc_speed_cap, friction {olc::vf2d(600, 0)};
        bool jumping {false}, enable_friction{true}, jumping_friction{false}, falling{true}, grounded {false};
        float jumpingtime{0.f}, jumpedtime{0.f}, starting_jumpingspeed{700.f}, gravity{900.f}, current_jumpingspeed{0.f};
        int baseheight {0};

        Rectangle hitbox;

        bool is_physic {true};


        MovingY jumpingfalling {MovingY::GROUND};

    public:

        Entity() = default;
        Entity(std::unique_ptr<AnimatedSprite> sprite) {
            this->idleanimation = std::move(sprite);
        }

        void update_character(float elapsed) {
            this->update_position(elapsed);
            this->idleanimation.get()->fastupdate_animation(elapsed);
        }
    
    private:

        void update_position (float elapsed) {

            switch (this->jumpingfalling) {
                case MovingY::GROUND:
                    this->movingspeed.y = 0;
                    this->jumpingfalling = MovingY::FALL;
                    break;

                case MovingY::FALL:
                    //this->movingspeed.y = std::max(this->movingspeed.y + (this->gravity * elapsed), 0.f);
                    this->movingspeed.y += this->gravity * elapsed;

                    break;

                case MovingY::JUMP:
                    this->movingspeed.y += this->gravity * elapsed;
                    if (this->movingspeed.y >= 0) {
                        this->jumpingfalling = MovingY::FALL;
                    }
                    break;

                default:
                    break;
            }

            printf("speed: %f, %f\n", this->movingspeed.x, this->movingspeed.y);

            this->position += this->movingspeed * elapsed;
            this->movingspeed += (this->movingacceleration) * elapsed;
            if (std::abs(this->movingspeed.x) >= std::abs(this->acc_speed_cap.x)) {
                this->movingspeed.x = this->acc_speed_cap.x;
            }

            if (this->enable_friction && (this->jumpingfalling == MovingY::GROUND || this->jumping_friction)) {
                printf("Frictioncalc\n");
                this->movingspeed.x -= (this->movingspeed.x == 0 ? 0 : (this->movingspeed.x > 0 ? this->friction.x : (-this->friction.x))) * elapsed;
                this->movingspeed.y -= (this->movingspeed.y == 0 ? 0 : (this->movingspeed.y > 0 ? this->friction.y : (-this->friction.y))) * elapsed;
            }
            //this->movingspeed.y -= (this->movingspeed.y == 0 ? 0 : (this->movingspeed.y > 0 ? this->friction.y : (-this->friction.y))) * elapsed;
            
        }

    public:

        void handleCollision(Rectangle coll) {
            if (check_intersect(coll, hitbox)) {
                //Generic entity collision would go here
                printf("Generic Entity Collision\n");
            }
        }

        void handleEntityCollision(Entity* ent) {
            for (auto& hbox : ent->get_all_hitboxes_pos()) {
                handleCollision(hbox);
            }

        }

        void set_position(olc::vf2d pos) {
            this->position = pos;
        }

        void set_posx(float posx) {
            this->position.x = posx;
        }

        void set_posy(float posy) {
            this->position.y = posy;
        }

        olc::vf2d get_position() {
            return this->position;
        }

        float get_posx() {
            return this->position.x;
        }

        float get_posy() {
            return this->position.y;
        }

        void set_size(olc::vf2d size) {
            this->size = size;
        }
        void set_offset(olc::vf2d spriteoffset) {
            this->offset = spriteoffset;
        }

        olc::vf2d get_size() {
            return this->size;
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
            this->idleanimation.get()->draw_current_frame(position, engine, as_decal);
            engine->DrawRect(this->position+this->hitbox.pos, this->hitbox.size);
        }

        void set_baseheight(int height) {
            this->baseheight = height;
        }

        void set_jumpingspeedstart(float speed) {
            this->starting_jumpingspeed = speed;
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

        olc::vf2d convert_coord_to_local(olc::vf2d worldcoord) {
            return worldcoord + this->position;
        }

        olc::vf2d convert_coord_to_world(olc::vf2d localcoord) {
            return localcoord - this->position;
        }

        void startJump() {
            if (this->jumpingfalling == MovingY::GROUND){
                this->jumpingfalling = MovingY::JUMP;
                this->movingspeed -= olc::vf2d(0, this->starting_jumpingspeed);
                //this->movingacceleration -= olc::vf2d(0, this->starting_jumpingspeed);
                this->current_jumpingspeed = this->starting_jumpingspeed;
            } else {
                printf("NO DOUBLEJUMP!!");
            }

        }

        void stopJump() {
            if (this->jumping){
                this->jumping = false;
                this->current_jumpingspeed = 0;
            }

        }

        void setFalling(bool state) {
            this->falling = state;
        }

        void set_speed(olc::vf2d speed) {
            this->movingspeed = speed;
        }

        void add_speed(olc::vf2d speed) {
            this->movingspeed += speed;
        }

        void set_hitbox(Rectangle newhitbox) {
            this->hitbox = newhitbox;
        }

        Rectangle get_hitbox() {
            return this->hitbox;
        }

        Rectangle get_hitbox_pos() {
            Rectangle tmp = this->hitbox;
            tmp.pos += this->position;
            return tmp;
        }

        std::vector<Rectangle> get_all_hitboxes() {
            std::vector<Rectangle> tmp = {{this->hitbox}};
            return tmp;
        }
        std::vector<Rectangle> get_all_hitboxes_pos() {
            std::vector<Rectangle> tmp = {{this->hitbox}};
            return tmp;
        }

        bool get_phys () {
            return this->is_physic;
        }
        void set_phys (bool physical) {
            this->is_physic = physical;
        }
    };

    enum HitboxTypes {
        GENERIC,
        TOP,
        BOTTOM,
        LEFT,
        RIGHT,

        NUM
    };

    class Character : public Entity {
        std::unique_ptr<AnimatedSprite> runanimation, specialanimation;
        std::array<Rectangle, HitboxTypes::NUM> hitboxes = {};

        public:
            Character() = default;
            Character(std::unique_ptr<AnimatedSprite> sprite) {
                this->idleanimation = std::move(sprite);
            }

            void set_hitbox(Rectangle hitbox, HitboxTypes type) {
                this->hitboxes.at(type)=hitbox;
            }

            void handleCollision(Rectangle coll) {
                for (int i = 0; i < this->hitboxes.size(); i++) {
                    Rectangle hb = this->hitboxes.at(i);
                    hb.pos += this->position;
                    if (check_intersect(coll, hb)) {
                        printf("Character Entity Collision\n");
                        switch (i) {
                            case HitboxTypes::GENERIC:
                                printf("GENERIC\ln");

                                break;

                            case HitboxTypes::TOP:
                                this->jumpingfalling = MovingY::FALL;
                                printf("TOP\n");
                                this->position.y = coll.bottomleft().y;
                                this->movingspeed.y = 2.f;

                                break;

                            case HitboxTypes::BOTTOM:
                                this->jumpingfalling = MovingY::GROUND;
                                printf("BOTTOM\n");
                                this->position.y = coll.topleft().y - this->size.y;
                                this->movingspeed.y = 0;

                                break;

                            case HitboxTypes::LEFT:
                                this->movingspeed.x = std::max(this->movingspeed.x, 6.f);
                                printf("LEFT\n");
                                this->position.x = std::max(coll.topright().x - (this->offset.x + 1), this->position.x);
                                //this->movingspeed.y = 2.f;


                                break;

                            case HitboxTypes::RIGHT:
                                this->movingspeed.x = std::min(this->movingspeed.x, -6.f);
                                printf("RIGHT\n");
                                this->position.x = std::min(coll.topright().x - (this->offset.x + 1), this->position.x);

                                break;

                            default:
                                printf("OTHER\n");
                                break;
                        }
                        //Generic entity collision would go here
                    } else {
                        if (this->jumpingfalling == MovingY::GROUND){
                            //this->jumpingfalling = MovingY::FALL;
                            //printf("NoCollision = FALL if ground previously\n");
                        }

                    }
                }

            }
            void DBGDRAW(olc::PixelGameEngine *engine) {
                for (auto& hb : this->hitboxes) {
                    engine->DrawRect(this->position+hb.pos, hb.size, olc::BLUE);
                }
            }

            void draw_entity(olc::PixelGameEngine *engine, bool as_decal) {
                this->idleanimation.get()->draw_current_frame(position, engine, as_decal);
                engine->DrawRect(this->position+this->hitbox.pos, this->hitbox.size);
                this->DBGDRAW(engine);
            }  

            std::vector<Rectangle> get_all_hitboxes() {
                std::vector<Rectangle> tmp = {{this->hitbox}};
                for (auto& hb : this->hitboxes) {
                    tmp.push_back(hb);
                }
                return tmp;
            }
            std::vector<Rectangle> get_all_hitboxes_pos() {
                std::vector<Rectangle> tmp = {{this->hitbox}};
                for (auto& hb : this->hitboxes) {
                    Rectangle hb_corr {hb};
                    hb_corr.pos += this->position;
                    tmp.push_back(hb_corr);
                }
                return tmp;
            }





    };

    class Player : Character {
        std::unique_ptr<AnimatedSprite> jumpup_anim, jumpdown_anim;

    };

    class Enemy : Character {
        std::unique_ptr<AnimatedSprite> activation;

    };
    
    class Object : Entity {

    };

    class Level {
        std::unique_ptr<olc::Sprite> currsprite;
        std::unique_ptr<olc::Decal> currsprite_dec;
        MapCollisors terrain_collision;
        
        std::unique_ptr<olc::Sprite> preloadsprite;
        std::unique_ptr<olc::Sprite> preload_collision;


        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<Character>> characters;

        olc::vf2d map_size {};
        olc::vf2d camera_size {}, camera_pos{};

        public:    
            Level() = default;

            Level ( std::unique_ptr<olc::Sprite> currsprite,
                    std::unique_ptr<olc::Sprite> collision_bitmap) {
                        this->currsprite = std::move(currsprite);
                        //this->collision_bitmap = std::move(collision_bitmap);
                    }

            void set_collisors(MapCollisors newcollisors) {
                this->terrain_collision = newcollisors;
            }
            
            void compute_collisions() {
                //printf ("computing collisions\n");

                for (auto& ent : this->entities) {
                    if (!(ent.get()->get_phys())) {continue;}
                    for (auto& collider : terrain_collision.get_allcollisors()) {
                        ent.get()->handleCollision(collider);
                    } 
                }

                for (auto& ent : this->characters) {
                    if (!(ent.get()->get_phys())) {continue;}
                    for (auto& collider : terrain_collision.get_allcollisors()) {
                        ent.get()->handleCollision(collider);
                    } 
                }
                //printf("endcollisions\n");
            }

            void get_entityptr (int index, Entity** res) {
                *res = this->entities.at(index).get();
            }

            void get_characterptr (int index, Character** res) {
                *res = this->characters.at(index).get();
            }

            void set_entity(std::unique_ptr<Entity> newent, int index) {
                entities.at(index) = std::move(newent);
            }
            
            void set_entity(std::unique_ptr<Character> newent, int index) {
                characters.at(index) = std::move(newent);
            }

            int add_entity(std::unique_ptr<Entity> newent) {
                int tmp = entities.size();
                entities.push_back(std::move(newent));
                return tmp;
            }

            int add_character(std::unique_ptr<Character> newent) {
                int tmp = characters.size();
                //entities.push_back(std::move(newent));
                characters.push_back(std::move(newent));
                return tmp;
            }

            void update_entities(float elapsed) {
                for (auto& ent : this->entities) {
                    ent.get()->update_character(elapsed);
                }
                for (auto& character : this->characters) {
                    character.get()->update_character(elapsed);
                }
                this->compute_collisions();
            }

            void draw_entities(olc::PixelGameEngine *engine, bool as_decal) {
                for (auto& ent : this->entities) {
                    ent.get()->draw_entity(engine, as_decal);
                }

                for (auto& ent : this->characters) {
                    ent.get()->draw_entity(engine, as_decal);
                }
            }

            void DBGDRAW(olc::PixelGameEngine *engine) {
                this->terrain_collision.DBG_draw_terrainhitboxes(engine);
            }

            
    };
}