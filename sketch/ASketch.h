#ifndef A_SKETCH_H
#define A_SKETCH_H
#include "SketchBase.h" //DO NOT change this include
#include "BoBHash.h"//If you want to use DSAB-builtin hashfunction must include this

/*----optional according to your need----*/
#include<string>
#include<iostream>
#include<memory.h>
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


class ASketch : public SketchBase {
private:
	BOBHash *hash;//optional DSAB-builtin hashfunction
				  /*----optional according to your need----*/
	int hash_num;
	int counter_per_array;
	int bit_per_counter;
	//int size_of_new_count;
	int **data;
	int *new_count;
	int *old_count;
	char *item[64];

	int heap_size;
	int heap_count;
	char **flowid;
	int *heap;
	/*----optional according to your need----*/
public:
	using SketchBase::sketch_name;//DO NOT change this declaration
	ASketch()
	{
		/*constructed function MUST BT non-parameter!!!*/
		sketch_name = "ASketch";//please keep sketch_name the same as class name and .h file name
	}
	void parameterSet(const std::string& parameterName, double parameterValue)
	{
		/*MUST have this function even empty function body*/

		/*----optional according to your need----*/
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
		/*
		if (parameterName=="size_of_new_count")
		{
		size_of_new_count = parameterValue;
		return;
		}
		*/
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
			hash[i].SetSeed((1001 * i)%1231);
		}
		new_count = new int[64];
		old_count = new int[64];
		memset(new_count, 0, sizeof(int) * 64);
		memset(old_count, 0, sizeof(int) * 64);
		for (int i = 0;i<64;i++)
		{
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
	int find_element_in_filter(const char *str)
	{
		for (int i = 0;i<64;i++)
		{
			if (strcmp(str, item[i]) == 0)
				return  i;
		}
		return -1;
	}
	int find_empty_in_filter()
	{
		for (int i = 0;i<64;i++)
		{
			if (strlen(item[i]) == 0)
				return i;
		}
		return -1;
	}
	void Insert(const char *str, const int & len)
	{
		/*MUST have this function DO NOT change parameter type*/

		/*----optional according to your need----*/
		int index = find_element_in_filter(str);
		//元素在filter中
		if (index != -1)
		{
			new_count[index]++;
			int flag = 0;
			for (int i = 0;i < heap_count;i++)
			{
				if (strcmp(flowid[i], str) == 0)
				{
					heap[i] = new_count[index];
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
					heap[heap_count] = new_count[index];
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
					if (heap[heap_size - 1] < new_count[index])
					{
						strcpy(flowid[heap_size - 1], str);
						heap[heap_size - 1] = new_count[index];
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
			return;
		}
		//元素不在filter中，同时filter中还有空位
		int index_empty = find_empty_in_filter();
		if (index_empty != -1)
		{
			strcpy(item[index_empty], str);
			new_count[index_empty] = 1;
			old_count[index_empty] = 0;
			if (heap_count < heap_size)
			{
				strcpy(flowid[heap_count], str);
				heap[heap_count] = 1;
				heap_count++;
			}
			return;
		}
		++data[0][hash[0].Run(str, len) % counter_per_array];
		int estimate_freq = data[0][hash[0].Run(str, len) % counter_per_array];
		//元素不在filter中，同时filter中没有空位，在sketch中操作
		for (int i = 1; i < hash_num; ++i) {
			++data[i][hash[i].Run(str, len) % counter_per_array];
			if (estimate_freq>data[i][hash[i].Run(str, len) % counter_per_array])
				estimate_freq = data[i][hash[i].Run(str, len) % counter_per_array];
		}
		int filter_min_index = 0;//找到filter中最小的元组
		int filter_min_freq = new_count[0];
		for (int i = 1;i<64;i++)
		{
			if (filter_min_freq>new_count[i])
			{
				filter_min_freq = new_count[i];
				filter_min_index = i;
			}
		}
		if (estimate_freq>filter_min_freq)
		{
			int diff = new_count[filter_min_index] - old_count[filter_min_index];
			if (diff>0)
			{
				for (int i = 0;i<hash_num;++i)
					data[i][hash[i].Run(str, len) % counter_per_array] += diff;

			}
			strcpy(item[filter_min_index], str);
			new_count[filter_min_index] = estimate_freq;
			old_count[filter_min_index] = estimate_freq;
		}

		int flag = 0;
		for (int i = 0;i < heap_count;i++)
		{
			if (strcmp(flowid[i], str) == 0)
			{
				heap[i] = estimate_freq;
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
				heap[heap_count] = estimate_freq;
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
				if (heap[heap_size - 1] < estimate_freq)
				{
					strcpy(flowid[heap_size - 1], str);
					heap[heap_size - 1] = estimate_freq;
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
		/*----optional according to your need----*/
	}
	int frequencyQuery(const char *str, const int & len)
	{
		/*MUST have this function DO NOT change function head and parameter type */

		/*----optional according to your need----*/
		int index = find_element_in_filter(str);
		if (index != -1)
			return new_count[index];
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
			printf("Your k %d is too large,try a small one!\n",k);
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
		for (int i = 0; i<hash_num; ++i)
		{
			memset(data[i], 0, sizeof(int)*counter_per_array);
		}
		memset(new_count, 0, sizeof(int) * 64);
		memset(old_count, 0, sizeof(int) * 64);
		memset(flowid, 0, sizeof(flowid));
		memset(heap, 0, sizeof(heap));
		for (int i = 0;i<64;i++)
		{
			item[i][0] = '\0';
		}
		/*----optional according to your need----*/
	}
	~ASketch()
	{
		/*MUST have this function */

		/*----optional according to your need----*/
		for (int i=0; i<hash_num; ++i)
		{
			delete[] data[i];
		}
		delete[] data;
		delete[] hash;
		delete new_count;
		delete old_count;
		for (int i = 0;i<64;i++)
		{
			delete []item[i];
		}
		delete[] heap;
		for (int i = 0;i < heap_size;++i)
			delete[] flowid[i];
		delete[] flowid;
		/*----optional according to your need----*/
	}

	/*----optional You can add your function----*/
};
#endif//DO NOT change this file
