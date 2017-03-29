#ifndef MAPTREE_H
#define MAPTREE_H
#include"../comm/define.h"
#include <map>
using namespace std;

template<int LEVEL,typename K,typename T>
class MapTree
{
public:
    typedef MapTree<LEVEL-1,K,T> Type;
    typedef map<K,Type*> MapType;
    typedef typename map<K,Type*>::iterator MapIterator;
    typedef typename map<K,Type*>::const_iterator ConstMapIterator;
    MapTree(map<K,T>& baseData,map<K,int>& baseRef) :
        _baseData(baseData),
        _baseRef(baseRef)
    {
    }
    ~MapTree()
    {
//        typename map<K,Type*>::iterator iterChild =   _mapChild.begin();
//        for(;iterChild != _mapChild.end(); iterChild++)
//        {
//            Type* tmpItem = iterChild->second;
//            if(tmpItem)
//            {
//                delete tmpItem;
//            }
//        }
//        _mapChild.clear();
        clearChild();
    }
    void clearChild()
    {
        typename map<K,Type*>::iterator iterChild =   _mapChild.begin();
        for(;iterChild != _mapChild.end(); iterChild++)
        {
            Type* tmpItem = iterChild->second;
            if(tmpItem)
            {
                tmpItem->clearChild();
                delete tmpItem;
            }
        }
        _mapChild.clear();
    }
    bool delChildByIter(MapIterator  iter)
    {
        Type* tmpItem = iter->second;
        tmpItem->clearChild();
        _mapChild.erase(iter);
        delete tmpItem;
        return true;
    }

    bool delChild(const K& key)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end())
        {
            Type* tmpItem = iterChild->second;
            tmpItem->clearChild();
            _mapChild.erase(iterChild);
            delete tmpItem;
            return true;
        }
        return false;
    }

    const map<K,Type*>& getChild() const
    {
        return _mapChild;
    }
    map<K,Type*>& getChild()
    {
        return _mapChild;
    }
    Type* getChildSingle(K key)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end())
        {
            return iterChild->second;
        }
        else
        {
            return NULL;
        }
    }

    Type* allocChild(K key)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end() && NULL != iterChild->second)
        {
            return iterChild->second;
        }
        else
        {
            Type* childItem = new Type(_baseData,_baseRef);
            _mapChild[key] = childItem;
            return childItem;
        }
    }

    Type* addChild(K key)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end())
        {
            return iterChild->second;
        }
        else
        {
            Type* childItem = new Type(_baseData,_baseRef);
            _mapChild.insert(pair<K,Type*>(key,childItem));
            return childItem;
        }
    }

private:
    map<K,Type*> _mapChild;

    map<K,T>& _baseData;
    map<K,int>& _baseRef;
};

//类特化
template<typename K,typename T>
class MapTree<1,K,T>
{
public:
    enum
    {
        RET_NONE,
        RET_DELINDEX,
        RET_DELDATA
    };
    typedef T Type;
    typedef map<K,T*> MapType;
    typedef typename map<K,T*>::iterator MapIterator;
    typedef typename map<K,T*>::const_iterator ConstMapIterator;
    MapTree(map<K,T>& baseData,map<K,int>& baseRef) :
        _baseData(baseData),
        _baseRef(baseRef)
    {
    }

    void clearChild()
    {
        //删記数
        typename map<K,Type*>::iterator iterChild =  _mapChild.begin();
        for(;iterChild != _mapChild.end();iterChild++)
        {
            const K& key = iterChild->first;
            //删記数
             typename map<K,int>::iterator iterRef = _baseRef.find(key);
            if(iterRef != _baseRef.end())
            {
                iterRef->second -= 1;
                if(iterRef->second < 2)
                {
                    _baseRef.erase(iterRef);
                }
            }
            else
            {
                _baseData.erase(key);
            }
        }
        _mapChild.clear();
    }

    map<K,Type*>& getChild()
    {
        return _mapChild;
    }
    const map<K,Type*>& getChild() const
    {
        return _mapChild;
    }
    Type* getChildSingle(const K& key)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end())
        {
            return iterChild->second;
        }
        else
        {
            return NULL;
        }
    }
    Type* allocChild(K key,bool bExist = false)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end() && NULL != iterChild->second)
        {
            return iterChild->second;
        }
        else
        {
            typename map<K,T>::iterator iterData = _baseData.find(key);
            if(iterData != _baseData.end())
            {
                T& tmpData = iterData->second;
                _mapChild[key] = &tmpData;
                typename map<K,int>::iterator iterRef = _baseRef.find(key);
                if(iterRef != _baseRef.end())
                {
                    iterRef->second += 1;
                }
                else
                {
                    _baseRef.insert(pair<K,int>(key,1));
                }
                return &tmpData;
            }
            else
            {
                if(bExist)
                {
                    return NULL;
                }
                else
                {
                    T& tmpData =  _baseData[key];
                    _mapChild[key] = &tmpData;
                    return &tmpData;
                }
            }
        }
    }
    Type* addChild(const K& key,bool bExist = false)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end())
        {
            return iterChild->second;
        }
        else
        {
            typename map<K,T>::iterator iterData = _baseData.find(key);
            if(iterData != _baseData.end())
            {
                T& tmpData = iterData->second;
                _mapChild.insert(pair<K,Type*>(key,&tmpData));
                typename map<K,int>::iterator iterRef = _baseRef.find(key);
                if(iterRef != _baseRef.end())
                {
                    iterRef->second += 1;
                }
                else
                {
                    _baseRef.insert(pair<K,int>(key,2));
                }
                return &tmpData;
            }
            else
            {
                if(bExist)
                {
                    return NULL;
                }
                else
                {
                    T& tmpData =  _baseData[key];
                    _mapChild.insert(pair<K,Type*>(key,&tmpData));
                    return &tmpData;
                }
            }
        }
    }
    //0 表示不存在此key,
    //1 表示存在此key删除此key,
    //2表示存在此key，且数据鏈唯一，删除key,删除数据
    int delChild(const K& key)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end())
        {
            _mapChild.erase(iterChild);
            //删記数
             typename map<K,int>::iterator iterRef = _baseRef.find(key);
            if(iterRef != _baseRef.end())
            {
                iterRef->second -= 1;
                if(iterRef->second < 2)
                {
                    _baseRef.erase(iterRef);
                }
                return RET_DELINDEX;
            }
            else
            {
                _baseData.erase(key);
                return RET_DELDATA;
            }
        }
        return RET_NONE;
    }

    bool delChildByIter(MapIterator  iter)
    {
        const K  key = iter->first;
        _mapChild.erase(iter);
        //删記数
         typename map<K,int>::iterator iterRef = _baseRef.find(key);
        if(iterRef != _baseRef.end())
        {
            iterRef->second -= 1;
            if(iterRef->second < 2)
            {
                _baseRef.erase(iterRef);
            }
            return RET_DELINDEX;
        }
        else
        {
            _baseData.erase(key);
            return RET_DELDATA;
        }
    }


    Type* addChild(const K& key,const T& data)
    {
        typename map<K,Type*>::iterator iterChild =  _mapChild.find(key);
        if(iterChild != _mapChild.end())
        {
            Type* itemData = iterChild->second;
            if(&data != itemData)
            {
                *itemData = data;
            }
            return itemData;
        }
        else
        {
            typename map<K,T>::iterator iterData = _baseData.find(key);
            if(iterData != _baseData.end())
            {
                T& tmpData = iterData->second;
                if(&tmpData != &data)
                {
                    tmpData = data;
                }
                _mapChild.insert(pair<K,Type*>(key,&tmpData));
                typename map<K,int>::iterator iterRef = _baseRef.find(key);
                if(iterRef != _baseRef.end())
                {
                    iterRef->second += 1;
                }
                else
                {
                    _baseRef.insert(pair<K,int>(key,2));
                }
                return &tmpData;
            }
            else
            {
               T& tmpData =  _baseData[key];
               tmpData = data;
               _mapChild.insert(pair<K,Type*>(key,&tmpData));
               return &tmpData;
            }
        }
    }
private:
    map<K,Type*> _mapChild;
    map<K,T>& _baseData;
    map<K,int>& _baseRef;
};

#endif // MAPTREE_H
