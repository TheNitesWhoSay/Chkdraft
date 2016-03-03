#ifndef UPDATER_H
#define UPDATER_H
#include <atomic>
#include <thread>

/** An object designed with the capacity to be automatically
	updated after certain periods of time */
class IUpdatable // ~Interface
{
	public:
		/** A method used to update parts of your program.
		When working with GUIs or thread sensative objects
		you should invoke their thread to run code */
		virtual void TimedUpdate() = 0;
};

/** A class that sends timed update messages to updatable objects */
class Updater
{
	public:
		Updater(IUpdatable &targetObject);
		~Updater();
		int GetUpdateInterval();
		void SetUpdateInterval(int newUpdateInterval);

		/** Begins sending update messages with the given frequency (in miliseconds) */
		bool StartTimedUpdates(int interval);
		void StopTimedUpdates();

	protected:
		friend void LaunchPushUpdates(Updater* updater);
		void PushUpdates();

	private:
		IUpdatable &updatableObject;
		std::atomic<bool> continueUpdating;
		std::atomic<int> intervalBetweenUpdates;
		std::unique_ptr<std::thread> updateThread;
};

#endif