#include <vector>
#include <cstdint>
#include <cmath>

struct uchar4
{
	unsigned char arr[4];
	uchar4(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
	{
		arr[0] = x; arr[1] = y; arr[2] = z; arr[3] = w;
	}
    uchar4()
    {
    }
};
struct uint4
{
	unsigned int arr[4];
	uint4(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
	{
		arr[0] = x; arr[1] = y; arr[2] = z; arr[3] = w;
	}
    uint4()
    {
    }
};

template<class T>
struct BoundaryCondition
{
    enum class Type
    {
        Undefined,
        Constant
    };
    T c;
    Type type;
    BoundaryCondition()
    {
        type = Type::Undefined;
    }
    BoundaryCondition(Type t)
    {
        type = t;
    }
    BoundaryCondition(T constant)
    {
        type = Type::Constant;
        c = constant;
    }
};

template<class T>
struct matrix
{
    std::vector<T> data;
    int w, h;
    matrix()
    {
        w = h = 0;
    }
    matrix(int width, int height)
    {
        w = width; h = height;
        data.resize(w * h);
    }
    matrix(int width, int height, T* data)
    {
        w = width; h = height;
        this->data.resize(w * h);
        for (int i = 0; i < w*h; i++)
        {
            this->data[i] = data[i];
        }
    }
    matrix(int width, int height, std::vector<T> data)
    {
        w = width; h = height;
        this->data = data;
    }
    matrix(std::vector<std::vector<T>> data)
    {
        h = data.size();
        w = data[0].size();
        for(auto &column: data)
        	for(auto cell: column)
        		this->data.emplace_back(cell);
    }

    T get(int x, int y, BoundaryCondition<T> b = BoundaryCondition<T>())
    {
        if (x >= 0 && x < w && y >= 0 && y < h)
            return data[y * w + h];
        switch(b.type)
        {
        case BoundaryCondition<T>::Type::Constant:
            return b.c;
        case BoundaryCondition<T>::Type::Undefined:
        default:
            return T();
        }
    }

    template <class Other>
    matrix<Other> copy_roi(int x, int y, int w, int h, BoundaryCondition<T> b = BoundaryCondition<T>())
    {
        matrix<Other> to_return(w, h);
        for(int i = y; i < y+h; i++)
        {
            for(int j = x; j < x+w; j++)
            {
                to_return.write(j-x, i-y, get(j, i, b));
            }
        }
        return to_return;
    }

    void write(int x, int y, T d)
    {
        data[y * w + x] = d;
    }
};

template<class T>
struct Accessor
{
public:
	matrix<T>& m;
	Accessor(matrix<T>& m)
		:m(m)
	{
	}

    T get(int x, int y, BoundaryCondition<T> b = BoundaryCondition<T>())
    {
        return m.get(x, y, b);
    }

    template <class Other>
    matrix<Other> copy_roi(int x, int y, int w, int h, BoundaryCondition<T> b = BoundaryCondition<T>())
    {
        return m.template copy_roi<Other>(x, y, w, h, b);
    }
};


template<class T>
struct IterationSpace
{
public:
	matrix<T>& m;
	IterationSpace(matrix<T>& m)
		:m(m)
	{
	}

    void write(int x, int y, T d)
    {
        m.write(x, y, d);
    }
};



