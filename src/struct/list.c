#include <stdio.h>
#include "list.h"

/**************************************************
��  ��:list_is_empty@4
��  ��:�ж�˫�������Ƿ�Ϊ��
��  ��:phead - ˫������ͷָ��
����ֵ:�����ʾΪ��,���ʾ��Ϊ��
˵  ��:
**************************************************/
int list_is_empty(list_s* phead)
{
	return phead->next == phead;
}

/**************************************************
��  ��:list_init@4
��  ��:��ʼ��˫������Ϊ��
��  ��:phead - ����ͷָ��
����ֵ:(��)
˵  ��:
**************************************************/
void list_init(list_s* phead)
{
	//ʹǰ��ָ�붼ָ���Լ�(ͷ���)
	//��Ϊ������
	phead->prior = phead;
	phead->next = phead;
}

/**************************************************
��  ��:list_insert_head@8
��  ��:��˫������ͷ��������
��  ��:	phead - ����ͷָ��
		plist - ��������������ָ��
����ֵ:(��)
˵  ��:
**************************************************/
void list_insert_head(list_s* phead, list_s* plist)
{
	//�õ���1������ָ��
	//Ϊ������ʱҲ����
	list_s* first = phead->next;
	//��ͷ���͵�1�����֮�����ʱ��Ҫ:
	//	ͷ����prior����;ͷ����nextָ���½��
	//	�½���priorָ��ͷ���;�½���nextָ���1�����
	//	��1������priorָ���½��;��1������next����
	phead->next = plist;
	plist->prior = phead;
	plist->next = first;
	first->prior = plist;
}

/**************************************************
��  ��:list_insert_tail@8
��  ��:��˫������β��������
��  ��:	phead - ����ͷָ��
		plist - ��������������ָ��
����ֵ:(��)
˵  ��:
**************************************************/
void list_insert_tail(list_s* phead, list_s* plist)
{
	//�õ����һ������ָ��
	//Ϊ������ʱҲ����
	list_s* last = phead->prior;
	//�����һ������ͷ���֮�����ʱ��Ҫ:
	//	���һ������nextָ���½��;���һ������prior����
	//	�½���priorָ�����һ�����;�½���nextָ��ͷ���
	//	ͷ����next����;ͷ����priorָ���½��
	last->next = plist;
	plist->prior = last;
	plist->next = phead;
	phead->prior = plist;
}

/**************************************************
��  ��:list_remove_head@4
��  ��:��˫������ͷ���Ƴ����
��  ��:phead - ����ͷָ��
����ֵ:���Ƴ��Ľ��ָ��
	������Ϊ��,����NULL
˵  ��:
**************************************************/
list_s* list_remove_head(list_s* phead)
{
	list_s* second = NULL;
	list_s* removed = NULL;
	if(list_is_empty(phead))
		return NULL;
	//��Ҫ�Ƴ���1�����,��Ӧ���ȱ����2�����
	//�������ڵ�2�����Ҳ��������(��ʱsecond��Ϊͷ���)
	second = phead->next->next;
	removed = phead->next;
	//�Ƴ���1�������Ҫ:
	//	ͷ����nextָ���2�����(��������,��ָ���Լ�);ͷ����prior����
	//	��2������priorָ��ͷ���;��2������next����
	phead->next = second;
	second->prior = phead;
	return removed;
}

/**************************************************
��  ��:list_remove_tail@4
��  ��:��˫������β���Ƴ����
��  ��:phead - ����ͷָ��
����ֵ:���Ƴ��Ľ��ָ��
	������Ϊ��,����NULL
˵  ��:
**************************************************/
list_s* list_remove_tail(list_s* phead)
{
	list_s* second_last = NULL;
	list_s* removed = NULL;
	if(list_is_empty(phead))
		return NULL;
	//��Ҫ�Ƴ����һ�����,��Ҫ���浹����2�����ָ��
	//�������ڵ�����2��(��һ�����ʱ),������2������ͷ���
	second_last = phead->prior->prior;
	removed = phead->prior;
	//�Ƴ�һ�������Ҫ
	//	������2������nextָ��ͷ���,prior����
	//	ͷ����priorָ������2�����,next����
	second_last->next = phead;
	phead->prior = second_last;
	return removed;
}

/**************************************************
��  ��:list_remove
��  ��:�Ƴ����ָ��Ϊp�Ľ��,�����ͷŽڵ��ڴ�,Ҳû��ȥ�ͷ�
��  ��:	p - ���Ƴ��Ľ��
		phead - ����ͷ���
��  ��:	0 - �޴˽��
		1 - �ɹ�
		2 - ����Ϊ��
˵  ��:
**************************************************/
int list_remove(list_s* phead,list_s* p)
{
	if(!list_is_empty(phead)){
		list_s* node = NULL;
		for(node=phead->next; node!=phead; node=node->next){
			if(node == p){
				//�Ƴ��ý����Ҫ:
				// ����ǰ������һ�����:
				//		nextָ��ǰ����next
				// ����ǰ������һ�����:
				//		priorָ��ǰ������һ�����
				node->prior->next = node->next;
				node->next->prior = node->prior;
				return 1;
			}
		}
		return 0;
	}else{
		return 2;
	}
}
