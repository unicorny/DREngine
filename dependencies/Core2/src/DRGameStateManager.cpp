#include "Core2Main.h"

DRGameStateManager::DRGameStateManager()
{

}

DRGameStateManager::~DRGameStateManager()
{

}

void DRGameStateManager::addConstantState(DRGameStatePtr newGameState, const char* name)
{
    mConstantGameStates.insert(CONSTANT_GAME_STATE_ENTRY(DRMakeStringHash(name), newGameState));
}

DRReturn DRGameStateManager::pushState(const char* name)
{
    DHASH id = DRMakeStringHash(name);
    if(mConstantGameStates.find(id) != mConstantGameStates.end())
    {
        pushState(mConstantGameStates[id]);
        return DR_OK;
    }
    return DR_ERROR;
}

void DRGameStateManager::pushState(DRGameStatePtr gameState)
{
    mGameStates.push_back(gameState);
}

void DRGameStateManager::popState()
{
	if(mGameStates.size() > 0)
		mGameStates.pop_back();
}

DRReturn DRGameStateManager::move(float fTime)
{
	if(mGameStates.size() > 0)
		return mGameStates.back()->move(fTime);
	else LOG_ERROR("no active GameState", DR_ERROR);
}

DRReturn DRGameStateManager::render(float fTime)
{
    bool first = true;
    for(std::list<DRGameStatePtr>::iterator it = mGameStates.begin(); it != mGameStates.end(); it++)
    {
        (*it)->setFirstState(first);
        first = false;
        if((*it)->render(fTime)) LOG_ERROR("error by render GameState", DR_ERROR);
        if((*it)->getTransparency() >= 1.0f)
            break;
    }
    return DR_OK;
}
