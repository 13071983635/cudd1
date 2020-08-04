#include<stdio.h>
#include<iostream>
#include"cudd.h"
#include"util.h"
using namespace std;
DdNode **createHalfAdderBDD(DdManager *manager)
{
  DdNode *x0 = Cudd_bddIthVar(manager, 0);
  DdNode *x1 = Cudd_bddIthVar(manager, 1);

  DdNode *and1 = Cudd_bddAnd(manager, x0, Cudd_Not(x1));
  Cudd_Ref(and1);

  DdNode *and2 = Cudd_bddAnd(manager, x1, Cudd_Not(x0));
  Cudd_Ref(and2);

  DdNode *sum = Cudd_bddOr(manager, and1, and2);
  Cudd_Ref(sum);
  Cudd_RecursiveDeref(manager, and1);
  Cudd_RecursiveDeref(manager, and2);

  DdNode *carry = Cudd_bddAnd(manager, x0, x1);
  Cudd_Ref(carry);
  
  //返回两个BDD根
  DdNode **outputs = new DdNode*[2];
  outputs[0] = sum;
  outputs[1] = carry;

  return outputs;
}
/*void toDot(DdManager *manager, DdNode **outputs)
{
  char **inputNames = new char*[2];//标记两个输入节点
  inputNames[0] = new char[3];
  inputNames[1] = new char[3];
  inputNames[0] = "x1";
  inputNames[1] = "x2";
  char **outputNames = new char*[2];//标记两个输出节点
  outputNames[0] = new char[4];
  outputNames[1] = new char[6];
  strcpy(outputNames[0], "sum");
  strcpy(outputNames[1], "carry");
  FILE *f = fopen("./HalfAdder.dot", "w");
  //manager：CUDD管理器.
  //2：输出的数量
  //outputs：输出节点的数组
  Cudd_DumpDot(manager, 2, outputs, inputNames, outputNames, f); 
}*/
DdNode *Cudd_bddRestrict(
  DdManager *manager, //DD管理器
  DdNode *BDD,        //用来限制的BDD 
  DdNode *restrictBy  //被限制的BDD
 );
void test(DdManager *manager, DdNode **node)
{
  DdNode *x0 = Cudd_bddIthVar(manager, 0);
  DdNode *x1 = Cudd_bddIthVar(manager, 1);
  const int SIZE = 4;
  DdNode *restrictBy[SIZE];
  DdNode *testSum[SIZE];
  DdNode *testCarry[SIZE];
  //通过如下方式来限制
  restrictBy[0] = Cudd_bddAnd(manager, Cudd_Not(x0), Cudd_Not(x1));
  //x1=0 and x2=0
  restrictBy[1] = Cudd_bddAnd(manager, Cudd_Not(x0), x1);//x1=0/and//x2=1
  restrictBy[2]= Cudd_bddAnd(manager,x0,Cudd_Not(x1));//x1=1//and//x2=0
  restrictBy[3] = Cudd_bddAnd(manager, x0, x1);
  //x1=1 and x2=1
  for(int i=0; i<SIZE; i++)
  {
    Cudd_Ref(restrictBy[i]);//限制引用
    //通过一个新函数限制
    testSum[i] = Cudd_bddRestrict(manager, node[0], restrictBy[i]);
    testCarry[i] = Cudd_bddRestrict(manager, node[1], restrictBy[i]);
    Cudd_RecursiveDeref(manager, restrictBy[i]);//清理限制
  }
  cout << "(x1 = 0, x2 = 0): sum = " << 1 - Cudd_IsComplement(testSum[0])<<' '
  << "Cary =" << 1 - Cudd_IsComplement(testCarry[0]) << endl
  << "(x1 = 0,x2 = 1)：sum = "<< 1-Cudd_IsComplement(testSum[1])<<' '
  << "Carry = "<< 1 - Cudd_IsComplement(testCarry[1]) << endl
  << "(x1 = 1,x2 = 0)：sum = "<< 1-Cudd_IsComplement(testSum[2])<<' '
  << "Carry = "<< 1 - Cudd_IsComplement(testCarry[2]) << endl
  << "(x1 = 1,x2 = 1)：sum = "<< 1-Cudd_IsComplement(testSum[3])<<' '
  << "Carry = " << 1 - Cudd_IsComplement(testCarry[3]) << endl;
  for(int i=0; i<SIZE; i++)
  {
    Cudd_RecursiveDeref(manager,testSum[i]);
    Cudd_RecursiveDeref(manager,testCarry[i]);
  }
}
void toDot(DdManager *manager, DdNode **outputs)
 {
   char **inputNames = new char*[2];//标记两个输入节点
   inputNames[0] = new char[3];
    inputNames[1] = new char[3];
    inputNames[0] = "x1";
    inputNames[1] = "x2";
   char **outputNames = new char*[2];//标记两个输出节点
   outputNames[0] = new char[4];
    outputNames[1] = new char[6];
    strcpy(outputNames[0], "sum");
   strcpy(outputNames[1], "carry");
    FILE *f = fopen("./HalfAdder.dot", "w");
    //manager：CUDD管理器.
    //2：输出的数量
    //outputs：输出节点的数组
   Cudd_DumpDot(manager, 2, outputs, inputNames, outputNames, f);
  }

int main()
{
  //                    初始化管理器            
  //0                   初始化bdd变量的数目
  //0                   初始化zdd变量的数目.
  //CUDD_UNIQUE_SLOTS： 初始化唯一表的大小（256）
  //CUDD_CACHE_SLOTS：  初始化缓存大小（262144）
  //0                   最大目标内存（0代表没有限制）
  DdManager *manager = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  //创建BDD
  DdNode **outputs = createHalfAdderBDD(manager);
  //写入BDD到一个点文件
  toDot(manager, outputs);
  test(manager, outputs);
  return 0;
}
