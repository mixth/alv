using namespace std;

class Controller : public Worker
{
	public:
	Controller(int *_avoid_result, int *_follow_result, int *_left_right)
	{
		avoid_result = _avoid_result;
		follow_result = _follow_result;
		left_right = _left_right;
	}
	
	void operator () ()
	{
		// readInput() returns: 0 - err, 1 - straight, 2 - left/right, 3 - unknown
		// follow_result has a value of: 0 - unknown, 1 - left, 2 - straight, 3 - right
		
		// FinalResult has a value for: 0 - unknown, 1 - left, 2 - straight, 3 - right
		int err = 0;
		while (true)
		{
			int from_another = readInput();
			int finalResult = 0;
			if (from_another == 0 || from_another == 3)
			{
				cout << "No result from another" << endl;
				finalResult = *follow_result;
			}
			else if (follow_result == 0)
			{
				cout << "No result from this one" << endl;
				if (*left_right == 0)
				{
					// If this node connects with the left camera
					switch (from_another)
					{
						case 1:
							finalResult = 2;
							break;
						case 2:
							finalResult = 3;
					}
				}
				else if (*left_right == 1)
				{
					// If this nodes connects with the right camera
					switch (from_another)
					{
						case 1:
							finalResult = 2;
							break;
						case 2:
							finalResult = 1;
					}
				}
			}
			else if (*follow_result == 2 && from_another == 1) // both agree on straight
			{
				cout << "Both agree on straight" << endl;
				finalResult = 2;
			}
			else // if there is some unexpected case, we will be confidence on main compute
			{
				cout << "Unexpected" << endl;
				finalResult = *follow_result;
			}
			
			if (finalResult > 0)
			{
				if(finalResult == 1) // decided to go left
				{
					err = main_control(5);
				}
				else if(finalResult == 3) // decided to go right
				{
					err = main_control(4);
				}
				else // Go straight
				{
					err = main_control(1);
				}
			}
			
			if (err < 0)
				return;
		}
	}
	/*
	 * single camera for avoider and folower
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
	*/
	private:
	int *avoid_result;
	int *follow_result;
	int *left_right;
};
