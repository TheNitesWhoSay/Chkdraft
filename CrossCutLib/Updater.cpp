#include "Updater.h"
#include <chrono>

Updater::Updater(IUpdatable &targetObject) :
	updatableObject(targetObject), continueUpdating(true), intervalBetweenUpdates(60), updateThread(nullptr)
{

}

Updater::~Updater()
{

}

int Updater::GetUpdateInterval()
{
	return intervalBetweenUpdates;
}

void Updater::SetUpdateInterval(int newUpdateInterval)
{
	intervalBetweenUpdates = newUpdateInterval;
}

void LaunchPushUpdates(Updater* updater)
{
	updater->PushUpdates();
}

bool Updater::StartTimedUpdates(int interval)
{
	intervalBetweenUpdates = interval;
	continueUpdating = true;
	updateThread = std::unique_ptr<std::thread>(new std::thread(LaunchPushUpdates, this));
	return true;
}

void Updater::StopTimedUpdates()
{
	continueUpdating = false;
	updateThread->join();
	updateThread = nullptr;
}

void Updater::PushUpdates()
{
	while ( continueUpdating )
	{
		updatableObject.TimedUpdate();
		std::this_thread::sleep_for(std::chrono::milliseconds(intervalBetweenUpdates));
	}
}
