#ifndef GWSingleton_h__
#define GWSingleton_h__

template <typename T> class GWSingleton
{
protected:
	GWSingleton(){}

public:
	static T& Instance()
	{
		static T singleton;

		return singleton;
	}

};

#endif // GWSingleton_h__