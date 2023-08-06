/* katerina tsimpirdoni 02/04/2023, compiled with:GNU GCC Compiler
this code combine a subsystem netlist ,that made from a Component library ,and a input file netlist into a final output file netlist.
The program reads an input file containing component declarations and a subsystem library file containing sub-circuits.
It then maps the subsystems from the library to the components in the input file and generates a combined netlist.




*/
// Include necessary header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// Define constants for the maximum number of characters, inputs, outputs, and components
#define MAX_CHARACTERS 20 // maximum number of characters
#define MAX_INPUTS 10 // maximum number of inputs
#define MAX_OUTPUTS 10 // maximum number of outputs
#define MAX_COMPONENTS 100 // maximum number of components
#define MAX_CONNECTIONS 10 // maximum number of connections
#define MAX_NETLISTS 100

// Define the Component structure for make Componenet library
typedef struct {
char* name; // name of the component
char** inputs; // input ports of the component
int input_count; // number of input ports
} Component_S;

// Define the ComponentLibrary structure
typedef struct {
Component_S* components; // array of components
int component_count; // number of components
} ComponentLibrary;

// Define the Component structure
typedef struct Component {
int id; // unique identifier for each component
char type[MAX_CHARACTERS]; // type of component
char inputs[MAX_INPUTS][MAX_CHARACTERS]; // input ports of the component
int connections[MAX_CONNECTIONS]; // array to store connections
int num_connections; // number of connections
int numofinputs; // number of input ports
} Component;

// Define the netlist structure
typedef struct {
Component components[MAX_COMPONENTS]; // array of components
int num_components; // number of components in the netlist
} netlist;

// Define the Component_Dec structure
typedef struct Component_Dec{
char inputs[MAX_INPUTS][MAX_CHARACTERS]; // input ports of the component
char Output[MAX_OUTPUTS][MAX_CHARACTERS]; // output ports of the component
char name[MAX_CHARACTERS]; // name of the component
int numofinputs; // number of input ports
int numofoutputs; // number of output ports
} Component_Dec;

// Define the Component_Dec_Array structure
typedef struct Component_Dec_Array{
Component_Dec CDA[MAX_COMPONENTS]; // array of component declarations
int CDA_num; // number of components in the array
} Component_Dec_Array;

// Define the Output_Dec structure
typedef struct Output_Dec{
char *Output; // output port name
int origin; // index of the originating component
} Output_Dec;

// Define the OutputArray structure
typedef struct OutputArray{
Output_Dec Output_Dec_array[MAX_COMPONENTS]; // array of output declarations
int output_num; // number of outputs in the array
} OutputArray;
//This struct represents a subsystem file, containing a component
// declaration array, a netlist of subnets, and an output array.
typedef struct subsystemFile{
Component_Dec_Array *CDA;
netlist *subnetlist;
OutputArray* new_out_array;


}subsystemFile;
// This struct represents an input file, containing a netlist and an output array.
typedef struct InputFile{
netlist *new_netlist;
OutputArray* outputarray;




}InputFile;
// This struct represents a set of three indexes.
typedef struct {
    int index1;
    int index2;
    int index3;
} Indexs;


// This struct represents an array of netlists with a specified maximum size.
typedef struct {
    netlist netlists[MAX_NETLISTS];
    int num_netlists;
} netlistArray;

// This struct represents an output file, containing a netlist array and an output array.
typedef struct{
netlistArray* Net_Array;
OutputArray *OUT_Array;
}OutputFile;








/*-----------------------------all the add functions---------------------------------------------------------------------*/





/* Adds a new netlist to the provided netlist array.
 If the array is full, the function returns without adding the new netlist.*/
void add_netlist(netlistArray *array, netlist* new_netlist) {
    // Check if the array is full
    if (array->num_netlists >= MAX_NETLISTS) {
        // The array is full, cannot add more netlists
        return;
    }

    // Add the new_netlist to the array and increment the num_netlists counter
    array->netlists[array->num_netlists] = *new_netlist;
    array->num_netlists++;

    // Successfully added the new_netlist
    return;
}



/* Function to add an output to the output array.
new_out: pointer to the output to be added.
new_out_array: pointer to the output array where the output will be added.*/
void addOutputToTheArray(Output_Dec* new_out, OutputArray* new_out_array) {
    int i = new_out_array->output_num;

    if( new_out_array->output_num<0){
            i=0;


    }

    if (new_out_array->Output_Dec_array == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for components.\n");
        return;
    }

    // Add the component to the netlist
    new_out_array->Output_Dec_array[i] = *new_out;

    // Increment the number of components in the netlist
    new_out_array->output_num++;
}
/* Function to add an output to the output array with origin information.
new_out_array: pointer to the output array where the output will be added.
new_out: pointer to the output to be added.
Output: name of the output.
origin: origin information for the output. */
void addOutput(OutputArray* new_out_array, Output_Dec* new_out, char *Output, int origin) {
    // Allocate memory for new_out->Output
    new_out->Output = (char *)malloc((strlen(Output) + 1) * sizeof(char));

    strcpy(new_out->Output, Output);
    new_out->origin = origin;

    addOutputToTheArray(new_out,new_out_array);
}


// Function to add a component to a netlist
// c: pointer to the component to be added
// netlist: pointer to the netlist where the component will be added
void addComponentToTheNetlist(Component *c, netlist *netlist) {
    int i = netlist->num_components;

    if(netlist->num_components<0){
            i=0;


    }

    if (netlist->components == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for components.\n");
        return;
    }

    // Add the component to the netlist
    netlist->components[i] = *c;

    // Increment the number of components in the netlist
    netlist->num_components++;
}
/* Function to add a component with its properties to a netlist
   netlist: pointer to the netlist where the component will be added
   new_component: pointer to the component to be added
   type: type of the component
   inputs: array of input port names
   id: ID of the component
   connections: array of connection indices*/
void add_component(netlist* netlist,Component* new_component, char* type, char inputs[MAX_INPUTS][MAX_CHARACTERS],int id,int connections[MAX_CONNECTIONS]) {

    new_component->id = id;// set the ID for the new component

    strcpy(new_component->type, type);                                       // set the type of the new component
    for (int i = 0; i < new_component->numofinputs; i++) {
        strcpy(new_component->inputs[i], inputs[i]);                         // set the input ports of the new component
    }
    for (int i = 0; i < new_component->num_connections; i++) {
     new_component->connections[i]=connections[i];

    }


    addComponentToTheNetlist(new_component,netlist);
}
/* Function to add a component to the component declaration array
  c: pointer to the component to be added
  CDarray: pointer to the component declaration array where the component will be added*/
void addComponentToTheDeclarationArray(Component_Dec *c, Component_Dec_Array *CDarray) {

    int i = CDarray->CDA_num;
     if(CDarray->CDA_num<0){
             i=0;



     }


    if (CDarray->CDA == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for components.\n");
        return;
    }

    // Add the component to the component declaration array
    CDarray->CDA[i] = *c;

    // Increment the number of components in the component declaration array
    CDarray->CDA_num++;
}
/* Function to add a component declaration with its properties to a component declaration array
   Component_Dec_Array: pointer to the array where the component declaration will be added
   newComponent_Dec: pointer to the component declaration to be added
   name: name of the component
    inputs: array of input port names
    outputs: array of output port names*/
void add_component_Dec(Component_Dec_Array *Component_Dec_Array,Component_Dec* newComponent_Dec,char name[MAX_CHARACTERS],char inputs[MAX_INPUTS][MAX_CHARACTERS],char outputs[MAX_OUTPUTS][MAX_CHARACTERS]){
strcpy(newComponent_Dec->name, name);//Set the name of the new component declaration


for (int i = 0; i < newComponent_Dec->numofinputs; i++) {
        strcpy(newComponent_Dec->inputs[i], inputs[i]);   // set the input ports of the new component
    }
// Set the output ports of the new component declaration
 for(int i=0;i<newComponent_Dec->numofoutputs;i++){

     strcpy(newComponent_Dec->Output[i], outputs[i]);


 }
// Add the component declaration to the component declaration array
addComponentToTheDeclarationArray(newComponent_Dec,Component_Dec_Array);







}
//-----function for create Component and subsystem Libraries-----------

/* Creates a new component library and returns a pointer to it.
 Components are added to the library and the library is written to a file.*/
ComponentLibrary* MakeComponentLibrary() {
    // Create the component library
    ComponentLibrary *library=(ComponentLibrary*)malloc(sizeof(ComponentLibrary*));
    library->component_count = 0;

    // Add components to the library
    char* inputs[] = {"P", "Q"};
    char* names[] = {"NOT", "NAND2", "NOR2", "XOR2", NULL};
    int names_count = 0;
    //store it in struct ComponentLibrary
    while (names[names_count] != NULL) {
        names_count++;
    }

    if (names_count == 0) {
        library->components = NULL;
    } else {
        library->components = (Component_S*)malloc(names_count * sizeof(Component_S));
        for (int i = 0; i < names_count; i++) {
            if (strcmp(names[i], "NOT") == 0) {
                library->components[i] = (Component_S){names[i], &inputs[0], 1};
            } else {
                library->components[i] = (Component_S){names[i], inputs, 2};
            }
            library->component_count++;
        }
    }

    // Write the component library to a file
    FILE* file = fopen("component_library.txt", "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return NULL;
    }
    fprintf(file,"%% THIS IS THE COMPONENT LIBRARY, COMPRISING OF GATES\n"
    "** COMPONENT LIBRARY\n");

    for (int i = 0; i < library->component_count; i++) {
        Component_S* comp = &library->components[i];
        fprintf(file, "COMP %s ; IN:", comp->name);
        for (int j = 0; j < comp->input_count; j++) {
            fprintf(file, "%s%s", comp->inputs[j], j < comp->input_count - 1 ? "," : "");
        }
        fprintf(file, "\n");
    }

    fclose(file);

    // Free the memory allocated for the component library


    return library;

}

// Checks if a component with the given name is present in the provided component library.
bool is_component_in_library(const char* name, ComponentLibrary* library) {
    for (int i = 0; i < library->component_count; i++) {
        if (strcmp(name, library->components[i].name) == 0) {
            return true;
        }
    }
    return false;
}
/* Function that creates a subsystem file using the provided component library.
library: pointer to the component library.
filename: name of the file where the subsystem will be written. */
void MakeSubsystemFile(ComponentLibrary* library,char * filename){
    int i=0;
    // this is the gates that we use
char* names[] = {"NOT", "NAND2", "NOR2", "XOR2",NULL};
// Check if the components exist in the library
while(names[i]!=NULL){
if(is_component_in_library(names[i],library)==true){


i++;

}else{

printf("Error: Component '%s' not found in the library.\n",names[i]);
i++;


}
}
// Create and open the subsystem file
FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

// Write the subsystem library header and the full adder netlist to the file
 fprintf(file,
        "%%%% THIS IS THE SUBSYSTEM LIBRARY, COMPRISING OF SUBSYSTEMS\n"
        "%%%% MADE OF GATES FROM THE COMPONENT LIBRARY\n"
        "** SUBSYSTEM LIBRARY\n"
        "COMP FULL_ADDER ; IN:A,B,C ; OUT: S, COUT\n"
        "BEGIN FULL_ADDER NETLIST\n"
        "U0 XOR2 A,B\n"
        "U1 XOR2 U0,C\n"
        "U2 NAND2 C,U0\n"
        "U3 NAND2 A,B\n"
        "U4 NOT U3\n"
        "U5 NOT U2\n"
        "U6 NOR2 U4,U5\n"
        "U7 NOT U6\n"
        "Sum=U1\n"
        "COUT=U7\n"
        "END FULL_ADDER NETLIST\n");

    fclose(file);








}
/*-------------function for read and store subsystemFile-------------------------------------------*/
/* Function to parse and store data based on a specified label
   array:  array to store the parsed data
   temp:  array containing input strings to be parsed
    index: index of the input string in the temp array to be parsed
    label: label to identify the section of the string to be parsed
    Returns: count of the parsed data items stored in the array*/
int CutAndStore(char array[][MAX_CHARACTERS], char temp[3][256], int index, char* label){
    int Counter=0;
    char *token2=":";
    char *token3=",";
    char*temp1;
    char*temp2;


   int i = 0;
// Check if the given label exists in the input string
if (strstr(temp[index], label) != NULL) {
    // cut the input string using ':' as the delimiter
    strtok(temp[index], token2);
    temp1 = strtok(NULL, token2);

    // Check if the tokenized string contains ','
    if (strstr(temp1, token3) != NULL) {
        // cut the string using ',' as the delimiter
        temp2 = strtok(temp1, token3);
        sscanf(temp2, "%s", array[i]);

        // Continue cut and storing the data items in the array
        while (temp2 != NULL) {
            Counter++;
            i++;
            temp2 = strtok(NULL, token3);
            if (temp2 != NULL) {
                sscanf(temp2, "%s", array[i]);
            }
        }
    } else {
        // If there is only one item in the string, store it in the array
        if (strlen(temp1) == 1) {
            sscanf(temp1, "%s", array[i]);
        }
    }
} else {
    // Print an error message if the required label is not found in the input string
    printf("Something is wrong with the netlist; inputs must be declared even if empty, and before output.");
}

return Counter;
}
/* Function to read and parse a component declaration
 line: input string containing the component declaration
 Component_Dec_Array: pointer to an array where the parsed component declaration will be stored*/
void readTheComponent(char line[256],Component_Dec_Array *Component_Dec_Array){



Component_Dec* new_component_D=(Component_Dec*)malloc(sizeof(Component_Dec));
    char *token1=";";
    char*temp1;
     int i;

    char temp[3][256];
char name[MAX_CHARACTERS] = {0};
char inputs[MAX_INPUTS][MAX_CHARACTERS] = {0};
char outputs[MAX_OUTPUTS][MAX_CHARACTERS] = {0};

// cut the input string using ';' as the delimiter
temp1 = strtok(line, token1);

// Check if the tokenized string contains "COMP" and extract the component name
if (strstr(temp1, "COMP") != NULL) {
    sscanf(temp1, "COMP %s", name);
} else {
    printf("Something is wrong with the sub netlist");
}

// Continue cut the input string and store the tokens in the temp array
while (temp1 != NULL) {
    temp1 = strtok(NULL, token1);
    if (temp1 != NULL) {
        strncpy(temp[i], temp1, sizeof(temp[i]));
    }
    i++;
}

// Parse and store input and output port names in the inputs and outputs arrays
new_component_D->numofinputs = CutAndStore(inputs, temp, 0, "IN:");
new_component_D->numofoutputs = CutAndStore(outputs, temp, 1, "OUT");

// Add the parsed component declaration to the Component_Dec_Array
add_component_Dec(Component_Dec_Array, new_component_D, name, inputs, outputs);

// Free the allocated memory for the new component declaration
free(new_component_D);

}

/* Function to read and parse a netlist body line from a given string
line: input string containing the netlist body line
netlist: pointer to the netlist where the parsed component will be stored*/

void readThebody(char line[256],netlist* netlist){
Component* new_component = (Component*) malloc(sizeof(Component));   // allocate memory for the new component
int counter=0;
char *token4=" ";
char *token3=",";
char *temp1=NULL;
char *temp2=NULL;

int id = 0;
int i, k, j, n;
int connections[MAX_CONNECTIONS] = {0};
char *type = (char *)malloc(sizeof(char));
char inputs[MAX_INPUTS][MAX_CHARACTERS] = {0};
char t[3][MAX_CHARACTERS] = {0}; // for storing the split line

// Process the line if it doesn't contain "END" or "="
if (strstr(line, "END") == NULL && strstr(line, "=") == NULL) {
    i = 0;
    temp1 = strtok(line, token4);

    // Extract the component ID
    for (i = 0; i < strlen(temp1); i++) {
        k = isdigit(temp1[i]);
        if (k > 0) {
            break;
        }
    }
    if (k == 1) {
        sscanf(&temp1[i], "%d", &id);
    }

    i = 0;

    // cut the rest of the line
    while (temp1 != NULL) {
        temp1 = strtok(NULL, token4);
        if (temp1 != NULL) {
            strncpy(t[i], temp1, sizeof(t[i]));
        }
        i++;
    }

    i = 0;

    // Extract the component type and inputs
    sscanf(t[0], "%s", type);
    temp2 = strtok(t[1], token3); // split the inputs

    sscanf((char*)temp2, "%s", inputs[i]);
    i++;

    // Continue cut and extracting the inputs
    while (temp2 != NULL) {
        counter++;
        temp2 = strtok(NULL, token3);
        if (temp2 != NULL) {
            sscanf((char*)temp2, "%s", inputs[i]);
        }
        i++;
    }

    new_component->numofinputs = counter;
    n = 0;
    new_component->num_connections = 0;

    // Find the connections for the new component
    for (i = 0; i < new_component->numofinputs; i++) {
        for (j = 0; j < netlist->num_components; j++) {
            int num = atoi(&inputs[i][1]);
            if (inputs[i][0] == 'U' && num == netlist->components[j].id) {
                connections[n] = netlist->components[j].id;
                new_component->num_connections = new_component->num_connections + 1;
                n++;
            }
        }
    }

    // Add the new component to the netlist
    add_component(netlist, new_component, type, inputs, id, connections);
}

// Free the allocated memory for the new component and type
free(new_component);
free(type);

}
/* Function to read and parse an output declaration from a given string
   line: input string containing the output declaration
   new_out_array: pointer to an output array where the parsed output declaration will be stored*/
void ReadTheOutput(char line[256], OutputArray* new_out_array) {
    Output_Dec* new_out = (Output_Dec*)malloc(sizeof(Output_Dec));
    int i, k;
    int origin;
    char *temp1 = NULL;
    char *token1 = "=";
    char *Output;

    // Allocate memory for Output
    // Allocate memory for Output
Output = (char *)malloc(256 * sizeof(char));

// cut the input line using "=" as a delimiter
temp1 = strtok(line, token1);
sscanf(temp1, "%s", Output); // extract the output name
temp1 = strtok(NULL, token1);

// Find the first digit in the remaining part of the line
for (i = 0; i < strlen(temp1); i++) {
    k = isdigit(temp1[i]);
    if (k > 0) {
        break;
    }
}

// Extract the id of the component that connected
if (k == 1) {
    sscanf(&temp1[i], "%d", &origin);
    addOutput(new_out_array, new_out, Output, origin); // add the new output to the output array
}


free(Output);

}

/* Function to store the subsystem file and its content into a structured subsystemFile object
 subsysemLibrary: input string containing the path to the subsystem file
 new_netlist: pointer to the netlist where components will be stored
 new_CDA: pointer to the component declaration array where component declarations will be stored
 new_out_array: pointer to the output array where output declarations will be stored*/
subsystemFile* StoreTheSubsystemfile(char * subsysemLibrary,netlist* new_netlist,Component_Dec_Array* new_CDA,OutputArray* new_out_array){
char line[256];
subsystemFile* new_subfile = (subsystemFile*)malloc(sizeof(subsystemFile));
FILE *fptr = fopen(subsysemLibrary, "r");
    if (fptr == NULL) {
        printf("Error opening SubSystem file.");
        return NULL;
    }
   // Read the file line by line
    while (fgets(line, sizeof(line), fptr)) {
if ((strncmp(line, "%%", 2) == 0)||strncmp(line, "**", 2) == 0) {
            continue; // ignore lines starting with '%%' or ''
        }


    if (strstr(line, "COMP") != NULL && strstr(line, "IN") != NULL && strstr(line, "OUT") != NULL) {
        // Parse component declarations
        readTheComponent(line, new_CDA);
    } else {
        if (strstr(line, "BEGIN") == NULL) {
            if (strstr(line, "=") == NULL) {
                if (line != NULL) {
                    // Parse the component instances
                    readThebody(line, new_netlist);
                }
            } else {
                if (strstr(line, "END") == NULL) {
                    // Parse output declarations
                    ReadTheOutput(line, new_out_array);
                }
             }
        }
      }
}

// Assign parsed data to the new_subfile object
new_subfile->subnetlist = new_netlist;
new_subfile->CDA = new_CDA;
new_subfile->new_out_array = new_out_array;

// Return the structured subsystemFile object
return new_subfile;


}

/*---------------function about INPUTFILE----------------------------------*/
/*


   This function reads an input file, processes the content, and stores the
   necessary information into the provided netlist and output array structures.
   inputFileName: The name of the input file to be read.
   new_netlistInput: Pointer to the netlist structure where the input data will be stored.
   new_out_array_input: Pointer to the output array structure where the output data will be stored.

   returns: Pointer to a newly created InputFile structure containing the netlist and output array.*/

InputFile* readAndStoreInputFile(char * inputFileName, netlist* new_netlistInput, OutputArray* new_out_array_input) {
    char line[256];
    InputFile* new_inputFile = (InputFile*)malloc(sizeof(InputFile));
    FILE *fptr = fopen(inputFileName, "r");

    // Check if the file is opened successfully
    if (fptr == NULL) {
        printf("Error opening SubSystem file.");
        return NULL;
    }

    // Read the file line by line
    while (fgets(line, sizeof(line), fptr)) {
        // Check if the line contains an '=' sign
        if (strstr(line, "=") == NULL) {
            if (line != NULL) {
                // Process the line and store it in the netlist
                readThebody(line, new_netlistInput);
            }
        } else {
            // Check if the line contains 'END'
            if (strstr(line, "END") == NULL) {
                // Process the line and store it in the output array
                ReadTheOutput(line, new_out_array_input);
            }
        }
    }

    // Store the netlist and output array in the InputFile structure
    new_inputFile->new_netlist = new_netlistInput;
    new_inputFile->outputarray = new_out_array_input;

    return new_inputFile;
}

/*


  This function finds the index of the input location in a given subsystem.
  subsystem: Pointer to the subsystemFile structure containing information about the subsystem.
  i1: The index of the component in the subsystem's netlist.
  k1: The index of the input in the component.
  l1: The index of the CDA in the subsystem.
  returns: Pointer to an Indexs structure containing the indixes of the input location, or NULL if not found.
 */
Indexs* findtheplaceofinputs(subsystemFile *subsystem, int i1, int k1, int l1) {
    Indexs *idx = (Indexs*)malloc(sizeof(Indexs*));

    for (int j = 0; j < subsystem->CDA->CDA->numofinputs; j++) {
        if (strcmp(subsystem->CDA->CDA[l1].inputs[j], subsystem->subnetlist->components[i1].inputs[k1]) == 0) {
            idx->index1 = k1; //place of input;
            idx->index2 = i1; //component;
            idx->index3 = j;
            return idx;
        }
    }

    return NULL;
}

/*


  This function constructs the final input for the given subsystem by
  updating the netlist with the input values provided.
  subsystem: Pointer to the subsystemFile structure containing information about the subsystem.
  new_netlistFinalIn: Pointer to the netlist structure where the final input data will be stored.
  inputs: array containing the input values to be updated in the netlist.
 */
void MakeTheFinalInput(subsystemFile *subsystem, netlist *new_netlistFinalIn, char inputs[MAX_INPUTS][MAX_CHARACTERS]) {
    Indexs *idx;
    Indexs temp1[MAX_INPUTS];
    int n = 0;
    int i = 0;
    int l = 0;
    int k = 0;

    for (l = 0; l < subsystem->CDA->CDA_num; l++) {
        for (i = 0; i < subsystem->subnetlist->num_components; i++) {
            for (k = 0; k < subsystem->subnetlist->components[i].numofinputs; k++) {
                idx = findtheplaceofinputs(subsystem, i, k, l);
                if (idx != NULL) {
                    temp1[n] = *idx;
                    n++;
                }
            }
        }
    }

    int h = 0;
    while (h < n) {
        strcpy(new_netlistFinalIn->components[temp1[h].index2].inputs[temp1[h].index1], inputs[temp1[h].index3]);
        h++;
    }
}
/*---------------------------------For the OUTPUTFILE-----------------------------------*/
/*

  This function updates the component ID and its connections in the final netlist.
  new_id: The new ID to be assigned to the component.
  i1: The index of the component in the netlist.
  new_netlistFinalIn: Pointer to the netlist structure where the updated data will be stored.
  new_subfile: Pointer to the subsystemFile structure containing information about the subsystem.
 */
void MakeTheFinalIdAndConnections(int new_id, int i1, netlist *new_netlistFinalIn, subsystemFile* new_subfile) {
    int n = 0;
    int k = 0;
    int prev_id = new_netlistFinalIn->components[i1].id;

    // Update the component ID
    new_netlistFinalIn->components[i1].id = new_id;

    // Update the connections
    for (n = 0; n < new_subfile->subnetlist->num_components; n++) {
        for (k = 0; k < new_subfile->subnetlist->components[n].num_connections; k++) {
            if (new_subfile->subnetlist->components[n].connections[k] == prev_id && new_subfile->subnetlist->components[n].num_connections > 0) {
                new_netlistFinalIn->components[n].connections[k] = new_id;
            }
        }
    }
    return;
}

/*

  This function creates the body of the output file by updating the final netlist with new IDs and inputs.
  new_subfile: Pointer to the subsystemFile structure containing information about the subsystem.
  new_id: The new ID to be assigned to the component.
  new_netlistFinalIny: Pointer to the netlist structure where the updated data will be stored.
  in: array containing the input values to be updated in the netlist.
 */
void makeTheBodyFortheOutputFile(subsystemFile* new_subfile, int new_id, netlist *new_netlistFinalIny, char in[MAX_INPUTS][MAX_CHARACTERS]) {
    int i = 0;

    MakeTheFinalInput(new_subfile, new_netlistFinalIny, in);

    for (i = 0; i < new_subfile->subnetlist->num_components; i++) {
        MakeTheFinalIdAndConnections(new_id, i, new_netlistFinalIny, new_subfile);
        new_id++;
    }
}

/*

  This function checks if the input string contains the specified pattern.
  input: The input string to be checked.
  pattern: The pattern to be searched for in the input string.
 returns: true if the input string contains the pattern, otherwise false.
 */
bool contains_pattern(const char *input, char* pattern) {
    int number;
    char check[strlen(input) + 1];

    if (sscanf(input, pattern, &number) == 1) {
        sprintf(check, pattern, number);

        if (strncmp(input, check, strlen(check)) == 0) {
            return true;
        }
    }

    return false;
}
/*

  This function checks the output and returns the corresponding value based on the output pattern.
  new_inputFile: Pointer to the InputFile structure containing input file data.
  new_subfile: Pointer to the subsystemFile structure containing information about the subsystem.
  t1: Index of the output in the Output_Dec_array.
  returns: The origin value based on the output pattern.
 */
int ChecktheOutput(InputFile* new_inputFile, subsystemFile* new_subfile, int t1) {
    if (contains_pattern(new_inputFile->outputarray->Output_Dec_array[t1].Output, "S%d") == true) {
        return new_subfile->new_out_array->Output_Dec_array[0].origin;
    } else {
        return new_subfile->new_out_array->Output_Dec_array[1].origin - new_subfile->subnetlist->num_components;
    }
    return 0;
}

/*
  This function updates the input IDs of the other Component in the netlistArray.
  new_netlistArray: Pointer to the netlistArray structure containing multiple netlists.
  k: Index of the netlist to be updated in the netlistArray.
 */
void updateTheOthers(netlistArray* new_netlistArray, int k) {
    int l, n, h;

    for (l = 0; l < new_netlistArray->netlists[k].num_components; l++) {
        h = 0;
        for (n = 0; n < new_netlistArray->netlists[k].components[l].numofinputs; n++) {
            if (contains_pattern(new_netlistArray->netlists[k].components[l].inputs[n], "U%d") == true) {
                int d = new_netlistArray->netlists[k].components[l].connections[h];
                sprintf(new_netlistArray->netlists[k].components[l].inputs[n], "U%d", d);
                h++;
    }
    }
    }
}

/*

  Generates the final output file by updating the connections, inputs, and output IDs.
  Writes the resulting netlist data into the output file.
  new_subfile: Pointer to the subsystemFile structure containing information about the subsystem.
  new_inputFile: Pointer to the InputFile structure containing input file data.
  finalFile: Pointer to the OutputFile structure containing final output file data.
  fp: Pointer to the output file.
 */
void makeTheFinalOutputFile(subsystemFile* new_subfile, InputFile* new_inputFile, OutputFile* finalFile, FILE* fp) {
    // Allocate memory for the necessary structures
    OutputArray* new_outarray_TheOutputFile = (OutputArray*)malloc(sizeof(OutputArray));
    netlistArray* new_netlistArray = (netlistArray*)malloc(sizeof(netlistArray));

    int k, j, i, n, l, h;
    i = 0;
    int new_id = 0;
    int Counter = 0;
    char in[MAX_INPUTS][MAX_CHARACTERS];

    // Loop through the subsystem's CDAs and input file components
    for (k = 0; k < new_subfile->CDA->CDA_num; k++) {
        for (j = 0; j < new_inputFile->new_netlist->num_components; j++) {
            if (strcmp(new_subfile->CDA->CDA[k].name, new_inputFile->new_netlist->components[j].type) == 0) {
                netlist* new_netlist = (netlist*)malloc(sizeof(netlist));
                new_netlist = new_subfile->subnetlist;
                add_netlist(new_netlistArray, new_netlist);

                // Copy input values for the current component
                for (n = 0; n < new_inputFile->new_netlist->components[j].numofinputs; n++) {
                    strcpy(in[n], new_inputFile->new_netlist->components[j].inputs[n]);
                }

                // Generate the body of the output file by updating the IDs and connections
                makeTheBodyFortheOutputFile(new_subfile, new_id, &new_netlistArray->netlists[i], in);

                // Update connections for components with input pattern "U%d_"
                for (l = 0; l < new_netlistArray->netlists[i].num_components; l++) {
                    for (h = 0; h < new_netlistArray->netlists[i].components[l].numofinputs; h++) {
                        if (contains_pattern(new_netlistArray->netlists[i].components[l].inputs[h], "U%d_") == true) {
                        Counter = (i - 1) * new_netlistArray->netlists[i].num_components;
                        new_netlistArray->netlists[i].components[l].connections[new_netlistArray->netlists[i].components[l].num_connections] = new_subfile->new_out_array->Output_Dec_array[1].origin + Counter;
                        new_netlistArray->netlists[i].components[l].num_connections++;
                        }
                      }
                    }

                // Update other components in the netlist array
                updateTheOthers(new_netlistArray, i);

                new_id = new_id + new_netlistArray->netlists[i].num_components;
                i++;
            }
        }
    }

    // Allocate memory for the new output declaration structure
    Output_Dec* new_out = (Output_Dec*)malloc(sizeof(Output_Dec));
    k = 0;
    int temp = 0;

    // Process output declarations from the input file
for (i = 0; i < new_inputFile->outputarray->output_num; i++) {
    // Check and update the output origins
    int new_origin = ChecktheOutput(new_inputFile, new_subfile, i);
    temp = k * new_subfile->subnetlist->num_components + new_origin;

    // Add updated output declarations to the output array
    addOutput(new_outarray_TheOutputFile, new_out, new_inputFile->outputarray->Output_Dec_array[i].Output, temp);
    k++;
}

// Assign the processed netlist array and output array to the final output file structure
finalFile->Net_Array = new_netlistArray;
finalFile->OUT_Array = new_outarray_TheOutputFile;

// Write the resulting netlist data into the output file
for (j = 0; j < finalFile->Net_Array->num_netlists; j++) {
    for (i = 0; i < finalFile->Net_Array->netlists[j].num_components; i++) {
        fprintf(fp, "U%d %s ", finalFile->Net_Array->netlists[j].components[i].id, finalFile->Net_Array->netlists[j].components[i].type);
        h = 0;
        for (k = 0; k < finalFile->Net_Array->netlists[j].components[i].numofinputs; k++) {
            if (finalFile->Net_Array->netlists[j].components[i].numofinputs - 1 > h) {
                fprintf(fp, "%s,", finalFile->Net_Array->netlists[j].components[i].inputs[k]);
                h++;
            } else {
                fprintf(fp, "%s ", finalFile->Net_Array->netlists[j].components[i].inputs[k]);
            }
        }
        fprintf(fp, "\n");
    }
}

// Write the output declarations into the output file
for (l = 0; l < finalFile->OUT_Array->output_num; l++) {
    fprintf(fp, "%s = U%d\n", finalFile->OUT_Array->Output_Dec_array[l].Output, finalFile->OUT_Array->Output_Dec_array[l].origin);
}

// Free allocated memory
free(new_netlistArray);
free(new_outarray_TheOutputFile);
free(new_out);

}
int main(){

char Filename[100];
char * subsysemLibrary = "Subsystem_Library.txt";
char * inputFileName="full_adder_netlist";
strcpy(Filename,"final_out.txt");
FILE* fp= fopen(Filename,"w");
if(fp!=NULL){

printf("File is opened successfully\n");
}
ComponentLibrary *library=(ComponentLibrary*)malloc(sizeof(ComponentLibrary*));
library=MakeComponentLibrary();
MakeSubsystemFile(library,subsysemLibrary);

netlist* new_netlist = (netlist*)malloc(sizeof(netlist)); // Allocate memory for the netlist
netlist* new_netlistInput=(netlist*)malloc(sizeof(netlist));
Component_Dec_Array* new_CDA=(Component_Dec_Array*)malloc(sizeof(Component_Dec_Array));
InputFile* new_inputFile=(InputFile*)malloc(sizeof(InputFile));
subsystemFile* new_subfile = (subsystemFile*)malloc(sizeof(subsystemFile));
OutputArray* new_out_array=(OutputArray*)malloc(sizeof(OutputArray));
OutputArray* new_out_array_input=(OutputArray*)malloc(sizeof(OutputArray));
OutputFile* finalFile=(OutputFile*)malloc(sizeof(OutputFile));

new_subfile=StoreTheSubsystemfile(subsysemLibrary,new_netlist,new_CDA,new_out_array);
new_inputFile=readAndStoreInputFile(inputFileName,new_netlistInput,new_out_array_input);
makeTheFinalOutputFile(new_subfile,new_inputFile, finalFile,fp);











    free(new_netlist);
    free(new_CDA);
    free(new_subfile);
    free(new_out_array);


}


