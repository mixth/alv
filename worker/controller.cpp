using namespace std;

class Controller : public Worker
{
	public:
	Controller(int *_avoid_result, int *_follow_result)
	{
		avoid_result = _avoid_result;
		follow_result = _follow_result;
	}
	void operator () ()
	{
		while (true)
		{
			if (*avoid_result != -1 && *follow_result == -1) // avoid when ALV can get the avoid_result but cannot see the target
			{
				usleep(100 * 1000);
				if (main_control(*avoid_result) < 0)
				{
					return;
				}
			}
			else if(*follow_result != -1) // follow when ALV can see the target
			{
				usleep(100 * 1000);
				if(main_control(*follow_result) < 0)
				{
					return;
				}

			}
		}
	}
	
	private:
	int *avoid_result;
	int *follow_result;
};
