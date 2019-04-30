#ifndef CM_SKETCH_H
#define CM_SKETCH_H
#include "SketchBase.h"
#include "BoBHash.h"
#include<string>
#include<iostream>
#include<memory.h>
/*
int is type of frequencyQuery Return
hashfunction type:BOBHash\
virtual ~SketchBase() = 0;
virtual void parameterSet(const string& parameterName, double & parameterValue)=0;
virtual init() = 0;
virtual void Insert(const char *str, const int & len) = 0;
virtual int frequencyQuery(const char *str, const int & len) = 0;
virtual int topkQuery(const int & k) = 0;
virtual void reset() = 0;//reset sketch to the initial state
*/
class CMSketch : public SketchBase {
private:
	int hash_num;
	int counter_per_array;
	BOBHash *hash;
	int bit_per_counter;
	int **data;

	int heap_size;
	int heap_count;
	char **flowid;
	int *heap;
public:
	using SketchBase::sketch_name;
	CMSketch()
	{
		sketch_name = "CMSketch";
	}
	void parameterSet(const std::string& parameterName, double parameterValue)
	{
		if (parameterName == "heap_size")
		{
			heap_size = parameterValue;
			return;
		}
		if (parameterName == "hash_num")
		{
			hash_num = parameterValue;
			return;
		}
		if (parameterName == "bit_per_counter")
		{
			bit_per_counter = parameterValue;
			return;
		}
		if (parameterName == "counter_per_array")
		{
			counter_per_array = parameterValue;
			return;
		}

	}
	void init()
	{
		data = new int*[hash_num];
		hash = new BOBHash[hash_num];
		for (int i = 0; i<hash_num; ++i)
		{
			data[i] = new int[counter_per_array];
			memset(data[i], 0, sizeof(int)*counter_per_array);
			hash[i].SetSeed((1001 * i) % 1231);
		}
		flowid = (char **)malloc(heap_size*(sizeof(char*)));
		for (int i = 0;i < heap_size;++i)
		{
			flowid[i] = new char[32];
			flowid[i][0] = '\0';
		}
		heap = new int[heap_size];
		memset(heap, 0, sizeof(int)*heap_size);
	}
	void Insert(const char *str, const int & len)
	{
		int min_value = 1000000000;
		for (int i = 0; i < hash_num; ++i)
		{
			if (min_value > data[i][hash[i].Run(str, len) % counter_per_array])
				min_value = data[i][hash[i].Run(str, len) % counter_per_array];
			++data[i][hash[i].Run(str, len) % counter_per_array];
		}

		int flag = 0;
		for (int i = 0;i < heap_count;i++)
		{
			if (strcmp(flowid[i], str) == 0)
			{
				heap[i] = min_value + 1;
				for (int j = i - 1;j >= 0;j--)
				{
					if (j == 0 && heap[j] < heap[i])
					{
						char temp[32] = { 0 };
						strcpy(temp, flowid[i]);
						strcpy(flowid[i], flowid[j]);
						strcpy(flowid[j], temp);
						int tempnum = heap[i];
						heap[i] = heap[j];
						heap[j] = tempnum;
						break;
					}
					else if (heap[j] >= heap[i] && j < i - 1)
					{
						char temp[32] = { 0 };
						strcpy(temp, flowid[i]);
						strcpy(flowid[i], flowid[j + 1]);
						strcpy(flowid[j + 1], temp);
						int tempnum = heap[i];
						heap[i] = heap[j + 1];
						heap[j + 1] = tempnum;
						break;
					}
				}
				flag = 1;
				break;
			}
		}
		if (flag == 0)
		{
			if (heap_count < heap_size)
			{
				strcpy(flowid[heap_count], str);
				heap[heap_count] = min_value + 1;
				heap_count++;
				for (int i = heap_count - 1, j = i - 1;j >= 0;j--)
				{
					if (j == 0 && heap[j] < heap[i])
					{
						char temp[32] = { 0 };
						strcpy(temp, flowid[i]);
						strcpy(flowid[i], flowid[j]);
						strcpy(flowid[j], temp);
						int tempnum = heap[i];
						heap[i] = heap[j];
						heap[j] = tempnum;
						break;
					}
					else if (heap[j] >= heap[i] && j < i - 1)
					{
						char temp[32] = { 0 };
						strcpy(temp, flowid[i]);
						strcpy(flowid[i], flowid[j + 1]);
						strcpy(flowid[j + 1], temp);
						int tempnum = heap[i];
						heap[i] = heap[j + 1];
						heap[j + 1] = tempnum;
						break;
					}
				}
			}
			else
			{
				if (heap[heap_size - 1] < min_value + 1)
				{
					strcpy(flowid[heap_size - 1], str);
					heap[heap_size - 1] = min_value + 1;
					for (int i = heap_size - 1, j = i - 1;j >= 0;j--)
					{
						if (j == 0 && heap[j] < heap[i])
						{
							char temp[32] = { 0 };
							strcpy(temp, flowid[i]);
							strcpy(flowid[i], flowid[j]);
							strcpy(flowid[j], temp);
							int tempnum = heap[i];
							heap[i] = heap[j];
							heap[j] = tempnum;
							break;
						}
						else if (heap[j] >= heap[i] && j < i - 1)
						{
							char temp[32] = { 0 };
							strcpy(temp, flowid[i]);
							strcpy(flowid[i], flowid[j + 1]);
							strcpy(flowid[j + 1], temp);
							int tempnum = heap[i];
							heap[i] = heap[j + 1];
							heap[j + 1] = tempnum;
							break;
						}
					}
				}
			}
		}
	}
	int frequencyQuery(const char *str, const int & len)
	{
		int res = data[0][hash[0].Run(str, len) % counter_per_array];
		for (int i = 1; i < hash_num; ++i) {
			int t = data[i][hash[i].Run(str, len) % counter_per_array];
			res = res < t ? res : t;
		}
		return res;
	}
	int topkQuery(const int & k)
	{
		if (k>heap_size)
		{
			printf("Your k %d is too large,try a small one!\n", k);
			return 0;
		}
		for (int i = 0;i < k;i++)
		{
			std::cout << flowid[i] << ' ' << heap[i] << std::endl;
		}
		return 0;
	}
	void reset()
	{
		for (int i = 0; i<hash_num; ++i)
		{
			memset(data[i], 0, sizeof(int)*counter_per_array);
		}
		for (int i = 0;i<heap_size;++i)
		{
			memset(flowid[i], 0, sizeof(char) * 32);
		}
		memset(heap, 0, sizeof(int)*heap_size);
	}
	~CMSketch()
	{
		for (int i=0; i<hash_num; ++i)
		{
			delete[] data[i];
		}
		delete[] data;
		delete[] heap;
		for (int i = 0;i < heap_size;++i)
			delete[] flowid[i];
		delete[] flowid;
	}
};
#endif
