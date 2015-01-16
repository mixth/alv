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
			if (*avoid_result != -1)
			{
				if (main_control(*avoid_result) < 0)
				{
					return;
				}
			}
			else
			{
				uwait(200);
			}
		}
	}
	
	private:
	int *avoid_result;
	int *follow_result;
}
