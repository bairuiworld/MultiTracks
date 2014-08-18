#ifndef __MULTITRACKS_PROPERTIES_H__
#define __MULTITRACKS_PROPERTIES_H__

#include <cassert>
#include <map>
#include <string>
#include <typeindex>

namespace mt {

class Properties
{
	class Placeholder
	{
	public:
		virtual ~Placeholder() {}
		virtual std::type_index GetTypeIndex() const = 0;
	};

	template <class T>
	class Holder : public Placeholder
	{
	public:
		Holder(const T& value) : mValue(value) {}
		virtual ~Holder() {}
		virtual std::type_index GetTypeIndex() const { return typeid(T); }
		T mValue;
	};

	using PropertyMap = std::map<std::string, Placeholder*>;

public:
	Properties(Properties* parent = nullptr) : mParent(parent), mChild(nullptr) {}
	~Properties()
	{
		for(PropertyMap::iterator it = mProperties.begin(); it != mProperties.end(); ++it)
			delete it->second;
		delete mChild;
	}

	void Push()
	{
		if(mChild) mChild->Push();
		else mChild = new Properties();
	}

	void Pop()
	{
		if(!mChild) return;
		if(mChild->mChild == nullptr)
		{
			delete mChild;
			mChild = nullptr;
		}
		else
			mChild->Pop();
	}

	bool Exists(std::string key) const
	{
		if(mChild && mChild->Exists(key)) return true;
		PropertyMap::const_iterator it = mProperties.find(key);
		return it != mProperties.end();
	}

	template <class T>
	Properties& Set(std::string key, T prop)
	{
		if(mChild) return mChild->Set(key, prop);
		PropertyMap::const_iterator it = mProperties.find(key);
		if(it != mProperties.end())
			delete it->second;
		mProperties.insert({key, new Holder<T>(prop)});
		return *this;
	}

	template <class T>
	const T& Get(std::string key) const
	{
		if(mChild && mChild->Exists(key)) return mChild->Get<T>(key);
		PropertyMap::const_iterator it = mProperties.find(key);
		if(it == mProperties.end())
		{
			if(!mParent) assert(0 && "Property does not exist");
			return mParent->Get<T>(key);
		}
		if(it->second->GetTypeIndex() != typeid(T))
			assert(0 && "Bad cast");
		return static_cast<Holder<T>*>(it->second)->mValue;
	}

	template <class T>
	const T& Get(std::string key, const T& bydefault) const
	{
		if(mChild && mChild->Exists(key)) return mChild->Get<T>(key, bydefault);
		PropertyMap::const_iterator it = mProperties.find(key);
		if(it == mProperties.end())
		{
			if(!mParent) return bydefault;
			return mParent->Get<T>(key, bydefault);
		}
		if(it->second->GetTypeIndex() != typeid(T))
			assert(0 && "Bad cast");
		return static_cast<Holder<T>*>(it->second)->mValue;
	}

private:
	Properties* mParent;
	Properties* mChild;
	PropertyMap mProperties;
};

template <class T>
class Property
{
public:
	Property() : mIsSet(false), mParent(nullptr), mChild(nullptr) {}
	Property(const Property<T>& parent) : mIsSet(false), mChild(nullptr) { SetParent(&parent); }
	
	~Property()
	{
		if(mChild != nullptr) delete mChild;
	}

	void SetParent(const Property<T>* parent) { mParent = parent; }
	const Property* GetParent() const { return mParent; }

	void Push(const T& value)
	{
		if(mChild != nullptr) mChild->Push();
		else mChild = new Property<T>();
		(*mChild)(value);
	}

	void Pop()
	{
		if(mChild == nullptr) return;
		if(mChild->mChild == nullptr)
		{
			delete mChild;
			mChild = nullptr;
		}
		else
			mChild->Pop();
	}				

	T operator ()() const
	{
		if(mChild != nullptr) return (*mChild)();
		if(mIsSet) return mValue;
		if(mParent != nullptr) return (*mParent)();
		return mValue;
	}

	Property& operator ()(const T& value)
	{
		if(mChild != nullptr) return (*mChild)(value);
		mValue = value;
		mIsSet = true;
		return *this;
	}

	void Unset() { mIsSet = false; }

	bool IsSet()
	{
		return mIsSet || (mParent != nullptr && mParent->IsSet());
	}

private:
	bool			mIsSet;
	const Property*	mParent;
	Property*		mChild;
	T				mValue;
};

}

#endif // __MULTITRACKS_PROPERTIES_H__