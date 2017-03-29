#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <vector>
using namespace std;
#include <assert.h>
#include<stdlib.h>

template<typename T>
class vector2d : private vector<vector<T> >
{
public:
    vector2d():_row(0),_col(0)
    {
    }
    vector2d(const uint row,const uint col)
    {
        if(row <= 0 || col <= 0)
        {
            _row = row;
            _col = col;
            return;
        }
        resize(row, col);
    }
    ~vector2d()
    {
    }
    vector2d(const vector2d& vec2d) : vector<vector<T> >(vec2d)
    {
        _row = vec2d._row;
        _col = vec2d._col;
    }

    const T& operator()(const int row,const int col) const
    {
        assert(row <  (int)_row && col <  (int)_col);
        return (*this)[row][col];
    }

    T& operator()(const int row,const int col)
    {
        assert(row < (int)_row && col < (int)_col);
        return (*this)[row][col];
    }

    vector2d& operator =(const  vector2d& vec2d)
    {
        if(this != &vec2d)
        {
            vector<vector<T> >::operator =(vec2d);
            this->_row = vec2d._row;
            this->_col = vec2d._col;
        }
        return *this;
    }
    void resizeFill(const int row,const int col,const T& fill)
    {
        if(row <  0 || col < 0)
        {
            return;
        }
        vector<vector<T> >::resize(row);
        typename vector<vector<T> >::iterator iterPa = this->begin();
        for(;iterPa != this->end(); iterPa++)
        {
            vector<T>& tmpVec =  *iterPa;
            tmpVec.resize(col);
            for(typename vector<T>::iterator iterEach = tmpVec.begin();
                iterEach != tmpVec.end(); ++iterEach)
            {
                T& tmpData = *iterEach;
                tmpData = fill;
            }
        }

        _row = row;
        _col = col;
    }

    void resize(const int row,const int col)
    {
        if(row <  0 || col < 0)
        {
            return;
        }


       /* if(row <  _row)//判断一个即可，这里为方阵
        {
            return;
        }*/

       /* if(row > this->capacity())
        {
            vector<vector<T> >::reserve(row * 2);
            typename vector<vector<T> >::iterator iterPa = this->begin();
            for(;iterPa != this->end(); iterPa++)
            {
                iterPa->reserve(col*2);
            }
        }*/
        vector<vector<T> >::resize(row);
        typename vector<vector<T> >::iterator iterPa = this->begin();
        for(;iterPa != this->end(); iterPa++)
        {
            iterPa->resize(col);
        }

        _row = row;
        _col = col;
    }

    void clear()
    {
        vector<vector<T> >::clear();
        _row = 0;
        _col = 0;
    }
    uint getRow() const
    {
        return _row;
    }
    uint getCol() const
    {
        return _col;
    }


private:
    uint _row;
    uint _col;
};
#endif // VECTOR2D_H
