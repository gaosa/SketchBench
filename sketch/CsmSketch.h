#ifndef CSMSketch_H //must change this MACRO
#define CSMSketch_H //must change this MACRO
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


class CSMSketch : public SketchBase {
private:
	BOBHash *hash;//optional DSAB-builtin hashfunction
				  /*----optional according to your need----*/
	int hash_num;
	int counter_per_array;
	int bit_per_counter;
	int **data;
	int acc = 0;
	int ave = 0;

	int heap_size;
	int heap_count;
	char **flowid;
	int *heap;
	/*----optional according to your need----*/
public:
	using SketchBase::sketch_name;//DO NOT change this declaration
	CSMSketch()
	{
		/*constructed function MUST BT non-parameter!!!*/
		sketch_name = "CSMSketch";//please keep sketch_name the same as class name and .h file name
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
			hash[i].SetSeed((1001 * i)%1231);
		}
		acc = 0;
		ave = 0;
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
		unsigned int idx = rand() % hash_num;
		int i=hash[idx].Run(str, len);
		++data[idx][hash[idx].Run(str, len) % counter_per_array];
		acc = (acc == counter_per_array - 1) ? 0 : acc + 1;
		ave += (acc == 0);

		int res = -ave;
		for (int i = 0; i < hash_num; ++i) {
			res += data[i][hash[i].Run(str, len) % counter_per_array];
		}
		res = res>0 ? res : 0;
		int flag = 0;
		for (int i = 0;i < heap_count;i++)
		{
			if (strcmp(flowid[i], str) == 0)
			{
				heap[i] = res;
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
				heap[heap_count] = res;
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
				if (heap[heap_size - 1] < res)
				{
					strcpy(flowid[heap_size - 1], str);
					heap[heap_size - 1] = res;
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
		int res = -ave;
		for (int i = 0; i < hash_num; ++i) {
			res += data[i][hash[i].Run(str, len) % counter_per_array];
		}
		res = res>0 ? res : 0;
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
		acc = 0;
		ave = 0;
		memset(flowid, 0, sizeof(flowid));
		memset(heap, 0, sizeof(heap));
		/*----optional according to your need----*/
	}
	~CSMSketch()
	{
		/*MUST have this function */

		/*----optional according to your need----*/
		for (int i=0; i<hash_num; ++i)
		{
			delete[] data[i];
		}
		delete[] data;
		delete[] hash;
		delete[] heap;
		for (int i = 0;i < heap_size;++i)
			delete[] flowid[i];
		delete[] flowid;
		/*----optional according to your need----*/
	}

	/*----optional You can add your function----*/
};
#endif//DO NOT change this file
