/*
LICENSE

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The Software will not be used to operate or support nuclear facilities, weapons, life support or other mission critical application where human life or property may be at stake and understand that the Software is not designed for such purposes. The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _KD_TREE_H_
#define _KD_TREE_H_

#include <vector>
#include <functional>
#include <queue>

template<typename PointClass, unsigned int DIM, typename RadiusType>
class KDTree
{
public:
	typedef struct SplittingPlane
	{
		RadiusType distance;

		int left;
		int right;
		unsigned int orgIndex;
		unsigned char splitPlane;

	} SplittingPlane;
	
	typedef struct BalanceDescriptor
	{
		unsigned int _start;
		unsigned int _end;
		int _depth;
		SplittingPlane * _leftParent;
		SplittingPlane * _rightParent;
		
	public:
		BalanceDescriptor()
		:_leftParent(nullptr)
		,_rightParent(nullptr)
		,_start(0xFFFFFFFF)
		,_end(0xFFFFFFFF)
		,_depth(0xFFFFFFFF)
		{
			
		}
		
		BalanceDescriptor(unsigned int start, unsigned int end, int depth, SplittingPlane * leftParent, SplittingPlane * rightParent)
		:_start(start)
		,_end(end)
		,_depth(depth)
		,_leftParent(leftParent)
		,_rightParent(rightParent) {}
		
	} BalanceDescriptor;
	

public:
	KDTree()
		:_root(-1)
	,_pointAdded(true)
	{}

	KDTree(unsigned int size)
		:_root(-1)
		,_pointAdded(true)
	{
		reserve(size);
	}

	void clear()
	{
		_points.clear();
		_splittingPlanes.clear();
		_root = -1;
		_pointAdded = true;
	}

	size_t getNumPoints() const { return _points.size(); }
	inline std::vector<PointClass> & getPoints() { return _points; }
	inline const std::vector<PointClass> & getPoints() const { return _points; }
	inline const PointClass & getPoint(const unsigned int index) const { return _points[index]; }
	inline PointClass getPoint(const unsigned int index) { return _points[index]; }
	
	void addPoints(const std::vector<PointClass> & points)
	{
		const size_t requiredSize = points.size() + _points.size();
		if (_points.capacity() < requiredSize)
			_points.reserve((requiredSize+1)*2);

		_points.insert(_points.end(), points.begin(), points.end());
		_pointAdded = true;
	}

	void addPoint(const PointClass & point)
	{
		_points.push_back(point);
		_pointAdded = true;
	}

	void reserve(unsigned int size)
	{
		if (_points.capacity() < size)
			_points.reserve(size);

		if (_splittingPlanes.capacity() < size)
			_splittingPlanes.reserve(size);
	}
	
	inline void outside(const PointClass & center, RadiusType radius, std::vector<unsigned int> & indices)
	{
		_searchStack.clear();
		outside(_searchStack, center, radius, indices);
	}

	inline void nearestNeighbours(const PointClass & center, unsigned int count, std::vector<unsigned int> & indices, bool wrapSearch = false, const PointClass & wrapDimensions = PointClass())
	{
		if (!wrapSearch)
		{
			_searchStack.clear();
			nearestNeighbours(_searchStack, center, count, indices);
		}
		else
		{
			assert(0); // TODO: fix this method. Until then, use a specialization. They are faster anyway!
		}
	}
	
	inline void inside(const PointClass & center, RadiusType radius, std::vector<unsigned int> & indices, bool wrapSearch = false, const PointClass & wrapDimensions = PointClass())
	{
		if (!wrapSearch)
		{
			_searchStack.clear();
			inside(_searchStack, center, radius, indices);
		}
		else
		{
			assert(0); // TODO: fix this method. Until then, use a specialization. They are faster anyway!
		}
	}
	
	inline void balance()
	{
		_balancingStack.clear();
		balance(_balancingStack);
	}
	

private:
	std::vector<PointClass> _points;
	std::vector<SplittingPlane> _splittingPlanes;
	int _root;
	
	bool _pointAdded;

    struct PriorityItem
    {
        unsigned int _index;
        RadiusType _distSquared;

        PriorityItem()
        :_index(0xFFFFFFFF)
        ,_distSquared(0) {}

        PriorityItem(unsigned int index, RadiusType distSquared)
        :_index(index)
        ,_distSquared(distSquared) {}

        PriorityItem(const PriorityItem & src)
        :_index(src._index)
        ,_distSquared(src._distSquared) {}


        bool operator < (const PriorityItem & right) const
        {
            return _distSquared < right._distSquared;
        }

        PriorityItem & operator = (const PriorityItem & right)
        {
            _index = right._index;
            _distSquared = right._distSquared;
            return *this;
        }

    };

    class PriorityQueue
    {
    public:
        PriorityQueue(unsigned int maxSize)
        :_maxSize(maxSize)
        ,_count(0)
        ,_items(maxSize )
        {
        }

        void clear()
        {
            _count = 0;
        }

        void insert(const PriorityItem & item)
        {
            // insert it at the right place
            if(_items.empty())
            {
                _items.push_back(item);
            }
            else
            {
                unsigned int i=0;
                unsigned int newPlace = _count;
                for( ; i <  _count ; i++)
                {
                    if(_items[i]._distSquared > item._distSquared)
                    {
                        newPlace = i;
                        break;
                    }
                }

                if(_count < _maxSize)
                    _count++;

                if(newPlace < _count)
                {
                    for(i=_count-1 ; i > newPlace ; i--)
                    {
                        _items[i] = _items[i-1];
                    }
                    _items[newPlace] = item;
                }

            }
        }

        int size() const
        {
            return (int)_count;
        }

        PriorityItem & operator [](int index) { return _items[index]; }
        const PriorityItem & operator [](int index) const { return _items[index]; }

    private:
        std::vector<PriorityItem> _items;
        unsigned int _maxSize;
        unsigned int _count;
    };

	
protected:
	bool compare(const SplittingPlane &a, const SplittingPlane &b, int axis)
	{
		return _points[a.orgIndex][axis] < _points[b.orgIndex][axis];
	}
	
	void balance(std::vector<BalanceDescriptor> & balancingStack)
	{
		if(_points.empty())
			return;
		
		if(_splittingPlanes.size()!=_points.size())
			_splittingPlanes.resize(_points.size());
		
		if(_pointAdded)
		{
			for (unsigned int i = 0; i < _splittingPlanes.size(); i++)
			{
				_splittingPlanes[i].left = -1;
				_splittingPlanes[i].right = -1;
				_splittingPlanes[i].orgIndex = i;
			}
			_pointAdded = false;
		}
		
		
		int balancingIndex = 0;
		if(balancingStack.size() < _points.size())
			balancingStack.resize(_points.size());
		
		balancingStack[balancingIndex] = {0, (unsigned int)_splittingPlanes.size() - 1, 0, nullptr, nullptr};
		
		
		_root = -1;
		while(balancingIndex>=0)
		{
			const BalanceDescriptor descriptor = std::move(balancingStack[balancingIndex--]);
			
			if (descriptor._start > descriptor._end)
				continue;
			
			int splitAxis = descriptor._depth % DIM;
			
			unsigned int median = (descriptor._end + descriptor._start) / 2;
			std::nth_element((&_splittingPlanes[0]) + descriptor._start, (&_splittingPlanes[0]) + median, (&_splittingPlanes[0]) + descriptor._end + 1, [splitAxis,this](const SplittingPlane &a, const SplittingPlane &b) { return compare(a, b, splitAxis); });
			if (_root == -1)
				_root = median;
			
			const RadiusType distance = - _points[(_splittingPlanes[median].orgIndex)][splitAxis];
			_splittingPlanes[median].distance = distance;
			_splittingPlanes[median].splitPlane = splitAxis;
			_splittingPlanes[median].left = -1;
			_splittingPlanes[median].right = -1;
			if (descriptor._leftParent != nullptr)
				descriptor._leftParent->left = median;
			if (descriptor._rightParent != nullptr)
				descriptor._rightParent->right = median;
			
			if (median != 0)
				balancingStack[++balancingIndex] = {descriptor._start, median-1, descriptor._depth+1, &_splittingPlanes[median], nullptr};
			balancingStack[++balancingIndex] = {median+1, descriptor._end, descriptor._depth+1, nullptr, &_splittingPlanes[median]};
		}
		
	}
	
	inline void inside(std::vector<int> & searchStack, const PointClass & center, RadiusType radius, std::vector<unsigned int> & indices)
	{
		if(_points.empty() || _root == -1)
			return;
		
		int searchIndex=0;
		
		if(searchStack.size() < _points.size())
			searchStack.resize(_points.size());
		
		searchStack[searchIndex] = _root;
		
		while(searchIndex>=0)
		{
			int nodeIndex = searchStack[searchIndex--];
			if (nodeIndex < 0)
				continue;
			
			
			const SplittingPlane * node = &_splittingPlanes[nodeIndex];
			
			
			const RadiusType signedDistanceToPlane = center[node->splitPlane] + node->distance;
			const RadiusType absDistance = std::abs(signedDistanceToPlane);
			
			const PointClass & point = _points[node->orgIndex];
			const PointClass dir = point - center;
			
			RadiusType squaredDistance = 0;
			for (int i = 0; i < DIM; i++)
				squaredDistance += dir[i] * dir[i];
			
			if (squaredDistance <= radius * radius)
				indices.push_back(node->orgIndex);
			
			// traverse towards root
			if (signedDistanceToPlane < 0)
			{
				if(node->left>=0)
					searchStack[++searchIndex] = node->left;
				if(absDistance < radius + EPSILON && node->right>=0)
					searchStack[++searchIndex] = node->right;
			}
			else if (signedDistanceToPlane > 0)
			{
				if(node->right >=0)
					searchStack[++searchIndex] = node->right;
				if (absDistance < radius + EPSILON && node->left>=0)
					searchStack[++searchIndex] = node->left;
			}
			else
			{
				if(node->left>=0)
					searchStack[++searchIndex] = node->left;
				if(node->right>=0)
					searchStack[++searchIndex] = node->right;
			}
		}
		
	}
	
	void nearestNeighbours(std::vector<int> & searchStack, const PointClass & center, unsigned int count, std::vector<unsigned int> & indices)
	{
		if(_points.empty() || _root == -1)
			return;
		
		PriorityQueue _priorityQueue(count);
		
		auto radius = [count, &_priorityQueue]()
		{
			if(_priorityQueue.size() < count)
				return FLT_MAX;
			
			return _priorityQueue[count-1]._distSquared;
		};
		
		if(searchStack.size() < _points.size()+2)
			searchStack.resize(_points.size()+2);
		
		int searchIndex=0;
		searchStack[searchIndex] = _root;
		
		while(searchIndex>=0)
		{
			int nodeIndex = searchStack[searchIndex--];
			if (nodeIndex < 0)
				continue;
			
			const SplittingPlane * node = &_splittingPlanes[nodeIndex];
			
			const RadiusType signedDistanceToPlane = center[node->splitPlane] + node->distance;
			const RadiusType absDistance = std::abs(signedDistanceToPlane);
			
			const PointClass & point = _points[node->orgIndex];
			const PointClass dir = point - center;
			
			RadiusType squaredDistance = 0;
			for (int i = 0; i < DIM; i++)
				squaredDistance += dir[i] * dir[i];
			
			if (squaredDistance <= radius() * radius())
			{
				_priorityQueue.insert({node->orgIndex, squaredDistance});
			}
			
			// traverse towards root
			if (signedDistanceToPlane < 0)
			{
				searchStack[++searchIndex] = node->left;
				if(absDistance < radius() + EPSILON)
					searchStack[++searchIndex] = node->right;
			}
			else if (signedDistanceToPlane > 0)
			{
				searchStack[++searchIndex] = node->right;
				if (absDistance < radius() + EPSILON)
					searchStack[++searchIndex] = node->left;
			}
			else
			{
				searchStack[++searchIndex] = node->left;
				searchStack[++searchIndex] = node->right;
			}
		}

        int startIndex = indices.size();
        indices.resize(indices.size() + _priorityQueue.size());
		for(int i= 0; i < _priorityQueue.size() ; i++)
			indices[startIndex+i] = _priorityQueue[i]._index;
		
	}
	
	inline void outside(std::vector<int> & searchStack, const PointClass & center, RadiusType radius, std::vector<unsigned int> & indices)
	{
		std::vector<unsigned int> insidePoints;
		insidePoints.reserve(_points.size());
		inside(searchStack, center, radius, insidePoints);
		if(insidePoints.empty())
		{
			const unsigned int startIndex = (unsigned int)indices.size();
			const unsigned int endIndex = startIndex + (unsigned int)_points.size();
			indices.resize(endIndex);
			for(unsigned int i=startIndex ; i < endIndex ; i++)
				indices[i] = i - (unsigned int)_points.size();
			return;
			
		}
		std::sort(insidePoints.begin(), insidePoints.end());
		unsigned int counter = 0;
		while(counter<insidePoints[0])
			indices.push_back(counter++);
		
		for(unsigned int index = 1 ; index < insidePoints.size()-1 ; index++)
		{
			unsigned int startValue = insidePoints[index];
			unsigned int endValue = insidePoints[index+1];
			for(unsigned int j= startValue+1 ; j < endValue ; j++)
				indices.push_back(j);
		}
		
		counter = insidePoints[insidePoints.size()-1]+1;
		while(counter < (unsigned int)_points.size())
			indices.push_back(counter++);
	}

private:
	// for the easy to use methods
	std::vector<BalanceDescriptor> _balancingStack;
protected:
	// for the easy to use methods
	std::vector<int> _searchStack;
};

// 1D specialization of kdtree
template<typename PointClass, typename RadiusType>
class KDTree1D : public KDTree<PointClass, 1, RadiusType>
{
public:
	inline void nearestNeighbours(const PointClass & center, unsigned int count, std::vector<unsigned int> & indices, bool wrapSearch = false, const PointClass & wrapDimensions = PointClass())
	{
		std::vector<int> & searchStack = KDTree<PointClass, 1, RadiusType>::_searchStack;
		if (!wrapSearch)
		{
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::nearestNeighbours(searchStack, center, count, indices);
		}
		else
		{
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::nearestNeighbours(searchStack, center - wrapDimensions[0], count, indices);
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::nearestNeighbours(searchStack, center, count, indices);
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::nearestNeighbours(searchStack, center + wrapDimensions[0], count, indices);

			// remove duplicates
			std::sort(indices.begin(), indices.end());
			indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
		}
	}

	inline void inside(const PointClass & center, RadiusType radius, std::vector<unsigned int> & indices, bool wrapSearch = false, const PointClass & wrapDimensions = PointClass())
	{
		std::vector<int> & searchStack = KDTree<PointClass, 1, RadiusType>::_searchStack;
		if (!wrapSearch)
		{
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::inside(searchStack, center, radius, indices);
		}
		else
		{
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::inside(searchStack, center - wrapDimensions[0], radius, indices);
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::inside(searchStack, center, radius, indices);
			searchStack.clear();
			KDTree<PointClass, 1, RadiusType>::inside(searchStack, center + wrapDimensions[0], radius, indices);

			// remove duplicates
			std::sort(indices.begin(), indices.end());
			indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
		}
	}

};

// 2D specialization of kdtree
template<typename PointClass, typename RadiusType>
class KDTree2D : public KDTree<PointClass, 2, RadiusType>
{
public:
	inline void nearestNeighbours(const PointClass & center, unsigned int count, std::vector<unsigned int> & indices, bool wrapSearch = true, const PointClass & wrapDimensions = PointClass())
	{
		std::vector<int> & searchStack = KDTree<PointClass, 2, RadiusType>::_searchStack;
		if (!wrapSearch)
		{
			searchStack.clear();
			KDTree<PointClass, 2, RadiusType>::nearestNeighbours(searchStack, center, count, indices);
		}
		else
		{
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					searchStack.clear();
					PointClass start = center + PointClass(wrapDimensions[0] * (RadiusType(x - 1)), wrapDimensions[1] * (RadiusType(y - 1)));
					KDTree<PointClass, 2, RadiusType>::nearestNeighbours(searchStack, start, count, indices);
				}
			}

			// remove duplicates
			std::sort(indices.begin(), indices.end());
			indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
		}
	}

	inline void inside(const PointClass & center, RadiusType radius, std::vector<unsigned int> & indices, bool wrapSearch = false, const PointClass & wrapDimensions = PointClass())
	{
		std::vector<int> & searchStack = KDTree<PointClass, 2, RadiusType>::_searchStack;
		if (!wrapSearch)
		{
			searchStack.clear();
			KDTree<PointClass, 2, RadiusType>::inside(searchStack, center, radius, indices);
		}
		else
		{
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					searchStack.clear();
					PointClass start = center + PointClass(wrapDimensions[0] * (RadiusType(x - 1)), wrapDimensions[1] * (RadiusType(y - 1)));
					KDTree<PointClass, 2, RadiusType>::inside(searchStack, start, radius, indices);
				}
			}

			// remove duplicates
			std::sort(indices.begin(), indices.end());
			indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
		}
	}

};

// 2D specialization of kdtree
template<typename PointClass, typename RadiusType>
class KDTree3D : public KDTree<PointClass, 3, RadiusType>
{
public:
	inline void nearestNeighbours(const PointClass & center, unsigned int count, std::vector<unsigned int> & indices, bool wrapSearch = true, const PointClass & wrapDimensions = PointClass())
	{
		std::vector<int> & searchStack = KDTree<PointClass, 3, RadiusType>::_searchStack;
		if (!wrapSearch)
		{
			searchStack.clear();
			KDTree<PointClass, 3, RadiusType>::nearestNeighbours(searchStack, center, count, indices);
		}
		else
		{
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					for (int z = 0; z < 3; z++)
					{
						searchStack.clear();
						PointClass start = center + PointClass(wrapDimensions[0] * (RadiusType(x - 1)), wrapDimensions[1] * (RadiusType(y - 1)), wrapDimensions[2] * (RadiusType(z - 1)));
						KDTree<PointClass, 3, RadiusType>::nearestNeighbours(searchStack, start, count, indices);
					}

				}
			}

			// remove duplicates
			std::sort(indices.begin(), indices.end());
			indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
		}
	}

	inline void inside(const PointClass & center, RadiusType radius, std::vector<unsigned int> & indices, bool wrapSearch = false, const PointClass & wrapDimensions = PointClass())
	{
		std::vector<int> & searchStack = KDTree<PointClass, 3, RadiusType>::_searchStack;
		if (!wrapSearch)
		{
			searchStack.clear();
			KDTree<PointClass,3, RadiusType>::inside(searchStack, center, radius, indices);
		}
		else
		{
			for (int x = 0; x < 3; x++)
			{
				for (int y = 0; y < 3; y++)
				{
					for (int z = 0; z < 3; z++)
					{
						searchStack.clear();
						PointClass start = center + PointClass(wrapDimensions[0] * (RadiusType(x - 1)), wrapDimensions[1] * (RadiusType(y - 1)), wrapDimensions[2] * (RadiusType(z - 1)));
						KDTree<PointClass, 3, RadiusType>::inside(searchStack, start, radius, indices);
					}
				}
			}

			// remove duplicates
			std::sort(indices.begin(), indices.end());
			indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
		}
	}

};

#endif
