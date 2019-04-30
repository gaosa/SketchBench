#ifndef LossyCUSketch_H //must change this MACRO
#define LossyCUSketch_H //must change this MACRO
#include "SketchBase.h" //DO NOT change this include
#include "BoBHash.h"//If you want to use DSAB-builtin hashfunction must include this

/*----optional according to your need----*/
#include<string>
#include<iostream>
#include<memory.h>
#include <cstdlib>
/*----optional according to your need----*/

/*----builtin hashfunction----*/
/*
DSAB-builtin hashfunction type:BOBHash\
HOW TO USE:
define: e.g. BOBHash myhash
setseed: e.g. myhash.SetSeed(1001)
calculate hash: e.g. myhash.Run(const char *str, const int & len)
*/
/*----builtin hashfunction----*/


/*----SketchBase virtual function must be finished----*/
/*
virtual ~SketchBase();
virtual void parameterSet(const string& parameterName, double & parameterValue)=0;
virtual init() = 0;
virtual void Insert(const char *str, const int & len) = 0;
virtual int frequencyQuery(const char *str, const int & len) = 0;
virtual int topkQuery(const int & k) = 0;
virtual void reset() = 0;//reset sketch to the initial state
*/
/*----SketchBase virtual function must be finished----*/


class LossyCUSketch : public SketchBase {
private:
	BOBHash *hash;//optional DSAB-builtin hashfunction
				  /*----optional according to your need----*/
	int hash_num;
	int counter_per_array;
	int bit_per_counter;
	int **data;
	int bucket_size;//每一个bucket的大小
	int bucket_id;//bucket的编号
	int total;//总共加入元素的个数
	double threshold;//查找下界
	double error;//允许的误差范围
	int bucket_num_cnt;//用于计算bucket编号
	char **item;

	int heap_size;
	int heap_count;
	char **flowid;
	int *heap;
	/*----optional according to your need----*/
public:
	using SketchBase::sketch_name;//DO NOT change this declaration
	LossyCUSketch()
	{
		/*constructed function MUST BT non-parameter!!!*/
		sketch_name = "LossyCUSketch";//please keep sketch_name the same as class name and .h file name
	}
	void parameterSet(const std::string& parameterName, double  parameterValue)
	{
		/*MUST have this function even empty function body*/

		/*----optional according to your need----*/
		if (parameterName == "heap_size")
		{
			heap_size = parameterValue;
			return;
		}
		if (parameterName == "bucket_size")
		{
			bucket_size = parameterValue;
			return;
		}
		if (parameterName == "threshold")
		{
			threshold = parameterValue;
			return;
		}
		if (parameterName == "error")
		{
			error = parameterValue;
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
		/*----optional according to your need----*/
	}
	void init()
	{
		/*MUST have this function even empty function body*/

		/*----optional according to your need----*/
		data = new int*[hash_num];
		hash = new BOBHash[hash_num];
		for (int i = 0; i<hash_num; ++i)
		{
			data[i] = new int[counter_per_array];
			memset(data[i], 0, sizeof(int)*counter_per_array);
			hash[i].SetSeed((1001 * i) % 1231);
		}
		bucket_id = 1;
		total = 0;
		bucket_num_cnt = 0;
		heap_count = 0;
		item = (char **)malloc(bucket_size * sizeof(char *));
		for (int i = 0;i < bucket_size; ++i) {
			item[i] = new char[32];
			item[i][0] = '\0';
		}

		flowid = (char **)malloc(heap_size*(sizeof(char*)));
		for (int i = 0;i < heap_size;++i)
		{
			flowid[i] = new char[32];
			flowid[i][0] = '\0';
		}
		heap = new int[heap_size];

		/*----optional according to your need----*/
	}

	void Insert(const char *str, const int & len)
	{
		/*MUST have this function DO NOT change parameter type*/

		/*----optional according to your need----*/
		int min_value = data[0][hash[0].Run(str, len) % counter_per_array];
		for (int i = 1; i < hash_num; ++i) {
			if (data[i][hash[i].Run(str, len) % counter_per_array]<min_value)
				min_value = data[i][hash[i].Run(str, len) % counter_per_array];
		}
		for (int i = 0;i<hash_num;++i) {
			if (data[i][hash[i].Run(str, len) % counter_per_array] == min_value)
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
					for (int i = heap_size - 1,j = i - 1;j >= 0;j--)
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

		memcpy(item[bucket_num_cnt], str, len);
		item[bucket_num_cnt][len] = '\0';
		bucket_num_cnt++;
		total++;
		if (bucket_num_cnt == bucket_size) {
			for (int j = 0;j < bucket_size; ++j) {
				for (int i = 0;i < hash_num; ++i)
					if (data[i][hash[i].Run(item[j], strlen(item[j])) % counter_per_array] <= bucket_id)
						--data[i][hash[i].Run(item[j], strlen(item[j])) % counter_per_array];
				item[j][0] = '\0';
			}
			bucket_id++;
			bucket_num_cnt = 0;
		}
		/*----optional according to your need----*/
	}
	int frequencyQuery(const char *str, const int & len)
	{
		/*MUST have this function DO NOT change function head and parameter type */

		/*----optional according to your need----*/
		int res = data[0][hash[0].Run(str, len) % counter_per_array];
		for (int i = 1; i < hash_num; ++i) {
			int t = data[i][hash[i].Run(str, len) % counter_per_array];
			res = res < t ? res : t;
		}
		return res;
		/*----optional according to your need----*/
	}
	int topkQuery(const int & k)
	{
		/*MUST have this function DO NOT change function head and parameter type */

		/*----optional according to your need----*/
		if (k>heap_size)
		{
			printf("Your k is too large,try a small one!\n");
			return 0;
		}
		for (int i = 0;i < k;i++)
		{
			std::cout << flowid[i] << ' ' << heap[i] << std::endl;
		}
		return 0;
		/*----optional according to your need----*/
	}
	void reset()
	{
		/*MUST have this function,reset sketch to the initial state */

		/*----optional according to your need----*/
		for (int i = 0; i < hash_num; ++i) {
			for (int j = 0; j < counter_per_array; ++j) {
				data[i][j] = 0;
			}
		}
		bucket_id = 1;
		total = 0;
		bucket_num_cnt = 0;
		memset(item, 0, sizeof(item));
		memset(flowid, 0, sizeof(flowid));
		memset(heap, 0, sizeof(heap));
		/*----optional according to your need----*/
	}
	~LossyCUSketch()
	{
		/*MUST have this function */

		/*----optional according to your need----*/
		for (int i = 0; i<hash_num; ++i)
		{
			delete[] data[i];
		}
		delete[] data;
		delete[] hash;
		for (int i = 0;i < bucket_size; ++i)
			delete[] item[i];
		delete[] item;
		delete[] heap;
		for (int i = 0;i < heap_size;++i)
			delete[] flowid[i];
		delete[] flowid;

		/*----optional according to your need----*/
	}

	/*----optional You can add your function----*/
};
#endif//DO NOT change this file
