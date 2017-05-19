
#include "gamestate_intro.hpp"
#include "gamestate_test3d.hpp"
#include "gamestate_headpose.hpp"

void init_gamestates( gamestate::game_state_system<object_repo>* gamestates )
	{
	// register all states
	gamestates->add<gamestate_intro, &gamestate_intro::update>();
	gamestates->add<gamestate_test3d, &gamestate_test3d::update>();
	gamestates->add<gamestate_headpose, &gamestate_headpose::update>();

	// set initial game state
	gamestates->set<gamestate_headpose>();
	}
