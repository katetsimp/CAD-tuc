/* Author: Katerina Tsimpirdoni
   Compiler: GNU GCC Compiler
   Date: 19/03/2023
  The program  creates a netlist for a full adder circuit with N bits.
  Ask the user for the number of bits and  Validate user input.
  It defines three structures - Component, netlist, and netlist_equals,
  to store the components, the netlist, and the netlist equivalences respectively.
  The add_component function sets and adds a new component to the netlist.
  The addComponentToTheNetlist function adds a component to a netlist.
  The CreateNetlistForFullAdder function creates a netlist for a full adder circuit with N bits.
  It initializes a netlist, creates the necessary components for each full adder, and adds them to the netlist.
  Finally, it writes the netlist to a file.
   */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Define constants for the maximum number of characters, inputs, outputs, and components
#define MAX_CHARACTERS 20
#define MAX_INPUTS 4
#define MAX_OUTPUTS 3
#define MAX_COMPONENTS 100
//struct for the components
typedef struct Component {
    int id;                             // unique identifier for each component
    char type[MAX_CHARACTERS];          // type of component
    char inputs[MAX_INPUTS][MAX_CHARACTERS];        // input ports of the component
    char output[MAX_OUTPUTS][MAX_CHARACTERS];       // output ports of the component
    struct Component* next;             // pointer to next component in the list
} Component;
//struct for the netlist
typedef struct {
    Component components[MAX_COMPONENTS];    // array of components
    int num_components;                     // number of components in the netlist
} netlist;
// struct for the operator '='
typedef struct {
    char input[MAX_CHARACTERS];
    char output[MAX_CHARACTERS];
} netlist_equals;

// function to add a new component to the netlist
void add_component(netlist*, Component**, char*, char[MAX_INPUTS][MAX_CHARACTERS], char[MAX_OUTPUTS][MAX_CHARACTERS]);

// function to add a component to the netlist
void addComponentToTheNetlist(Component*, netlist*);

// function to create a netlist for a full adder circuit with N bits
void CreateNetlistForFullAdder(int N, FILE* fp, netlist_equals* netlist_equals_arr);

/*
 * add_component:  sets and adds a new component to the netlist
 *
 * netlist: pointer to the netlist data structure
 * head: pointer to the head of the linked list of components
 * type: type of the new component
 * inputs: array of input ports of the new component
 * output: array of output ports of the new component

 */
void add_component(netlist* netlist, Component** head, char* type, char inputs[MAX_INPUTS][MAX_CHARACTERS], char output[MAX_OUTPUTS][MAX_CHARACTERS]) {
    Component* new_component = (Component*) malloc(sizeof(Component));     // allocate memory for the new component
    new_component->id = netlist->num_components;                             // set the ID for the new component

    strcpy(new_component->type, type);                                       // set the type of the new component
    for (int i = 0; i < MAX_INPUTS; i++) {
        strcpy(new_component->inputs[i], inputs[i]);                         // set the input ports of the new component
    }
    for (int i = 0; i < MAX_OUTPUTS; i++) {
        strcpy(new_component->output[i], output[i]);                         // set the output ports of the new component
    }

    new_component->next = *head;
    *head = new_component; //add a new component at the beginning
    addComponentToTheNetlist(new_component,netlist);
}

// Function to add a component to a netlist
// c: pointer to the component to be added
// netlist: pointer to the netlist where the component will be added
void addComponentToTheNetlist(Component *c, netlist *netlist){
    int i;
    i = netlist->num_components;
    netlist->components[i] = *c;  // add the component to the netlist
    netlist->num_components++;     // increment the number of components in the netlist
}

// Function to create a netlist for a N full adder
// N: number of full adders
// fp: pointer to the file where the netlist will be written
// netlist_equals_arr: array of structures for the netlist equivalences
void CreateNetlistForFullAdder(int N, FILE *fp, netlist_equals *netlist_equals_arr){
    int i;
    netlist newnetlist = {0};       // initialize the netlist
    Component* head = NULL;
    newnetlist.num_components = 0; // set the number of components to 0
    char input_carry[MAX_CHARACTERS] = "cin";  // set the input carry
    char output_carry[MAX_CHARACTERS];         // initialize the output carry
    char inputs[MAX_INPUTS][MAX_CHARACTERS] = {0};  // initialize the inputs
    char output[MAX_OUTPUTS][MAX_CHARACTERS] = {0}; // initialize the outputs


    for (i = 0; i < N; i++){
        // create the names for  inputs, and outputs

        sprintf(inputs[0], "A%d", newnetlist.num_components);
        sprintf(inputs[1], "B%d", newnetlist.num_components);
        sprintf(inputs[2], "%s", input_carry);
        sprintf(output[0], "S%d", newnetlist.num_components);

        // if it's the last full adder, add the "Cout" output
        if (i == N-1){
            sprintf(output[0], "S%d", newnetlist.num_components);
            sprintf(output[1], "Cout");
        }

        // add the full adder component to the netlist
        add_component(&newnetlist, &head, "Full_adder", inputs, output);

        // update the input carry for the next full adder
        sprintf(output_carry, "U%d_C", newnetlist.components[i].id);
        strcpy(input_carry, output_carry);

        // set the netlist equivalences
        sprintf(netlist_equals_arr[i].output, "U%d_S", newnetlist.components[i].id);
        strcpy(netlist_equals_arr[i].input, newnetlist.components[i].output[0]);
        if (i == N-1){
            sprintf(netlist_equals_arr[i].output, "U%d_S", newnetlist.components[i].id);
            strcpy(netlist_equals_arr[i].input, newnetlist.components[i].output[0]);
            sprintf(netlist_equals_arr[i+1].output, "U%d_C", newnetlist.components[i].id);
            strcpy(netlist_equals_arr[i+1].input, newnetlist.components[i].output[1]);
        }
    }

    // write the netlist to the file
    for (i = 0; i < N; i++){
        fprintf(fp, "U%d %s ", newnetlist.components[i].id, newnetlist.components[i].type);
        for (int j = 0; j < MAX_INPUTS; j++){
            if (strcmp(newnetlist.components[i].inputs[j], "") == 0) {
                break;
            }
            if (strcmp(newnetlist.components[i].inputs[j+1], "") == 0){


            fprintf(fp,"%s", newnetlist.components[i].inputs[j]);


            }else{
             fprintf(fp,"%s,", newnetlist.components[i].inputs[j]);}
        }

     fprintf(fp,"\n");
    }

    for (i=0;i<N+1;i++){

        fprintf(fp,"%s=%s\n", netlist_equals_arr[i].input,netlist_equals_arr[i].output);

   }
fclose(fp);



}
int main(){
    int n;
    char filename[100];
    netlist_equals n_e[100]={0};

    // Prompt the user to enter the number of bits
printf("Please enter number of bits (1-8): ");
scanf("%d", &n);

// Validate user input
if (n < 1 || n > 8) {
    printf("Invalid input. Please enter a number between 1 and 8.\n");
    return 0;
}

// Open the file for writing
strcpy(filename, "full_adder_netlist");
FILE *fp = fopen(filename, "w");
if (fp!=NULL){
    printf("file:%s opened successfully\n", filename);
}

// Create the netlist for the full adder circuit
CreateNetlistForFullAdder(n, fp,n_e);

// Exit the program
return 0;
}
