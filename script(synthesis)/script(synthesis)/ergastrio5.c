/* katerina tsimpirdoni 02/04/2023, compiled with:GNU GCC Compiler
this code combine a subsystem netlist ,that made from a Component library ,and a input file netlist into a final output file netlist.
The program reads an input file containing component declarations and a subsystem library file containing sub-circuits.
It then maps the subsystems from the library to the components in the input file and generates a combined netlist.*/
// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


// Define constants
#define MAX_CHARACTERS 35
#define MAX_DEFINITION 15
#define MAX_COMPONENTS 15
#define MAX_CONNECTIONS 15


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
char inputs[MAX_DEFINITION][MAX_CHARACTERS]; // input ports of the component
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
char inputs[MAX_DEFINITION][MAX_CHARACTERS]; // input ports of the component
char Output[MAX_DEFINITION][MAX_CHARACTERS]; // output ports of the component
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
int label; //label about S,C OR SIMPLE OUTPUT

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
OutputArray *new_out_array;


}subsystemFile;
//add struct to store subsystemFile into array
typedef struct subsystemFilesArray{
subsystemFile sub_array[MAX_COMPONENTS];
int sub_array_num;

}subsystemFilesArray;
// This struct represents an input file, containing a netlist and an output array.
typedef struct InputFile{
netlist *new_netlist;
OutputArray *newoutputarray;




}InputFile;
typedef struct InputFile_array{
InputFile inputfile_array[MAX_DEFINITION];
int inputarraynum;



}InputFile_array;
// This struct represents a set of three indexes.
typedef struct {
    int index1;
    int index2;
    int index3;
} Indexs;


// This struct represents an array of netlists with a specified maximum size.
typedef struct {
    netlist netlists[MAX_DEFINITION];
    int num_netlists;
} netlistArray;

// This struct represents an output file, containing a netlist array and an output array.
typedef struct{
netlistArray* Net_Array;
OutputArray *OUT_Array;
}OutputFile;



/* Adds a new netlist to the provided netlist array.
 If the array is full, the function returns without adding the new netlist.*/
int add_netlist(netlistArray *array, netlist* new_netlist,int c1) {
    // Check if the array is full


    // Add the new_netlist to the array and increment the num_netlists counter
    array->netlists[c1] = *new_netlist;
    c1++;
    array->num_netlists=c1;

    // Successfully added the new_netlist
    return c1;
}



/* Function to add an output to the output array.
new_out: pointer to the output to be added.
new_out_array: pointer to the output array where the output will be added.*/
int addOutputToTheArray(Output_Dec* new_out, OutputArray* new_out_array,int c1) {


    if (new_out_array->Output_Dec_array == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for components.\n");
        return-1;
    }

    // Add the component to the netlist
    new_out_array->Output_Dec_array[c1] = *new_out;
    c1++;
    // Increment the number of components in the netlist
    new_out_array->output_num=c1;
    return c1;
}

/*
  Add an InputFile to an InputFile_array.
   Parameters:
    new_inputFile: Pointer to the InputFile to be added.
    new_inputfilearray: Pointer to the InputFile_array.
    c1: Current count of InputFiles in the array.
    Returns:
   Updated count of InputFiles in the array.
 */
int add_inputfilearray(InputFile *new_inputFile,InputFile_array*new_inputfilearray,int c1){
new_inputfilearray->inputfile_array[c1]=*new_inputFile;



c1++;
new_inputfilearray->inputarraynum=c1;
return c1;




}
/*
  Add an InputFile to a netlist, output array, and InputFile_array.
 Parameters:
    new_inputFile: Pointer to the InputFile to be added.
    new_netlist: Pointer to the netlist.
    newoutputarray: Pointer to the OutputArray.
    new_input_array: Pointer to the InputFile_array.
    c1: Current count of InputFiles in the array.
 Returns:
 Updated count of InputFiles in the array.
 */
 int add_inputfile(InputFile *new_inputFile,netlist *new_netlist,OutputArray *newoutputarray,InputFile_array* new_input_array,int c1){

 new_inputFile->newoutputarray=newoutputarray;
 new_inputFile->new_netlist=new_netlist;

c1=add_inputfilearray(new_inputFile,new_input_array,c1);

 return c1;





 }
/* Function to add an output to the output array with origin information.
new_out_array: pointer to the output array where the output will be added.
new_out: pointer to the output to be added.
Output: name of the output.
origin: origin information for the output. */
int addOutput(OutputArray* new_out_array, Output_Dec* new_out, char *Output, int origin ,int label,int c1) {
    // Allocate memory for new_out->Output
    new_out->Output = (char *)malloc((strlen(Output) + 1) * sizeof(char));

    strcpy(new_out->Output, Output);
    new_out->origin = origin;
    new_out->label=label;


    c1=addOutputToTheArray(new_out,new_out_array,c1);
    return c1;
}


// Function to add a component to a netlist
// c: pointer to the component to be added
// netlist: pointer to the netlist where the component will be added
int addComponentToTheNetlist(Component *c, netlist *netlist ,int c1) {


    if (netlist->components == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for components.\n");
        return -1;
    }

    // Add the component to the netlist
    netlist->components[c1] = *c;
    c1++;
    // Increment the number of components in the netlist
    netlist->num_components=c1;
    return c1;
}
/* Function to add a component with its properties to a netlist
   netlist: pointer to the netlist where the component will be added
   new_component: pointer to the component to be added
   type: type of the component
   inputs: array of input port names
   id: ID of the component
   connections: array of connection indices*/
int add_component(netlist* netlist,Component* new_component, char* type, char inputs[MAX_DEFINITION][MAX_CHARACTERS],int id,int connections[MAX_CONNECTIONS],int c1) {

    new_component->id = id;// set the ID for the new component

    strcpy(new_component->type, type);                                       // set the type of the new component
    for (int i = 0; i < new_component->numofinputs; i++) {
        strcpy(new_component->inputs[i], inputs[i]);                         // set the input ports of the new component
    }
    for (int i = 0; i < new_component->num_connections; i++) {
     new_component->connections[i]=connections[i];

    }


   c1= addComponentToTheNetlist(new_component,netlist,c1);
   return c1;
}
/* Function to add a component to the component declaration array
  c: pointer to the component to be added
  CDarray: pointer to the component declaration array where the component will be added*/
int addComponentToTheDeclarationArray(Component_Dec *c, Component_Dec_Array *CDarray,int c1) {




    if (CDarray->CDA == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for components.\n");
        return -1;
    }

    // Add the component to the component declaration array
    CDarray->CDA[c1] = *c;
    c1++;
    // Increment the number of components in the component declaration array
    CDarray->CDA_num=c1;
    return c1;
}
/* Function to add a component declaration with its properties to a component declaration array
   Component_Dec_Array: pointer to the array where the component declaration will be added
   newComponent_Dec: pointer to the component declaration to be added
   name: name of the component
    inputs: array of input port names
    outputs: array of output port names*/
int add_component_Dec(Component_Dec_Array *Component_Dec_Array,char name[MAX_CHARACTERS],char inputs[MAX_DEFINITION][MAX_CHARACTERS],char outputs[MAX_DEFINITION][MAX_CHARACTERS],int num_inputs,int num_outputs,int c1){
Component_Dec* newComponent_Dec=(Component_Dec*)malloc(sizeof(Component_Dec));
strcpy(newComponent_Dec->name, name);//Set the name of the new component declaration
newComponent_Dec->numofinputs=num_inputs;
newComponent_Dec->numofoutputs=num_outputs;

for (int i = 0; i < newComponent_Dec->numofinputs; i++) {
        strcpy(newComponent_Dec->inputs[i], inputs[i]);   // set the input ports of the new component
    }
// Set the output ports of the new component declaration
 for(int i=0;i<newComponent_Dec->numofoutputs;i++){

     strcpy(newComponent_Dec->Output[i], outputs[i]);


 }
// Add the component declaration to the component declaration array
 c1=addComponentToTheDeclarationArray(newComponent_Dec,Component_Dec_Array,c1);


return c1;





}
int add_subfile_array(subsystemFile* new_subfile,subsystemFilesArray*new_sub_array,int c1){
new_sub_array->sub_array[c1]=*new_subfile;


c1++;
new_sub_array->sub_array_num=c1;
return c1;




}
int add_subfile(Component_Dec_Array *CDA,netlist *subnetlist ,OutputArray* new_out_array, subsystemFilesArray*new_sub_array,int c1){
subsystemFile* new_subfile = (subsystemFile*)malloc(sizeof(subsystemFile));
new_subfile->CDA=CDA;
new_subfile->new_out_array=new_out_array;
new_subfile->subnetlist=subnetlist;

c1=add_subfile_array(new_subfile,new_sub_array,c1);



return c1;

}







//-----function for create Component and subsystem Libraries------------------------------------------------------------------------


/* This function reads a component library from a file and returns a pointer to the ComponentLibrary structure.
The ComponentLibrary structure contains an array of Component_S structures that represent each component in the library.
The file format is assumed to follow the format specified in the assignment instructions.
This function allocates memory for the ComponentLibrary and its components, which must be freed later by the caller.
The function reads the file line by line, removing any comments that start with "%%".
It parses the lines to extract the component definitions, which are assumed to start with "** COMPONENT LIBRARY" and end with a semicolon.
The component definitions contain a component name and a list of input ports, which are extracted and stored in the Component_S structures.
Parameters:
filename: the name of the file to read the component library from
Returns:
A pointer to the ComponentLibrary structure if the library was successfully read from the file, NULL otherwise.
*/
ComponentLibrary* read_component_library(const char* filename) {
// Initialize variables
ComponentLibrary* lib = NULL; // Pointer to the ComponentLibrary structure
FILE* file = fopen(filename, "r"); // Pointer to the file to read from
if (file == NULL) { // Check if file could be opened
printf("Error: could not open file '%s'\n", filename);
return NULL;
}

char line[100]; // Buffer to store each line read from the file


while (fgets(line, 100, file)) { // Read each line from the file
    // Remove comments
    char* comment_start = strstr(line, "%%"); // Find the start of any comments in the line
    if (comment_start != NULL) {
        *comment_start = '\0'; // Null-terminate the line at the start of the comment
    }

    // Skip leading whitespace
    char* name_start = line; // Pointer to the start of the component definition
    while (*name_start == ' ' || *name_start == '\t') { // Skip any leading whitespace
        name_start++;
    }

    // Check if line starts with "** COMPONENT LIBRARY"
    if (strncmp(name_start, "** COMPONENT LIBRARY", strlen("** COMPONENT LIBRARY")) == 0) { // Check if the line contains the start of a component library definition
        if (lib != NULL) { // Check if a component library has already been defined in the file
            printf("Error: found multiple component libraries in file\n");
            fclose(file);
            return NULL;
        }
        lib = malloc(sizeof(ComponentLibrary)); // Allocate memory for the ComponentLibrary structure
        lib->components = NULL; // Initialize the components array to NULL
        lib->component_count = 0; // Initialize the component count to 0
    } else if (lib != NULL) { // If a component library has been defined, parse the component definition
        // Parse component definition
        char* name_end = strstr(name_start, " ;"); // Find the end of the component name
        if (name_end == NULL) { // Check if the component definition is malformed
            printf("Error: malformed component definition\n");
            fclose(file);
            free(lib);
            return NULL;
        }
        *name_end = '\0'; // Null-terminate the component name

// Use strtok function to extract the component name from the current line
strtok(name_start, " ");
char* component_name = strtok(NULL, " ");

// Check if the component name is NULL, which indicates a malformed component definition
if (component_name == NULL) {
printf("Error: malformed component definition\n");
fclose(file);
free(lib);
return NULL;
}

// Allocate memory for the new component in the component library
lib->components = realloc(lib->components, (lib->component_count + 1) * sizeof(Component_S));
Component_S* component = &lib->components[lib->component_count];

// Copy the component name to the new component
component->name = strdup(component_name);

// Initialize the input count and inputs for the new component
component->input_count = 0;
component->inputs = NULL;

// Parse input ports from the current line
char* inputs_start = strstr(name_end + 2, "IN:");
if (inputs_start != NULL) {
inputs_start += strlen("IN:");
char* input_end = inputs_start;
       // Loop through all input ports
while (*input_end != '\0') {
    // If a comma is found, the current input port name has ended
    if (*input_end == ',') {
        *input_end = '\0';

        // Increase the input count for the current component and allocate memory for the new input port
        component->input_count++;
        component->inputs = realloc(component->inputs, component->input_count * sizeof(char*));

        // Copy the input port name to the new input port
        component->inputs[component->input_count - 1] = strdup(inputs_start);

        // Move to the next input port name
        inputs_start = input_end + 1;
    }
    input_end++;
}

// Allocate memory for the final input port (or the only input port if there is only one)
component->input_count++;
component->inputs = realloc(component->inputs, component->input_count * sizeof(char*));

// Copy the final input port name to the new input port
component->inputs[component->input_count - 1] = strdup(inputs_start);
    }

    lib->component_count++;
}
    }

    fclose(file);
    return lib;
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
int checkIfExist(ComponentLibrary* lib,char * filename){
    int flag=0;
 FILE* file = fopen(filename, "r");
    if (file == NULL) { // Check if file could be opened
        printf("Error: could not open file '%s'\n", filename);
        return-1;
    }

    char line[100]; // Buffer to store each line read from the file
    while (fgets(line, 100, file)) { // Read each line from the file
        // Remove comments
        char* comment_start = strstr(line, "%%"); // Find the start of any comments in the line
        if (comment_start != NULL) {
            *comment_start = '\0'; // Null-terminate the line at the start of the comment
        }else{
        if((strstr(line,"**")!=NULL)||(strstr(line,"COMP")!=NULL)||(strstr(line,"BEGIN")!=NULL)||(strstr(line,"=")!=NULL)||(strstr(line,"END")!=NULL) ||(strlen(line) == 0) || (line[0] == '\n')){
        //do nothing
        }else{
        // Look for component names in the line
        for (int i = 0; i < lib->component_count; i++) {
            Component_S* component = &lib->components[i];
            char* component_name = component->name;
            char* component_def = strstr(line, component_name);
            if (component_def != NULL) {
            flag=0;
              break;
            }else{
           flag=1;
            }
        }
     if(flag==1){
        printf("this component in this line %s does not exist\n",line);


     }
    }
    }
    }
    fclose(file);
    return flag;
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
/* Function to read and parse an output declaration from a given string
   line: input string containing the output declaration
   new_out_array: pointer to an output array where the parsed output declaration will be stored*/
int ReadTheOutput(char line[256], OutputArray* new_out_array,subsystemFilesArray* new_subfile_array,netlist* new_netlistInput, int c1) {
    Output_Dec* new_out = (Output_Dec*)malloc(sizeof(Output_Dec));
    int i, k,j,n;
    int origin;

    char *temp1 = NULL;
    char *token1 = "=";
    char *Output;
    int label=0;

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
    if(new_subfile_array==NULL && new_netlistInput==NULL){
       label=-1;
      

    }else{
    
   for(int m=0;m<new_subfile_array->sub_array_num;m++){
    for(j=0;j<new_subfile_array->sub_array[m].CDA->CDA_num;j++){
    
    for(n=0;n<new_subfile_array->sub_array[m].CDA->CDA[j].numofoutputs;n++){
     if(strstr(new_subfile_array->sub_array[m].CDA->CDA[j].name,new_netlistInput->components[origin].type)!=NULL){
   
     

    char *found_ptr = strstr(temp1,new_subfile_array->sub_array[m].CDA->CDA[j].Output[n]);

    if (found_ptr != NULL) {

    label=n;
    

    break;

   }
     }
       }
          }
              }
                  }



    c1=addOutput(new_out_array, new_out, Output, origin,label,c1); // add the new output to the output array
free(Output);
}




return c1;

}
/* Function to read and parse a netlist body line from a given string
line: input string containing the netlist body line
netlist: pointer to the netlist where the parsed component will be stored*/
int readThebody(char line[256],netlist* netlist,int c1){
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
char inputs[MAX_DEFINITION][MAX_CHARACTERS] = {0};
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
   c1= add_component(netlist, new_component, type, inputs, id, connections,c1);
   free(new_component);
}

// Free the allocated memory for the new component and type

return c1;


}
/* Function to read and parse a component declaration
 line: input string containing the component declaration
 Component_Dec_Array: pointer to an array where the parsed component declaration will be stored*/
int readTheComponent(char line[256],Component_Dec_Array *Component_Dec_Array,int c1){




    char *token1=";";
    char*temp1;
     int i;

    char temp[3][256];
char name[MAX_CHARACTERS] = {0};
char inputs[MAX_DEFINITION][MAX_CHARACTERS] = {0};
char outputs[MAX_DEFINITION][MAX_CHARACTERS] = {0};

// cut the input string using ';' as the delimiter
temp1 = strtok(line, token1);

// Check if the tokenized string contains "COMP" and extract the component name
if (strstr(temp1, "COMP") != NULL) {
    sscanf(temp1, "COMP %s", name);


} else {
   
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
int numofinputs = CutAndStore(inputs, temp, 0, "IN:");
int numofoutputs = CutAndStore(outputs, temp, 1, "OUT");

// Add the parsed component declaration to the Component_Dec_Array
c1=add_component_Dec(Component_Dec_Array, name, inputs, outputs,numofinputs,numofoutputs,c1);
return c1;


}
 /*
 Read and store the contents of an input file.
  Parameters:
   inputFileName: The name of the input file to be read.
   new_subfile_array: Pointer to the subsystemFilesArray.
   new_input_array: Pointer to the InputFile_array.
 */
void readAndStoreInputFile(char * inputFileName,subsystemFilesArray* new_subfile_array,InputFile_array* new_input_array) {
    char line[256];
    int c1=0;
    int c2=0;
    int c3=0;
    netlist* new_netlistInput=NULL;
    OutputArray* new_out_array_input=NULL;
    int read_comp=0;
    int read_out=0;
    InputFile* new_inputFile=(InputFile*)malloc(sizeof(InputFile));
    FILE *fptr = fopen(inputFileName, "r");
    // Check if the file is opened successfully
    if (fptr == NULL) {
        printf("Error opening SubSystem file.");
        return ;
    }

    // Read the file line by line
    while (fgets(line, sizeof(line), fptr)) {




        // Check if the line contains an '=' sign
        if (strstr(line, "=") == NULL) {
            if (line != NULL) {
                if(read_comp==0){
                 new_netlistInput=(netlist*)malloc(sizeof(netlist));
                }
                 read_comp=1;
                // Process the line and store it in the netlist
               c1= readThebody(line, new_netlistInput,c1);
               
            }
        } else {
            // Check if the line contains 'END'
            if (strstr(line, "END") == NULL) {
                // Process the line and store it in the output array
                if(read_out==0){
                new_out_array_input=(OutputArray*)malloc(sizeof(OutputArray));

                }
                 read_out=1;
                c2=ReadTheOutput(line, new_out_array_input,new_subfile_array,new_netlistInput,c2);
                

            } }

    }

    // Store the netlist and output array in the InputFile structure
   c3=add_inputfile(new_inputFile,new_netlistInput,new_out_array_input,new_input_array,c3);
   read_out=0;
   read_comp=0;
   // free(new_inputFile);
    //free(new_netlistInput);
    //free(new_out_array_input);
    fclose(fptr);
    return ;
}
/*
  Store the contents of a subsystem file.
  Parameters:
    subsysemLibrary: The name of the subsystem file to be read.
   new_subfile_array: Pointer to the subsystemFilesArray.
 */
void StoreTheSubsystemfile(char * subsysemLibrary,subsystemFilesArray* new_subfile_array ){
char line[256];
int c1=0;
int c2=0;
int c3=0;
int c4=0;
int readcomp_declaration=0;
int readcomponets=0;
netlist* new_netlist = NULL;
Component_Dec_Array* new_CDA = NULL;
OutputArray* new_out_array = NULL;

FILE *fptr = fopen(subsysemLibrary, "r");
    if (fptr == NULL) {
        printf("Error opening SubSystem file.");
        return;
    }
   // Read the file line by line
    while (fgets(line, sizeof(line), fptr)) {
if ((strncmp(line, "%%", 2) == 0)||strncmp(line, "**", 2) == 0) {
            continue; // ignore lines starting with '%%' or ''
        }


    if (strstr(line, "COMP") != NULL && strstr(line, "IN") != NULL && strstr(line, "OUT") != NULL) {
        readcomp_declaration=1;
        new_CDA=(Component_Dec_Array*)malloc(sizeof(Component_Dec_Array));
        c4=readTheComponent(line, new_CDA,c4);
        c4=0;

    } else {
        if((strlen(line) != 0) && (line[0] != '\n')){
        if (strstr(line, "END") == NULL) {
        if (strstr(line, "BEGIN") == NULL) {
            if (strstr(line, "=") == NULL) {
                if (line != NULL) {
                if(readcomp_declaration==1){
                new_netlist=(netlist*)malloc(sizeof(netlist));
                }
                readcomp_declaration=0;
                    // Parse the component instances
                    c2=readThebody(line, new_netlist,c2);
                    readcomponets=1;
                }
            } else {
                if (strstr(line, "END") == NULL) {
                    // Parse output declarations
                    if(readcomponets==1){
                    new_out_array=(OutputArray*)malloc(sizeof(OutputArray));
                    }
                    readcomponets=0;
                   c3= ReadTheOutput(line, new_out_array,NULL,NULL,c3);
                }
                }
             }
        }else{

    c1=add_subfile(new_CDA,new_netlist,new_out_array,new_subfile_array,c1);
    c2=0;
    c3=0;
    

        }
      }
    }
  }




fclose(fptr);

return;


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
            idx->index1 = k1; //place of input in subsystem file;
            idx->index2 = i1; //component;
            idx->index3 = j; //place of input in input file
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
void MakeTheFinalInput(subsystemFile *subsystem, netlist *new_netlistFinalIn, char inputs[MAX_DEFINITION][MAX_CHARACTERS]) {
    Indexs *idx;
    Indexs temp1[MAX_DEFINITION];
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
void makeTheBodyFortheOutputFile(subsystemFile* new_subfile, int new_id, netlist *new_netlistFinalIny, char in[MAX_DEFINITION][MAX_CHARACTERS]) {
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
   
    if (new_inputFile->newoutputarray->Output_Dec_array[t1].label==0) {
        return new_subfile->new_out_array->Output_Dec_array[0].origin; //s
    } else {
        return new_subfile->new_out_array->Output_Dec_array[1].origin - new_subfile->subnetlist->num_components;//cout
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
    int c1=0;
    int c2=0;
    int new_id = 0;
    int Counter = 0;
    char in[MAX_DEFINITION][MAX_CHARACTERS];

    // Loop through the subsystem's CDAs and input file components
    for (k = 0; k < new_subfile->CDA->CDA_num; k++) {
        for (j = 0; j < new_inputFile->new_netlist->num_components; j++) {
            if (strcmp(new_subfile->CDA->CDA[k].name, new_inputFile->new_netlist->components[j].type) == 0) {
                netlist* new_netlist = (netlist*)malloc(sizeof(netlist));
                new_netlist = new_subfile->subnetlist; // num_components  that exists in the inputfile copies of  the body of subnetlist without output
               c1= add_netlist(new_netlistArray, new_netlist,c1); // add to a new netlist_array for final output

                // Copy input values for the current component
                for (n = 0; n < new_inputFile->new_netlist->components[j].numofinputs; n++) {
                    strcpy(in[n], new_inputFile->new_netlist->components[j].inputs[n]);//take the inputs of inputfile
                   
                }

                // Generate the body of the output file by updating the IDs and connections and the inputs that contains in the input file(A0,B0,CIN/A1,B1,U0_C)
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
                //make the new id
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
for (i = 0; i < new_inputFile->newoutputarray->output_num; i++) {
if (strcmp(new_subfile->CDA->CDA[0].name, new_inputFile->new_netlist->components[new_inputFile->newoutputarray->Output_Dec_array[i].origin].type) == 0 ) {
    // Check and update the output origins
    int new_origin = ChecktheOutput(new_inputFile, new_subfile, i);
    temp = k * new_subfile->subnetlist->num_components + new_origin;
     
     int label =-1;
    // Add updated output declarations to the output array
   c2= addOutput(new_outarray_TheOutputFile, new_out, new_inputFile->newoutputarray->Output_Dec_array[i].Output, temp,label,c2);
    k++;
}
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

int main()
{

int doesnot_exist=0;

char * subsysemLibrary = "Subsystem_Library.txt";
ComponentLibrary *library=(ComponentLibrary*)malloc(sizeof(ComponentLibrary*));
library = read_component_library("component_library.txt");
doesnot_exist=checkIfExist(library,subsysemLibrary);
if(doesnot_exist==1){
printf("Cant read and cut the Subsystem file because a Component does not exist in the Component Library");
} else{
}

char * inputFileName="output.txt";
char Filename[100];
strcpy(Filename,"final_out.txt");



subsystemFilesArray* new_subfile_array = (subsystemFilesArray*)malloc(sizeof(subsystemFilesArray));
//subsystemFilesArray* new_subfile_array1 = (subsystemFilesArray*)malloc(sizeof(subsystemFilesArray));
InputFile_array* new_input_array=(InputFile_array*)malloc(sizeof(InputFile_array));
OutputFile*new_out_array=(OutputFile*)malloc(sizeof(OutputFile));

StoreTheSubsystemfile(subsysemLibrary,new_subfile_array);

readAndStoreInputFile(inputFileName,new_subfile_array,new_input_array);

FILE* fp= fopen(Filename,"w");

if(fp!=NULL){

printf("File is opened successfully\n");
}
for(int i=0;i<new_input_array->inputarraynum;i++){
   for(int j=0;j<new_subfile_array->sub_array_num;j++){

    makeTheFinalOutputFile(&new_subfile_array->sub_array[j],&new_input_array->inputfile_array[i],new_out_array,fp);
   }
}




free(new_input_array);
return 1;
}
