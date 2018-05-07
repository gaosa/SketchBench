#ifndef LOSSYCOUNTING_H_INCLUDED
#define LOSSYCOUNTING_H_INCLUDED

#include<map>
#include<utility>


template<class Unit>
class LossyCounting {
private:
    int bucket_size;//ÿһ��bucket�Ĵ�С
    int bucket_id;//bucket�ı��
    int total;//�ܹ�����Ԫ�صĸ���
    double threshold;//�����½�
    double error;//�������Χ
    int bucket_num_cnt;//���ڼ���bucket���
	std::map <const char*, std::pair<Unit,Unit> > data;
public:
    LossyCounting(int size,double thre,double err){
        bucket_size=size;
        threshold=thre;
        error=err;
        bucket_id=1;
        total=0;
        bucket_num_cnt=0;
    }
    void Insert(const char *str, const int len) {
        if(data.find(str)!=data.end()){
            data[str].first++;
        }
        else{
            std::pair<Unit,Unit> temp(1,bucket_id-1);
            data[str]=temp;
        }
        bucket_num_cnt++;
        total++;
        //��ǰbucket���ˣ�������һ��bucket
        if(bucket_num_cnt>=bucket_size){
            auto ii=data.begin();
            while(ii!=data.end())
            {
                if(ii->second.first+ii->second.second< bucket_id)
                    data.erase(ii++);
                else
                    ++ii;
            }
			bucket_id++;
			bucket_num_cnt = 0;
        }
    }
    Unit Query(const char *str, const int len) {
        return data[str].first;
    }
};

#endif // LOSSYCOUNTING_H_INCLUDED
