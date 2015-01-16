#include <boost/thread.hpp>

using namespace std;

class Worker
{
	public:
		Worker() {}
	protected:
		static boost::mutex _locker;
	
};
