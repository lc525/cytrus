#include <iostream>
using namespace std;

/* this function calculates the factorial of a given number
 * using recursion. 
 */
long factR(short number){
	if(number==0 || number==1) return 1;
	return factR(number-1)*number;
}


/* this function calculates the factorial of a given number
 * using an iterative method. 
 */
long fact(short number){
	long result=1;
	if(number==0) result=1;
	for(int i=1; i<=number; i++){
		result*=i;
	}
	return result;
}


int main(int argc, char** argv){
	cout<<"Hello World"<<endl;
	cout<<"15! calculat recursiv:"<<factR(15)<<endl;
	cout<<"15! calculat iterativ:"<<fact(15)<<endl;
	return 0;
}
