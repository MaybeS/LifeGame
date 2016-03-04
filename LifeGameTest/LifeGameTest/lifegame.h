#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#define IDT_TIMER_GEN 1
#define TIMER_GEN_INTERVAL 333

using namespace std;
class lifegame {
public:
	size_t size, count;
	int gen = 0;
	vector<vector<bool>> ele;
	lifegame(size_t _size) : size(_size)
	{
		ele.assign(size,vector<bool>(size));
	}
	~lifegame()
	{

	}
	int next()
	{
		count = 0;
		vector<vector<bool>> nxt(size, vector<bool>(size, false));
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
			{
				if (!ele[i][j] && nearby(i, j) == 3)
				{
					nxt[i][j] = true;
					count++;
				}
				if (ele[i][j] && (nearby(i, j) == 2 || nearby(i, j) == 3))
				{
					nxt[i][j] = true;
					count++;
				}
			}
		if (!count)
			gen = 0;
		ele.assign(nxt.begin(), nxt.end());
		gen++;
		return count;
	}
	void clear()
	{
		ele.assign(size, vector<bool>(size, false));
	}
private:
	int nearby(size_t i, size_t j)
	{
		int di[] = {-1,0,1,1,1,0,-1,-1 }, dj[] = {-1,-1,-1,0,1,1,1,0};
		int result = 0;
		for (int k = 0; k < 8; k++)
			try {
				if (i + di[k] < 0 || i + di[k] >= size || j + dj[k] < 0 || j + dj[k] >= size)
					throw k;
				if (this->ele[i + di[k]][j + dj[k]])
					result++;
			} catch (...) {
				continue;
			}
		return result;
	}
};