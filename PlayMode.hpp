#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	float background_fade = 0.0f;

	//player info:
	glm::vec2 player_at = glm::vec2(0.0f);
	uint8_t player_sprite_index = 36;
	uint8_t player_color_index = 7;
	float shot_delay = 1.0f; //time to charge a shot

	//package info
	glm::vec2 box_at = glm::vec2(0.0f);
	bool holding_box = false;
	uint8_t box_sprite_index = 35;
	uint8_t box_color_index = 4;

	//goal info
	glm::vec2 goal_at = glm::vec2(150.0f, 150.0f);
	uint8_t goal_sprite_index = 34;
	uint8_t goal_color_index = 3;

	bool near_box(glm::vec2 target);
	void grab_box();
	void release_box();
	glm::vec4 shoot();
	glm::vec2 destroy();
	void reset_goal();
	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};


