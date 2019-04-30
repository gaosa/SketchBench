#ifndef CMMSKETCH_H
#define CMMSKETCH_H
#include "SketchBase.h" //DO NOT change this include
#include "BoBHash.h"//If you want to use DSAB-builtin hashfunction must include this

#include<string>
#include<iostream>
#include<algorithm>
#include<memory.h>

class CMMSketch : public SketchBase {
private:
	BOBHash *hash;//optional DSAB-builtin hashfunction
	int hash_num;
	int counter_per_array;
	int bit_per_counter;
	int **data;
	//int size_of_new_count;
	int *num_element;
	int *t;

	int heap_size;
	int heap_count;
	char **flowid;
	int *heap;
public:
	using SketchBase::sketch_name;//DO NOT change this declaration
	CMMSketch()
	{
		sketch_name = "CMMSketch";//please keep sketch_name the same as class name and .h file name
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
		num_element = new int[hash_num];
		t = new int[hash_num];
		memset(num_element, 0, sizeof(int)*hash_num);
		memset(t, 0, sizeof(int)*hash_num);

		flowid = (char **)malloc(heap_size*(sizeof(char*)));
		for (int i = 0;i < heap_size;++i)
		{
			flowid[i] = new char[32];
			flowid[i][0] = '\0';
		}
		heap = new int[heap_size];
	}

	void Insert(const char *str, const int & len)
	{
		for (int i = 0;i<hash_num;++i) {
			++data[i][hash[i].Run(str, len) % counter_per_array];
			num_element[i]++;
		}
		int estimate = frequencyQuery(str, len);
		int flag = 0;
		for (int i = 0;i < heap_count;i++)
		{
			if (strcmp(flowid[i], str) == 0)
			{
				heap[i] = estimate;
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
				heap[heap_count] = estimate;
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
				if (heap[heap_size - 1] < estimate)
				{
					strcpy(flowid[heap_size - 1], str);
					heap[heap_size - 1] = estimate;
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
		int temp, noise;
		temp = data[0][hash[0].Run(str, len) % counter_per_array];
		noise = (num_element[0] - temp) / (counter_per_array - 1);
		t[0] = temp - noise;
		for (int i = 1; i < hash_num; ++i) {
			temp = data[i][hash[i].Run(str, len) % counter_per_array];
			noise = (num_element[i] - temp) / (counter_per_array - 1);
			t[i] = temp - noise;
		}
		std::sort(t, t + hash_num);
		int res = (t[hash_num >> 1] + t[(hash_num - 1) >> 1]) >> 1;
		res = res<0 ? 0 : res;
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
		memset(flowid, 0, sizeof(flowid));
		memset(heap, 0, sizeof(heap));
		memset(num_element, 0, sizeof(int)*hash_num);
		memset(t, 0, sizeof(int)*hash_num);
	}

	~CMMSketch()
	{
		for (int i = 0; i<hash_num; ++i)
		{
			delete[] data[i];
		}
		delete[] data;
		delete[] hash;
		delete[]t;
		delete[]num_element;
		delete[] heap;
		for (int i = 0;i < heap_size;++i)
			delete[] flowid[i];
		delete[] flowid;
	}
};
#endif//DO NOT change this file


