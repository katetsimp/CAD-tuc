

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#define MAXSIZE 20 // Maximum size of the polynomial

int AddSubCounterD=0; // Counter for additions and subtractions in delta mode
int AddSubCounterA=0; // Counter for additions and subtractions in analytical mode
int MultDCounter=0; // Counter for multiplications in delta mode
int MultACounter=0; // Counter for multiplications in analytical mode
int divDCounter=0; // Counter for divisions in delta mode
int divACounter=0; // Counter for divisions in analytical mode


struct result {
    float xk, fx; // The root and the function value at the root
    int step;// The number of iterations taken

};
typedef struct result Struct;


float CalcPolF(float ,int, int[],int);
float CalculateDer(float,float,float);
float findX0(int, int[]);
float NewtonRaphsonMethod(float,float,float,int);
Struct CalcforNewRapsMethodForDelta(float,int, int[],float,int);
void CalculateRootWithDelta(float,int, int[]);
Struct CalcforNewRapsMethodForAn(float,int, int[],int[],int);
void CalculateRootWithAn(float,int, int[],int[]);

/* This function calculates the value of a polynomial of degree d with coefficients stored in numOfPol[] at the given point x
Inputs:
- x: the point at which to evaluate the polynomial
- d: the degree of the polynomial
- numOfPol[]: an array of coefficients of the polynomial
- mode: an integer representing the mode of operation
mode=1 means debug mode, mode=0 means normal mode
Output:
- fx: the value of the polynomial at x
*/
float CalcPolF( float x,int d, int numOfPol[],int mode){
  float fx;
  int i;
  fx = numOfPol[0];
// loop through the coefficients and evaluate the polynomial
  for (i = 1; i <= d; i++)
    {
        fx = fx * x + numOfPol[i];
// if delta mode is on, increment counters for addition/subtraction and multiplication operations for delta.
 if (mode==1){
        AddSubCounterD++;

        MultDCounter++;
//if analytical mode is on, increment counters for addition/subtraction and multiplication operations
 }else if(mode==0){
   AddSubCounterA++;
   MultACounter++;

  }else{
//do nothing

  }
    }

// return the value of the polynomial at x
return fx;
}
/* This function calculates the derivative of a function at a given point using the method with delta. /
/ Parameters:
- fx: the function value at the given point
- fxd: the function value at the nearby point
- delta: the distance between the given and nearby points
Returns:
- fd: the estimated derivative value at the given point
*/
float CalculateDer(float fx,float fxd,float delta){
    float fd;
    fd=(fxd-fx)/delta;
// Increment the counters for addition/subtraction and division operations
    AddSubCounterD++;
    divDCounter++;

return fd;

}
/*This function finds a root (x0) of a polynomial function, given its degree and array of coefficients.
It uses the bisection method to search for the root within a range of values.
The range is defined as [-20, 20], and the number of iterations is set to 20.
Inputs:
- Degree: an integer representing the degree of the polynomial
- NumOfPol: an integer array representing the coefficients of the polynomial
Outputs:
- x0: a float representing the root of the polynomial
*/
float findX0(int Degree, int NumOfPol[]){
    int count=20;
    int i,j;
    float f[MAXSIZE],Num[MAXSIZE];
    int mode=2;

// generate 20 random values within the range [-20, 20], and calculate the value of the polynomial at each point
for (i = 0; i < count; i++) {
         Num[i]= -20 + rand() % (20+1 - (-20));
         f[i]= CalcPolF(Num[i],Degree,NumOfPol,mode);

}
// apply the bisection method to find the root
for (i = 0; i < count; i++)
    {
    for (j=1; j<count;j++)
    {
       if ( f[i]*f[j]<0)
        {

        return ((Num[i]+ Num [j])/2);
       }

    }

}
// return a default value of -10 if the root is not found
return -10;
}
/* This function implements the Newton-Raphson method to find the root of a polynomial
// Inputs: fx - the value of the polynomial at the current iteration
// fd - the derivative of the polynomial at the current iteration
// xk - the current approximation of the root
// mode - a flag indicating whether to count arithmetic operations as additions/subtractions (mode=0) or multiplications/divisions (mode=1)
// Output: xk1 - the new approximation of the root*/
float NewtonRaphsonMethod(float fx,float fd,float xk,int mode){
    float xk1;
    float fk1;



// check if the derivative is zero to avoid dividing by zero
		  if(fd == 0.0)
		  {
			   printf("Mathematical Error.");
			   exit(0);
		  }

// calculate the new approximation using the Newton-Raphson formula
		  xk1 = xk - fx/fd;
// update counters based on the mode
//delta mode
		  if(mode==1){
          AddSubCounterD++;

          divDCounter++;
//analytic mode
		  }else{
		  AddSubCounterA++;
		  divACounter++;
		  }

printf("\n xk = %6.5f \n", xk1);
return xk1;

}
/* This function calculates the value of fx, fxd and fd for the Newton-Raphson method
It takes the current value of xk, degree of polynomial, the polynomial coefficients array NumOfPol,
the value of delta and the current iteration step as input parameters and returns a struct of type 'Struct'
containing the updated value of xk, fx, and iteration step.
Input:float xk: current value of xk
int Degree: degree of polynomial
int NumOfPol[]: array containing the coefficients of the polynomial
float delta: value of delta used in calculating fxd and fd
int step: current round
Output:

Struct s: a struct containing the updated value of xk, fx, and iteration step
*/
Struct CalcforNewRapsMethodForDelta(float xk,int Degree, int NumOfPol[],float delta,int step){
// Create a struct s and initialize step to increment.
Struct s;
step++;
// Set mode to 1 and print the current round.
int mode=1;
printf("\n round: %d", step);
float  fx,fxd,fd;
// If the number of iteration steps exceeds 25, exit and print an error message
    if(step > 25)
		  {
			   printf("\nNot Convergent.");

		  }
else{
// calculate the value of fx, fxd and fd using the current value of xk and delta
fx= CalcPolF(xk,Degree,NumOfPol,mode);
fxd=CalcPolF(xk+delta ,Degree,NumOfPol,mode);
fd=CalculateDer( fx, fxd, delta);
// Calculate the next xk value using the Newton-Raphson method and store it in the struct s.
xk=NewtonRaphsonMethod(fx,fd,xk,mode);
s.xk=xk;
s.fx=fx;
}
s.step=step;

return s;
}
/*
 * This function calculates the root of a polynomial using Newton-Raphson method with delta
 * It takes the initial value of xk, degree of the polynomial, and an array of polynomial coefficients as input.
 * It uses the CalcforNewRapsMethodForDelta() function to calculate the next xk and check convergence.
 * It prints the total rounds, add/sub, multiplication, and division counts.
 */
void CalculateRootWithDelta(float xk,int Degree, int NumOfPol[]){
Struct r;
float delta=0.0001;
int step=1;
// print the initial round and value of xk
printf("\n round: %d", step);
printf("\n x0 = %6.5f \n",xk);
do{
r=CalcforNewRapsMethodForDelta(xk, Degree,  NumOfPol, delta,step);
xk=r.xk;
step=r.step;
// check if the function value is not converging after 25 rounds
if (step > 25){
printf("\nTotal rounds: %d", step);
printf("\nTotal add/sub: %d", AddSubCounterD);
printf("\nTotal multiplications: %d",MultDCounter);
printf("\nTotal division: %d",divDCounter);
    return;
}
}while(fabs(r.fx)>0.001);

// print the root and the total counts of operations
printf("\nRoot is: %6.5f", xk);
printf("\nTotal rounds: %d", step);
printf("\nTotal add/sub: %d", AddSubCounterD);
printf("\nTotal multiplications: %d",MultDCounter);
printf("\nTotal division: %d",divDCounter);
}
/* This function calculates the value of fx, fdA for the Newton-Raphson method
It takes the current value of xk, degree of polynomial, the polynomial coefficients array NumOfPol,
an array with the coefficients of the derivative of the polynomial NumOfPolD  and the current iteration step as input parameters and returns a struct of type 'Struct'
containing the updated value of xk, fx, and iteration step.
Input:float xk: current value of xk
int Degree: degree of polynomial
int NumOfPol[]: array containing the coefficients of the polynomial
int NumOfPolDD[]: array containing the derivative coefficients of the polynomial used in calculating  fdA
int step: current round
Output:

Struct s: a struct containing the updated value of xk, fx, and round
*/
Struct CalcforNewRapsMethodForAn(float xk,int Degree, int NumOfPol[],int NumOfPolD[],int step){
// Create a struct s and initialize step to increment.
Struct s;
step++;
// Set mode to 0 and print the current round.
int mode=0;
printf("\n round: %d", step);
float  fx,fdA;
// If the number of iteration steps exceeds 25, exit and print an error message
    if(step > 25)
		  {
			   printf("Not Convergent.");

		  }
else{
// Calculate fx and fdA using the current xk, Degree, NumOfPol, and mode.
fx= CalcPolF(xk,Degree,NumOfPol,mode);
fdA=CalcPolF(xk,Degree-1,NumOfPolD,mode);
// Calculate the next xk value using the Newton-Raphson method and store it in the struct s.
xk=NewtonRaphsonMethod(fx,fdA,xk,mode);
s.xk=xk;
s.fx=fx;
}
s.step=step;
return s;

}
/* This function calculates the root of a polynomial using Newton-Raphson method with an analytic method
    The function takes the initial guess of the root (xk), degree of the polynomial (Degree),
    the coefficients of the polynomial (NumOfPol), the coefficients of the derivative of the polynomial (NumOfPolD)
    and initializes the step counter (step) */
void CalculateRootWithAn(float xk,int Degree, int NumOfPol[],int NumOfPolD[]){
Struct r;
int step=1;
// print the initial round and value of xk
printf("\n round: %d", step);
printf("\n x0 = %6.5f \n",xk);
do{
r=CalcforNewRapsMethodForAn(xk, Degree,  NumOfPol,NumOfPolD,step);
xk=r.xk;
step=r.step;
// check if the function value is not converging after 25 rounds
if (step >25){
printf("\nTotal rounds: %d", step);
printf("\nTotal add/sub: %d", AddSubCounterA);
printf("\nTotal multiplications: %d",MultACounter);
printf("\nTotal division: %d",divACounter);
    return;
}
}while(fabs(r.fx)>0.001);
// print the root and the total counts of operations
printf("\nRoot is: %6.5f", xk);
printf("\nTotal rounds: %d", step);
printf("\nTotal add/sub: %d", AddSubCounterA);
printf("\nTotal multiplications: %d",MultACounter);
printf("\nTotal division: %d",divACounter);
}
/* Main function */
void main (){
/* Inputs */
 //variable for polynomial degree
int i,deg;
int Degree =0;
int NumOfPol[MAXSIZE],NumOfPolD[MAXSIZE];//variable for the coefficients  of the polynomial function

float x0;
printf("Type the polynomial degree: \n"); //Ask the user as input the degree
scanf("%d", &Degree);// Get and save the Degree

printf("Enter the coefficients  of the polynomial function ,max number  of coefficients  is: %d ,separated by spaces \n",Degree+1);
for(i=0;i<Degree+1;i++){
scanf("%d", &NumOfPol[i]);
}
printf("degree is :%d\n",Degree);
printf("the polynomial function is:");

deg = Degree;
for (i = 0; i <= Degree; i++)
    {
        if (deg < 0)
        {
            break;
        }
        /*  printing proper polynomial function */
        if (NumOfPol[i] > 0)
            printf(" + ");
        else if (NumOfPol[i] < 0)
            printf(" - ");
        else
            printf(" ");

        NumOfPolD[i]=NumOfPol[i]*deg;//make the coefficients of the derivative function
        printf("%dx^%d  ", abs(NumOfPol[i]), deg--);



    }
    printf("the derivative is:");
    deg = Degree-1;
for (i = 0; i <= Degree; i++)
    {
        if (deg < 0)
        {
            break;
        }
         /* Printing the derivative */
        if (NumOfPol[i] > 0)
            printf(" + ");
        else if (NumOfPol[i] < 0)
            printf(" - ");
        else
            printf(" ");


        printf("%dx^%d  ", abs(NumOfPolD[i]), deg--);



    }

x0=findX0(Degree,NumOfPol);
printf("\nderivative with arithmetic method delta:");
//Function to calculate the root using the arithmetic method with delta
CalculateRootWithDelta(x0,Degree, NumOfPol);
//Function to calculate the root using the analytical method
printf("\nderivative with analytic method:");
CalculateRootWithAn(x0, Degree, NumOfPol, NumOfPolD);


}


