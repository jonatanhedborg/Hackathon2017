struct gamestate_intro; 
struct gamestate_test3d;
struct gamestate_headpose;
struct gamestate_ingame;

#include "gamestate_ingame.hpp"
#include "gamestate_intro.hpp"
#include "gamestate_test3d.hpp"
#include "gamestate_headpose.hpp"

void init_gamestates( gamestate::game_state_system<object_repo>* gamestates )
	{
	// register all states
	gamestates->add<gamestate_intro, &gamestate_intro::update>();
	gamestates->add<gamestate_test3d, &gamestate_test3d::update>();
	gamestates->add<gamestate_headpose, &gamestate_headpose::update>();
	gamestates->add<gamestate_ingame, &gamestate_ingame::update>();

	// set initial game state
	#ifdef _DEBUG
	gamestates->set<gamestate_ingame>();
	#else // _DEBUG
	gamestates->set<gamestate_intro>();
	#endif	
	}
