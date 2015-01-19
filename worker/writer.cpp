#include <boost/thread.hpp>

using namespace std;

class Writer : public Worker
{
	public:
		Writer (IplImage *_y, Mat *_mask)
		{
			y = _y;
			mask = _mask;
		}
		void operator () () 
		{
			_locker.lock();
			*mask = Mat(y);
			_locker.unlock();
			//cout << "write new value to mask" << endl;
			return;
		}
	private:
		IplImage *y;
		Mat *mask;
};
