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
		int err = 0;
		while (true)
		{
			//usleep(50 * 1000);
			if(*follow_result > 0) // follow when ALV can see the target
			{				
				if(*follow_result == 1) // decided to go left
				{
					err = main_control(5);
				}
				else if(*follow_result == 3) // decided to go right
				{
					err = main_control(4);
				}
				else // Go straight
				{
					err = main_control(1);
				}
			}
			else if (*avoid_result != -1) // avoid when ALV can get the avoid_result but cannot see the target
			{
				if (*avoid_result != 6)
				{
					err = main_control(*avoid_result);
				}
				else
				{
					//cout << "this should be a stop from Avoider" << endl;
				}
			}
			if (err < 0)
			{
				return;
			}
		}
	}
	
	private:
	int *avoid_result;
	int *follow_result;
};
