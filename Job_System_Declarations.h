#ifndef JOB_SYSTEM_DECLARATIONS_VISIONGL
#define JOB_SYSTEM_DECLARATIONS_VISIONGL

#include<thread>
#include<mutex>
#include<vector>
#include<array>

void Handle_Scene();

#define NUMBER_OF_WORKERS 2u

class Model;

class Controller
{
public:
	Model* Object;
	virtual void Control_Function() {}
	virtual void Initialise_Control(Model* Objectp) { Object = Objectp; }
};

namespace Job_System
{
	bool Working = false;

	bool Part_Time_Work();

	class Job
	{
	public:
		void* Parameters;
		void (*Job_Function)(void*); // This is the actual function that the worker runs

		Job() {}
		Job(void (*Job_Functionp)(void*), void* Parametersp)
		{
			Job_Function = Job_Functionp;
			Parameters = Parametersp;
		}
	};

	class Worker
	{
	public:
		std::vector<Job> Job_Pool;
		std::mutex Job_Pool_Lock;

		uint8_t Worker_Index = 0;

		Worker() {}
	};

	std::array<Worker, NUMBER_OF_WORKERS> Workers;
	std::vector<std::thread> Worker_Threads;

	void Begin_Work(Worker* Worker);

	uint8_t Last_Job_Submitted_Index;

	std::mutex Last_Job_Submitted_Mutex;

	void Submit_Job(Job Task);
}

void Initialise_Job_System();

#endif