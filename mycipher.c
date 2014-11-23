
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int K1[8];												//will contain the 1st key
int K2[8];												//will contain the 2nd and final key			

const int P10 [] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
const int P8 [] =  {6, 3, 7, 4, 8, 5, 10, 9};
const int IP [] = {2, 6, 3, 1, 4, 8, 5, 7};
const int IP_INV[] = {4, 1, 3, 5, 7, 2, 8, 6};
const int EP[] = {4, 1, 2, 3, 2, 3, 4, 1 };

const int S0[][4]  = {{1, 0, 3, 2},
					 {3, 2, 1, 0},
					 {0, 2, 1, 3},
					 {3, 1, 3, 2}};

const int S1[][4]  = {{0, 1, 2, 3},
					 {2, 0, 1, 3},
					 {3, 0, 1, 0},
					 {2, 1, 0, 3}};



int IP_Output[8];
int IP_Inv_Output[8];
int F_Mapping_Output[4];
int f1_Function_Output[8];
int f2_Function_Output[8];
int S0_out[2];
int S1_out[2];



void gen_keys(int * key);
void print_array(int * array);
void left_shift_1(int * bits, int low, int high);
void left_shift_2(int * bits, int low, int high);
void int_to_bitset(int value, int * bitset, int N);
int bitset_to_int(int * bitset, int N);
void IP_function(int * input);
void IP_INV_function(int * input);
void raise_error(char * msg) { perror(msg); exit(-1); } 
void swap(int * array, int i , int j);
void S_function(int * input,int low, int high, const int S[4][4]  , int * S_out );
void F_mapping(int * input, int * sk, int low, int high, int * output);
void f_function(int * input, int * sk, int * output);		//4bit
void SW_function(int * input);
int encrypt(char c);
int decrypt(char c);
void XOR(int * a, int * b, int N);


int main (int argc, char ** argv)
{
	int arr [] = {1,1,1,1};
	printf("%d\n", bitset_to_int(arr,4));


	char c = 'h';   	
	int K [] = {1,0,1,0,0,0,0,0,1,0};
	gen_keys(K);	

	printf("char = %c\n", c);
	int cipher = encrypt(c);
	printf("encrypt() -> %c\n", cipher);
	printf("decrypt() -> %c\n", decrypt(cipher));


//	IP_INV_function(IP_Output);

	

}


void gen_keys(int * K)
{
	
	int permuted[10];									//will contain the results of the permutation with P10

	int i;
	for(i = 0; i < 10; i++){							//P10() permute
		permuted[i] = K[P10[i]-1];
	}
	left_shift_1(permuted, 0, 4);						//apply circular LS-1 to the 5 lower and upper bits 
	left_shift_1(permuted, 5, 9);							
	
	for(i = 0; i < 8; i++){								//P8() permute
		K1[i] = permuted[P8[i]-1];
	}
	printf("K1: ");
	print_array(K1);

	left_shift_2(permuted,0,4);							//apply circular LS-2 to the 4 lower and upper bits 
	left_shift_2(permuted,5,9);

	for(i = 0; i < 8; i++){								//generate the K2 from the K1 using P8()
		K2[i] = permuted[P8[i]-1];
	}
	printf("K2: ");
	print_array(K2);



}

void left_shift_1(int * bits, int low, int high)
{
	int first = bits[low];
	int i = low + 1;
	for(; i <= high; i++){
		bits[i-1] = bits[i];
	}
	bits[--i] = first;
}

void left_shift_2(int * bits, int low, int high)
{
	left_shift_1(bits,low,high);
	left_shift_1(bits,low,high);
}

void IP_function(int * input)
{
	int i;
	for(i = 0; i < 8; i++){								//IP permute
		IP_Output[i] = input[ IP[i] - 1];
	}
	printf("My IP: ");
	print_array(IP_Output);
}
void IP_INV_function(int * input)
{
	int i;
	for(i = 0; i < 8; i++){								//IP permute
		IP_Inv_Output[i] = input[ IP_INV[i] - 1];
	}
	printf("My IP_INV: ");
	print_array(IP_Inv_Output);
}


void print_array(int * array)
{
	int i;
	for(i = 0; i < 8; i++){
		printf("%d", array[i]);
	}
	printf("\n");
}
void swap(int * array, int i , int j)
{
	int temp = array[i];
	array[i] = array[j];
	array[j] = temp;
}	

void int_to_bitset(int value, int * bitset, int N)
{
	if(value < 0){
	//	raise_error("invalid number range. exiting...");
	}	
	memset(bitset,0,N);
	int i;
	for(i = N-1; i >= 0; i--){
		bitset[i] = value % 2;
		value /= 2;
	}
}

int bitset_to_int(int * bitset, int N)
{
	int i, value  =0;
	int p = 0;
	for(i = N - 1; i >= 0; i--){
		if (bitset[i] == 1){
			value += pow(2,p);
		}
		p++;
	}

	return value;
}

int encrypt(char c){							//pass output as arg to the functions;
	int ciphertext[8];
	int_to_bitset(c,ciphertext,8);				//will convert the int value to a bitset
	printf("BitSet: ");
	print_array(ciphertext);					

	IP_function(ciphertext);
	f_function(IP_Output, K1, f1_Function_Output);

	printf("After f(): ");
	print_array(ciphertext);				

	SW_function(f1_Function_Output);
	f_function(f1_Function_Output, K2, f2_Function_Output);
	IP_INV_function(f2_Function_Output);
	return bitset_to_int(IP_Inv_Output,8);
}
int decrypt(char c){
	int text[8];
	int_to_bitset(c,text,8);					//will convert the int value to a bitset
	printf("BitSet: ");
	print_array(ciphertext);					
	IP_function(text);
	f_function(IP_Output, K2, f2_Function_Output);
	SW_function(f2_Function_Output);
	f_function(f2_Function_Output, K1, f1_Function_Output);
	IP_INV_function(f1_Function_Output);
	return bitset_to_int(IP_Inv_Output,8);
}


void SW_function(int * input){
	swap(input,0,3);
}


void f_function(int * input, int * sk, int * output)
{	

	//fK(L, R) = (L XOR F(R, SK), R)
	//sk some key
													//let L and R be the left and right 4 bits of input
	F_mapping(input, sk, 4, 7, F_Mapping_Output);	//F(R, SK)
	XOR(F_Mapping_Output, input, 4);				//L XOR F(R, SK) 		
	int i;
	for(i = 0; i < 4; i++)
		output[i] = F_Mapping_Output[i];
	for(i = 4; i < 8; i++)
		output[i] = input[i];

}

void F_mapping(int * input, int * sk, int low, int high, int * output){
	int EP_Ouput[8];
	int i;
	for(i = 0; i < 8; i++){							//EP() expand and permute and XOR() to LEFT
		EP_Ouput[i] = input[EP[i]-1] ^ sk[i];
	}												
													//S0() and S1() boxes
	S_function(EP_Ouput,0,3,S0,S0_out);
	S_function(EP_Ouput,4,7,S1,S1_out);				//F() ---> SO() Union S1()  

	//int P4[] = {2, 4, 3, 1};						//P4 permutation
	output[0] =  S0_out[1];
	output[1] =  S1_out[1];
	output[2] =  S1_out[0];
	output[3] =  S0_out[0];
}

void S_function(int * input,int low, int high, const int S [4][4]  , int * S_out ){
	int i_arr[] = {input[low], input[high]};
	int j_arr[] = {input[low+1], input[high-1]};
	int i = bitset_to_int(i_arr,2);
	int j = bitset_to_int(j_arr,2);
	int val = S[i][j];
	printf("val: %d\n", val);
	int_to_bitset(val, S_out, 2);

}


void XOR(int * a, int * b, int N){
	for(int i = 0; i < N; i++)
		a[i] = a[i] ^ b[i];
}



