#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

#include "load_save_png.hpp"

#include "PPU466.hpp"

#define GRAB_DISTANCE 8.0f;


PPU466::Tile gen_tile_from_png(glm::uvec2 size, std::vector< glm::u8vec4 > data) {
	glm::uvec2 step_size = glm::uvec2(0, 0);
	step_size.x = size.x / 8;
	step_size.y = size.y / 8;
	
	int color_select0 = 1;
	int color_select1 = 1;
	PPU466::Tile gen_tile;
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			
			if (data[i * step_size.x + j * step_size.y * size.x].w == 0) {
				color_select0 = 0;
				color_select1 = 0;
			}
			else if (data[i * step_size.x + j * step_size.y * size.x].x > data[i * step_size.x + j * step_size.y * size.x].y &&
				     data[i * step_size.x + j * step_size.y * size.x].x > data[i * step_size.x + j * step_size.y * size.x].z) {
				color_select0 = 1;
				color_select1 = 0;
			}
			else if (data[i * step_size.x + j * step_size.y * size.x].y > data[i * step_size.x + j * step_size.y * size.x].x &&
				     data[i * step_size.x + j * step_size.y * size.x].y > data[i * step_size.x + j * step_size.y * size.x].z) {
				color_select0 = 0;
				color_select1 = 1;
			}
			else {
				color_select0 = 1;
				color_select1 = 1;
			}
			gen_tile.bit0[j] = gen_tile.bit0[j] | color_select0 << i;
			gen_tile.bit0[j] = gen_tile.bit0[j] | color_select1 << i;
		}
	}
	return gen_tile;
}

PlayMode::PlayMode() {

	glm::uvec2 player_sprite_png_size, box_sprite_png_size, goal_sprite_png_size, map_sprite_png_size;
	std::vector< glm::u8vec4 > player_sprite_png_data, box_sprite_png_data, goal_sprite_png_data, map_sprite_png_data;

	load_png("test_sprite3.png", &player_sprite_png_size, &player_sprite_png_data, LowerLeftOrigin);
	load_png("test_sprite2.png", &box_sprite_png_size, &box_sprite_png_data, LowerLeftOrigin);
	load_png("test_sprite.png", &goal_sprite_png_size, &goal_sprite_png_data, LowerLeftOrigin);
	load_png("map.png", &map_sprite_png_size, &map_sprite_png_data, LowerLeftOrigin);



	ppu.tile_table[36] = gen_tile_from_png(player_sprite_png_size, player_sprite_png_data);
	ppu.tile_table[35] = gen_tile_from_png(box_sprite_png_size, box_sprite_png_data);
	ppu.tile_table[34] = gen_tile_from_png(goal_sprite_png_size, goal_sprite_png_data);
	ppu.tile_table[0] = gen_tile_from_png(map_sprite_png_size, map_sprite_png_data);

	
	//map pallet
	ppu.palette_table[0] = {
		glm::u8vec4(0xf0, 0xa0, 0xa0, 0xff),
		glm::u8vec4(0xa0, 0xf0, 0xa0, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0xa0, 0xa0, 0xf0, 0xff),
	};

	//box pallet
	ppu.palette_table[4] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0xa0, 0xa0, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0xff, 0x00, 0x00),
	};

	//goal pallet
	ppu.palette_table[3] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x0f, 0x0f, 0x00, 0xff),
		glm::u8vec4(0xa0, 0xa0, 0x00, 0xff),
		glm::u8vec4(0xa0, 0x00, 0x00, 0xff),
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0xff, 0x00, 0x00),
		glm::u8vec4(0xff, 0xff, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for alternate player color:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0xff, 0xff, 0xff, 0xff),
		glm::u8vec4(0x00, 0xff, 0x00, 0xff),
	};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		}
		
	}

	return false;
}


void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	// (will be used to set background color)
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);

	float PlayerSpeed = holding_box ? 35.0f : 50.0f;
	if (left.pressed) {
		player_at.x -= PlayerSpeed * elapsed;
	}
	else if (right.pressed) { //elif so you can only move in one direction at a time
		player_at.x += PlayerSpeed * elapsed;
	}
	else if (down.pressed) {
		player_at.y -= PlayerSpeed * elapsed;
		player_sprite_index = 36;
	}
	else if (up.pressed) {
		player_at.y += PlayerSpeed * elapsed;
		player_sprite_index = 36;
	}
	if (space.pressed) {
		if (near_box(player_at)) {
			grab_box();
		}
		else {
			//shoot();
		}
		
	}
	else {
		release_box();
		if (near_box(goal_at)) {
			reset_goal();
		}
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	space.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color
	ppu.background_color = glm::u8vec4(0xff, 0xff, 0xff, 0xff);
		
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			ppu.background[x + PPU466::BackgroundWidth * y] = 0;// ((x + y) % 16);
		}
	}

	//background scroll:
	ppu.background_position.x = int32_t(-0.5f * player_at.x);
	ppu.background_position.y = int32_t(-0.5f * player_at.y);

	

	//Box Sprite
	ppu.sprites[1].x = int32_t(box_at.x);
	ppu.sprites[1].y = int32_t(box_at.y);
	ppu.sprites[1].index = box_sprite_index;
	ppu.sprites[1].attributes = box_color_index;

	//Goal Sprite
	ppu.sprites[2].x = int32_t(goal_at.x);
	ppu.sprites[2].y = int32_t(goal_at.y);
	ppu.sprites[2].index = goal_sprite_index;
	ppu.sprites[2].attributes = goal_color_index;

	//some other misc sprites:
	for (uint32_t i = 3; i < 63; ++i) {
		float amt = (i + 2.0f * background_fade) / 62.0f;
		ppu.sprites[i].x = 250; 
		ppu.sprites[i].y = 250; 
		ppu.sprites[i].index = 32;
		ppu.sprites[i].attributes = 6;
		
	}

	//player sprite:
	ppu.sprites[0].x = int32_t(player_at.x);
	ppu.sprites[0].y = int32_t(player_at.y);
	ppu.sprites[0].index = player_sprite_index;
	ppu.sprites[0].attributes = player_color_index;

	//--- actually draw ---
	ppu.draw(drawable_size);
}

// Is the player near the box?
bool PlayMode::near_box(glm::vec2 target)
{
	return glm::distance(target, box_at) < GRAB_DISTANCE;
}

// Pick up the box
void PlayMode::grab_box()
{
	box_at = player_at;
	player_color_index = 6;
	holding_box = true;
}

// release the box
void PlayMode::release_box()
{
	player_color_index = 7;
	holding_box = false;
}

void PlayMode::reset_goal()
{
	static std::mt19937 mt;

	goal_at.x = (mt() / (float)(mt.max())) * 235.0f;
	goal_at.y = (mt() / (float)(mt.max())) * 235.0f;

	box_at.x = (mt() / (float)(mt.max())) * 235.0f;
	box_at.y = (mt() / (float)(mt.max())) * 235.0f;
}



