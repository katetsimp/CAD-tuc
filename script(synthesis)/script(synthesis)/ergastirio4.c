/* katerina tsimpirdoni 27/04/2023, compiled with:GNU GCC Compiler
The code is a C program that reads an input file containing information about entities in a design,
parses the information and stores it in an Entity_list structure.
The makeOutputFile function generates the netlist data structure from the Entity_list structure.*/


// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define constants
#define MAX_CHARACTERS 25
#define MAX_DEFINITION 8
#define MAX_COMPONENTS 8
#define MAX_CONNECTIONS 5

// Define the Iodeclaretion structure
typedef struct{
    char name[MAX_CHARACTERS]; // name of INPUTS/OUTPUTS
    char label[MAX_CHARACTERS]; // IN or OUT
    char IOnames[MAX_DEFINITION][MAX_CHARACTERS]; //the name of values of INPUTS/OUTPUTS
    int numofIOnames; //bits of input/outputs
} Iodeclaretion;

// Define the Port structure
typedef struct{
    char name[MAX_CHARACTERS];
    Iodeclaretion IO_array[MAX_DEFINITION]; //an array of Iodeclaretion
    int ionum; // total num of inputs/outputs
    int numofinputs; //total num of inputs
    int numofoutputs; //total num of outputs
} Port;

// Define the variable structure
typedef struct{
    char name[MAX_CHARACTERS];
    int value;
} variable;

// Define the variables_array structure
typedef struct{
    variable ArrayOfVariables[MAX_DEFINITION];
    int NumOfvariables;
} variables_array;

// Define the Entity structure
typedef struct{
    char name[MAX_CHARACTERS];
    variables_array* variables_dec;
    Port*PortMap;
} Entity;

// Define the Entity_list structure
typedef struct{
    Entity entity_array[MAX_DEFINITION];
    int numofEntity;
} Entity_list;

// Define the Component structure
typedef struct Component {
    int id; // unique identifier for each component
    char type[MAX_CHARACTERS]; // type of component
    char inputs[MAX_DEFINITION][MAX_CHARACTERS]; // input ports of the component
    int label;
    int numofinputs; // number of input ports
} Component;

// Define the Component_array structure
typedef struct {
    Component components[MAX_COMPONENTS]; // array of components
    int num_components; // number of components in the netlist
} Component_array;

// Define the Output_Dec structure
typedef struct Output_Dec{
    char *Output; // output port name
    int origin; // index of the originating component
    char *label; //label about S,C OR SIMPLE OUTPUT
} Output_Dec;

// Define the OutputArray structure
typedef struct OutputArray{
    Output_Dec Output_Dec_array[MAX_COMPONENTS]; // array of output declarations
    int output_num; // number of outputs in the array
} OutputArray;

// Define the InputFile structure
typedef struct InputFile{
    Component_array *new_Component_array;
    OutputArray* outputarray;

} netlist;
typedef struct arrayofnetlist{
    netlist different_netlist[MAX_COMPONENTS];
   int numofnetlist;

} arrayofnetlist;

// Function prototypes
int addIO(char nameofcomp[MAX_CHARACTERS],Port *newPort, char name[MAX_CHARACTERS], char label[MAX_CHARACTERS], char IOnames[MAX_DEFINITION][MAX_CHARACTERS], int numofIOnames, int i, int c1);
int makePort(char name[MAX_CHARACTERS],Iodeclaretion *new_iodec, int c1, Port *newPort);
int addVaraible(variables_array *newVarArray, char name[MAX_CHARACTERS], int value, int c2);
int addvartolist(variables_array *newVarArray, variable *new_var, int c2);
int MakeEnity(variables_array *variables_dec, Port *PortMap, Entity_list *new_entity_list, int c3,char name[MAX_CHARACTERS]);
int MakelistofEntities(int c3, Entity *new_entity, Entity_list *new_entity_list);
/* Function to add a component to a netlist
c: pointer to the component to be added
 netlist: pointer to the netlist where the component will be added*/
void addComponentToArray(Component *c, Component_array* new_component_arr) {

    int i = new_component_arr->num_components;

    if(new_component_arr->num_components<0){
            i=0;
     new_component_arr->num_components++;

    }

    if (new_component_arr->components == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for components.\n");
        return;
    }

    // Add the component to the netlist
    new_component_arr->components[i] = *c;

    // Increment the number of components in the netlist
    new_component_arr->num_components++;

}
int addnetlisttothearray(arrayofnetlist*new_array,netlist* new_net,int c1){
new_array->different_netlist[c1]=*new_net;

c1++;
new_array->numofnetlist=c1;
return c1;


}
int addnetlist(arrayofnetlist*new_array, Component_array *new_Component_array ,OutputArray* outputarray,int c1){

    // Allocate memory for new_netlist
    netlist* new_net= (netlist*)malloc(sizeof(netlist));
    new_net->new_Component_array=new_Component_array;
    new_net->outputarray=outputarray;
    c1=addnetlisttothearray(new_array,new_net,c1);

    return c1;







}


/*
Adds a component to the component array.
c: pointer to the component to be added
new_component_arr: pointer to the component array
                                                     */

void add_component(Component_array* new_component_arr,Component* new_component, char* type, char inputs[MAX_DEFINITION][MAX_CHARACTERS],int id,int label) {

    new_component->id = id;// set the ID for the new component

    strcpy(new_component->type, type); // set the type of the new component
     new_component->label=label;
    for (int i = 0; i < new_component->numofinputs; i++) {
        strcpy(new_component->inputs[i], inputs[i]);        // set the input ports of the new component
       
    }




    addComponentToArray(new_component,new_component_arr);
}
/* Function to add an output to the output array.
new_out: pointer to the output to be added.
new_out_array: pointer to the output array where the output will be added.*/
void addOutputToTheArray(Output_Dec* new_out, OutputArray* new_out_array) {
    int i = new_out_array->output_num;

    if( new_out_array->output_num<0){
            i=0;

     new_out_array->output_num++;
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
origin: origin information for the output.
label:put the name of the output that has origin */
void addOutput(OutputArray* new_out_array, Output_Dec* new_out, char *Output, int origin ,char *label) {
    // Allocate memory for new_out->Output
    new_out->Output = (char *)malloc((strlen(Output) + 1) * sizeof(char));
    new_out->label=(char *)malloc((strlen(label) + 1) * sizeof(char));
    strcpy(new_out->label,label);
    strcpy(new_out->Output, Output);
    new_out->origin = origin;
    new_out->label=label;


    addOutputToTheArray(new_out,new_out_array);
}









/*
 * Adds an I/O declaration to the given port with the
 * specified name, label, and I/O names.
 * Returns the updated number of I/O declarations.
 */
int addIO(char nameofcomp[MAX_CHARACTERS],Port *newPort,char name[MAX_CHARACTERS], char label[MAX_CHARACTERS],char IOnames[MAX_DEFINITION][MAX_CHARACTERS],int numofIOnames,int i,int c1){
    int c=0;
    // Allocate memory for a new I/O declaration
    Iodeclaretion* new_iodec = (Iodeclaretion*)malloc(sizeof(Iodeclaretion));
    strcpy(new_iodec->name,name);
    strcpy(new_iodec->label,label);
    new_iodec->numofIOnames=i;
    // Copy the IOnames to the new I/O declaration
    while(c<i){
        strcpy(new_iodec->IOnames[c],IOnames[c]);
        c++;
    }

    // Call the makePort function to create a new port
    c1=makePort(nameofcomp,new_iodec,c1,newPort);

    return c1;
}

/*
 * Creates a new port from the given I/O declaration,
 * updating the input/output count and I/O number.
 * Returns the updated number of I/O declarations.
 */
int makePort(char name[MAX_CHARACTERS],Iodeclaretion* new_iodec,int c1,Port*newPort){
    // Add the new I/O declaration to the port
    newPort->IO_array[c1]=*new_iodec;
    strcpy(newPort->name,name);
    // Increment the input/output count based on the label
    if(strcmp(newPort->IO_array[c1].label,"IN")==0){
        newPort->numofinputs++;
    }else{
        newPort->numofoutputs++;
    }

    // Increment the I/O number
    c1++;
    newPort->ionum=c1;
    return c1;
}

/*
 * Adds a variable to the given variables array with
 * the specified name and value.
 * Returns the updated number of variables.
 */
int addVaraible(variables_array* newVarArray,char name[MAX_CHARACTERS],int value,int c2){
    // Allocate memory for a new variable
    variable* new_var = (variable*)malloc(sizeof(variable));
    strcpy(new_var->name,name);
    new_var->value=value;

    // Call the addvartolist function to add the variable to the list
    c2=addvartolist(newVarArray,new_var,c2);

    return c2;
}
 // Function to add a variable to the variables_array
/*
 * newVarArray: pointer to the variables_array
 * new_var: pointer to the variable to be added
 * c2: current number of variables in the array
 */
int addvartolist(variables_array* newVarArray,variable* new_var,int c2){
    // Add the variable to the array
    newVarArray->ArrayOfVariables[c2]=*new_var;
    // Increment the counter
    c2++;
    // Update the number of variables
    newVarArray->NumOfvariables=c2;
    // Return the updated counter
    return c2;
}

// Function to create a new entity and add it to the Entity_list
/*
 * variables_dec: pointer to the variables_array for the new entity
 * PortMap: pointer to the Port structure for the new entity
 * new_entity_list: pointer to the Entity_list
 * c3: current number of entities in the list
 * name: name of the new entity
 */
int MakeEnity(variables_array* variables_dec,Port*PortMap, Entity_list* new_entity_list,int c3,char name[MAX_CHARACTERS]){
    // Allocate memory for the new entity
    Entity* new_entity = (Entity*)malloc(sizeof(Entity));
    // Set the name of the new entity
    strcpy(new_entity->name,name);
    // Assign the variables_dec and PortMap pointers to the new entity
    new_entity->variables_dec=variables_dec;
    new_entity->PortMap=PortMap;

    // Add the new entity to the list of entities
    c3=MakelistofEntities(c3,new_entity,new_entity_list);
    // Return the updated counter
    return c3;
}

// Function to add an entity to the list of entities
/*
 * c3: current number of entities in the list
 * new_entity: pointer to the Entity to be added
 * new_entity_list: pointer to the Entity_list
 */
int MakelistofEntities(int c3,Entity* new_entity, Entity_list* new_entity_list){
    // Add the entity to the array
    new_entity_list->entity_array[c3]=*new_entity;
    // Increment the counter
    c3++;
    // Update the number of entities
    new_entity_list->numofEntity=c3;
    // Return the updated counter
    return c3;
}

/*

The ReadInputFile function reads an input file and store an entity list in structs
with the data from the file. The input file should contain ENTITY, VAR, and PORT
definitions.
parameters:
Filename: The name of the input file to read.
entity_list: The pointer to the entity list where the data will be stored.
return:
The updated entity list with the data from the input file.
The input file format is as follows:
ENTITY <entity_name>
VAR <variable_name> = <value>
PORT <port_label> <port_name>: {<io_name1>, <io_name2>, ...}
END
*/
Entity_list* ReadInputFile(char * Filename ,Entity_list* entity_list){
 FILE* fp= fopen(Filename,"r");
 char line[256];
 char *comment_start;





if(fp!=NULL){

printf("File is opened successfully\n");
}
    int reading_entity = 0;

    int reading_port = 0;
   char comp_name[MAX_CHARACTERS]={0};

    entity_list->numofEntity = 0;

    Entity *current_entity =(Entity*)malloc(sizeof(Entity));
    variables_array *current_var_array = NULL;
    Port *current_port = NULL;
    int c1;
    int c2;
    int c3 = 0;
 while (fgets(line, sizeof(line), fp)) {
comment_start = strstr(line, "%%");
        if (comment_start != NULL) {
            *comment_start = '\0'; // Terminate the string before the comment
        }

        // Remove newline character if it's present
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

  if (strstr(line, "ENTITY") != NULL) {
    // If the current line contains the string "ENTITY", set the reading_entity flag to 1 and extract the name of the entity
    // from the line using sscanf.
    reading_entity = 1;
    char entity_name[MAX_CHARACTERS];

    sscanf(line, "ENTITY %s", entity_name);
      c1 = 0;
      c2 = 0;
    // Dynamically allocate memory for a new variables_array and Port struct for the current entity, and create the entity
    // using the MakeEntity function. Also set the current_entity pointer to point to the newly created entity.
    current_var_array = (variables_array *)malloc(sizeof(variables_array));
    current_var_array->NumOfvariables = 0;
    current_port = (Port *)malloc(sizeof(Port));
    current_port->ionum = 0;
    current_port->numofinputs = 0;
    current_port->numofoutputs = 0;

    c3 = MakeEnity(current_var_array, current_port, entity_list, c3, entity_name);
    current_entity = &entity_list->entity_array[c3 - 1];
    current_entity->variables_dec=(variables_array *)malloc(sizeof(variables_array));
    current_entity->PortMap=(Port *)malloc(sizeof(Port));
        } else if (strstr(line, "VAR") != NULL && reading_entity) {
    // If the current line contains the string "VAR" and the reading_entity flag is set, set the reading_var flag to 1 and
    // extract the variable name and value. Then add the variable to the current entity's
    // variables_array using the addVariable function.

    char var_name[MAX_CHARACTERS];
    int var_value;
    sscanf(line, "VAR %s = %d", var_name, &var_value);
    c2 = addVaraible(current_entity->variables_dec, var_name, var_value, c2);
}else if(strstr(line, "LIB")!= NULL){
 sscanf(line, "LIB %s",comp_name);


} else if (strstr(line, "PORT") != NULL && reading_entity) {
    // If the current line contains the string "PORT" and the reading_entity flag is set, set the reading_port flag to 1 and

    reading_port = 1;

    if(strstr(line, ":") != NULL ){
        char port_label[MAX_CHARACTERS];
        char port_name[MAX_CHARACTERS];
        char io_names[MAX_DEFINITION][MAX_CHARACTERS];
        int num_io_names = 0;
    // Use strtok to extract the port label and name from the line, and extract the I/O names using sscanf and strtok.
        char *token1= strtok(line, ":");
        char* token2=strtok(NULL,":");
        token1=strtok(token1,"(");
        token1= strtok(NULL,"(");
        sscanf(token1, "%s %s", port_label, port_name);

        char *token = strtok(token2, "{,");  // split the line using "{" and "," as delimiters

    // Check if the word contains the character "}"
   if(strstr(token,"}")!=NULL){
    // If it does, then slit it again using "}" as a delimiter
    token=strtok(token,"}");
    // Store the word in the io_names array
    sscanf(token, " %s", io_names[num_io_names]);
        }
            sscanf(token, " %s", io_names[num_io_names]);
            while (token != NULL) {
    token = strtok(NULL, ",");
    if (token != NULL) {
        // Check if the word contains the character "}"
        if(strstr(token,"}")!=NULL){
            // If it does, then split it again using "}" as a delimiter
            token=(strtok(token, "}"));
        }
        // Increase the number of io names
        num_io_names++;
        // Store the word in the io_names array
        sscanf(token, " %s", io_names[num_io_names]);

    }
}

// Call the addIO function to add the input/output information to the entity's PortMap
c1 = addIO(comp_name,current_entity->PortMap, port_name, port_label, io_names, num_io_names, num_io_names, c1);




            }

        // Repeat the same process for other lines in the file
} else if (strstr(line, ")") != NULL && reading_port) {
    // Set the reading_port flag to 0 when a ")" is encountered
    reading_port = 0;
} else if (strstr(line, "END") != NULL && reading_entity) {
    // Set the reading_entity flag to 0 when "END" is encountered
    reading_entity = 0;
        } else if (reading_port) {
    //split again for the rest of the line with almost the same algorithm that use in PORT when exist informatios
            char port_label[MAX_CHARACTERS];
            char port_name[MAX_CHARACTERS];
            char io_names[MAX_DEFINITION][MAX_CHARACTERS];
            int num_io_names = 0;
            char *token1= strtok(line, ":");
            sscanf(token1, "%s %s", port_label, port_name);
            token1=strtok(NULL,":");

            char *token = strtok(token1, "{,");
            if(strstr(token,"}")!=NULL){
            token=strtok(token,"}");
            sscanf(token, " %s", io_names[num_io_names]);

            }
            sscanf(token, " %s", io_names[num_io_names]);
            while (token != NULL) {
                token = strtok(NULL, ",");
                if (token != NULL) {
                 if(strstr(token,"}")!=NULL){
                  token=(strtok(token, "}"));
                 }
                    num_io_names++;
                    sscanf(token, " %s", io_names[num_io_names]);
                    

                }

                }


            c1 = addIO(comp_name,current_entity->PortMap, port_name, port_label, io_names, num_io_names, num_io_names, c1);
        }
    }

    fclose(fp);

return entity_list;
 }



/*
   makeOutputFile is a function that generates a netlist data structure from a given Entity_list.
   new_entity_list : Entity_list structure containing the information about the components and connections
   returns : pointer to the generated netlist data structure
 */

arrayofnetlist* makeOutputFile(Entity_list*new_entity_list){
arrayofnetlist* new_array_list=(arrayofnetlist*)malloc(sizeof(arrayofnetlist));
   int id=0; // unique identifier for each component
   char type[MAX_CHARACTERS]={0}; // type of component
   char inputs[MAX_DEFINITION][MAX_CHARACTERS]={0}; // input ports of the component

   int counter=0;
   int counter1=0;
   int c3=0;

   int label;
   int k;
   OutputArray *new_out_array = NULL;
   Component_array *new_Component_array = NULL;
   Component* new_Comp=NULL;

   //loop through each entity in the entity list
   for(int i=0;i<new_entity_list->numofEntity;i++){
    strcpy(type,new_entity_list->entity_array[i].PortMap->name);
     counter1=0;
      for( k=0;k<new_entity_list->entity_array[i].PortMap->ionum;k++){

         // if you find  label out alocate an OutputArray
         if(strcmp(new_entity_list->entity_array[i].PortMap->IO_array[k].label,"OUT")==0){
            new_out_array=(OutputArray*)malloc(sizeof(OutputArray));
            new_out_array->output_num=-1;
            break;
         }
      }
      for(k=0;k<new_entity_list->entity_array[i].PortMap->ionum;k++){
          // if you find  label out alocate an Component_array
         if(strcmp(new_entity_list->entity_array[i].PortMap->IO_array[k].label,"IN")==0){
            new_Component_array=(Component_array*)malloc(sizeof(Component_array));
            new_Component_array->num_components=-1;
            break;
         }
      }

      //loop through each variable in the variable declaration */
      for(int n=0;n<new_entity_list->entity_array[i].variables_dec->NumOfvariables;n++){
         counter=new_entity_list->entity_array[i].variables_dec->ArrayOfVariables[n].value;
         for(int j=0;j<new_entity_list->entity_array[i].variables_dec->ArrayOfVariables[n].value;j++){
            new_Comp = (Component*)malloc(sizeof(Component));
             new_Comp->numofinputs=0;

            id=j;

for(int m=0;m<new_entity_list->entity_array[i].PortMap->ionum;m++){

    // loop through the ionum of the PortMap of the current entity
    if(strcmp(new_entity_list->entity_array[i].PortMap->IO_array[m].label,"IN")==0){
        // compare the label of the current IO_array to see if it is an input port
        if(new_entity_list->entity_array[i].PortMap->IO_array[m].numofIOnames==new_entity_list->entity_array[i].variables_dec->ArrayOfVariables[n].value){

            // if the number of IOnames is equal to the number of variables
            strcpy(inputs[m],new_entity_list->entity_array[i].PortMap->IO_array[m].IOnames[counter-1]);
           
            new_Comp->numofinputs++;

            // copy the name of the IOnames to the inputs array
            label=-1;
            // set label to -1
        }else{
            int index=new_entity_list->entity_array[i].PortMap->IO_array[m].numofIOnames;

            if(m > new_entity_list->entity_array[i].variables_dec->ArrayOfVariables[n].value-1){

            strcpy(inputs[m],new_entity_list->entity_array[i].PortMap->IO_array[m].IOnames[index-1]);
            new_Comp->numofinputs=new_Comp->numofinputs+1;
            }else{
            index=new_entity_list->entity_array[i].PortMap->IO_array[m].numofIOnames;
            // get the number of IOnames
            if(counter1<index ) {
                // if counter1 is less than the index
                strcpy(inputs[m],new_entity_list->entity_array[i].PortMap->IO_array[m].IOnames[index-1]);
                new_Comp->numofinputs++;
                // copy the name of the IOnames to the inputs array
                counter1++;
                // increment the counter1
                label=-1;
                // set label to -1
            }else{
                // if counter1 is not less than the index
                for(int n=0;n<new_entity_list->entity_array[i].PortMap->ionum;n++){
                    // loop through the ionum of the PortMap of the current entity
                    if(strcmp(new_entity_list->entity_array[i].PortMap->IO_array[n].label,"OUT")==0){
                        // compare the label of the current IO_array to see if it is an output port
                        if(new_entity_list->entity_array[i].PortMap->IO_array[n].numofIOnames==new_entity_list->entity_array[i].PortMap->IO_array[m].numofIOnames){
                            // if the number of IOnames of the input port is equal to the number of IOnames of the output port
                            label=n;

                            // set the label to the index of the output port
                            sprintf(inputs[m],"U%d_%s",id-1,new_entity_list->entity_array[i].PortMap->IO_array[n].name);
                            new_Comp->numofinputs++;
                            // format the inputs string with the index of the component and the name of the output port
                            counter1++;
                            // increment the counter1
                            }

                        }
                    }

                }

            }
        }
     }
}

add_component(new_Component_array,new_Comp,type,inputs,id,label);
counter=counter-1;
}
//for make the output
// Iterates over all the IO elements in `new_entity_list->entity_array[i].PortMap->ionum`
for(int m=0;m<new_entity_list->entity_array[i].PortMap->ionum;m++){

// Checks if the label of the current IO element is "OUT"
if(strcmp(new_entity_list->entity_array[i].PortMap->IO_array[m].label,"OUT")==0){

// Allocates memory for the output port name, origin component index, and label */
char *Output; // output port name
int origin; // index of the originating component
char *l; //label about S,C OR SIMPLE OUTPUT
counter1=0;

Output = (char *)malloc(256 * sizeof(char));
l=(char*)malloc(256*sizeof(char));
counter=new_entity_list->entity_array[i].variables_dec->ArrayOfVariables[n].value;

// If the number of IO names for the current IO element is equal to the value N
if(new_entity_list->entity_array[i].PortMap->IO_array[m].numofIOnames==new_entity_list->entity_array[i].variables_dec->ArrayOfVariables[n].value){

  for(int j=0;j<new_entity_list->entity_array[i].variables_dec->ArrayOfVariables[n].value;j++){
    // Allocates memory for the output declaration
    Output_Dec* new_dec_array=(Output_Dec*)malloc(sizeof(Output_Dec));

    //Copies the current IO name to the output port name
    strcpy(Output,new_entity_list->entity_array[i].PortMap->IO_array[m].IOnames[counter-1]);

    // Sets the origin component index to the component ID at index `j` in the component array `new_Component_array` */
    origin=new_Component_array->components[j].id;

 strcpy(l,new_entity_list->entity_array[i].PortMap->IO_array[m].name);
addOutput(new_out_array,new_dec_array,Output,origin,l);

counter--;

}
// if the number of IO names for the current IO element is not equal to the value N
}else{
// Allocate memory for a new Output_Dec structure
Output_Dec* new_dec_array = (Output_Dec*) malloc(sizeof(Output_Dec));

// Get the number of IO names for the current output port
int index = new_entity_list->entity_array[i].PortMap->IO_array[m].numofIOnames;

// Check if the counter is less than the number of IO names
if (counter1 < index) {
  // Copy the name of the current IO name to the Output variable
  strcpy(Output, new_entity_list->entity_array[i].PortMap->IO_array[m].IOnames[index - 1]);

  // Increment the counter
  counter1++;

  // Copy the label of the current output port to the l variable
  strcpy(l, new_entity_list->entity_array[i].PortMap->IO_array[m].name);

  // Get the id of the last component in the new_Component_array
  origin = new_Component_array->components[new_Component_array->num_components - 1].id;

  // Add the new Output_Dec structure to the new_out_array
  addOutput(new_out_array, new_dec_array, Output, origin, l);
}
   }
     }
       }
         }
// Assign the new_Component_array and new_out_array to the new_net structure
c3=addnetlist(new_array_list,new_Component_array,new_out_array,c3);
           }



// Return the new_net structure
return new_array_list;
}

int main()
{

Entity_list* new_entity_list = (Entity_list*)malloc(sizeof(Entity_list));

char * Filename = "input.txt";
char*OutputFile ="output.txt";
new_entity_list=ReadInputFile(Filename,new_entity_list);
arrayofnetlist* new_array_list=(arrayofnetlist*)malloc(sizeof(arrayofnetlist));

new_array_list=makeOutputFile(new_entity_list);
FILE* fp= fopen(OutputFile,"w");

//write in File
for( int m=0; m<new_array_list->numofnetlist;m++){

for (int i=0;i<new_array_list->different_netlist[m].new_Component_array->num_components;i++){


        fprintf(fp, "U%d %s ", new_array_list->different_netlist[m].new_Component_array->components[i].id, new_array_list->different_netlist[m].new_Component_array->components[i].type);
      for(int j=0;j<new_array_list->different_netlist[m].new_Component_array->components[i].numofinputs;j++){
            if (strcmp(new_array_list->different_netlist[m].new_Component_array->components[i].inputs[j], "") == 0) {
                break;
            }
            if (strcmp(new_array_list->different_netlist[m].new_Component_array->components[i].inputs[j+1], "") == 0){
             

            fprintf(fp,"%s", new_array_list->different_netlist[m].new_Component_array->components[i].inputs[j]);


            }else{
             fprintf(fp,"%s,", new_array_list->different_netlist[m].new_Component_array->components[i].inputs[j]);}
        }

     fprintf(fp,"\n");
    }

    for (int k=0;k<new_array_list->different_netlist[m].outputarray->output_num;k++){


    fprintf(fp,"%s= U%d_%s\n", new_array_list->different_netlist[m].outputarray->Output_Dec_array[k].Output,new_array_list->different_netlist[m].outputarray->Output_Dec_array[k].origin,new_array_list->different_netlist[m].outputarray->Output_Dec_array[k].label);

    }
   }
fclose(fp);

return 1;
}
