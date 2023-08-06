// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

// Define the length
#define MAX_DEFINITION 50
#define MAX_INPUTS 200
#define MAX_CHARACTERS 25
#define MAX_COMPONENTS 100
#define BILLION  1000000000L;

// structs

// Structure for a DEC (Digital Element) which represents a component
typedef struct DEC {
    char name[MAX_DEFINITION];             // Name of the component
    char inputs[MAX_INPUTS][MAX_CHARACTERS];   // Input ports of the component
    int truth_table[MAX_DEFINITION];       // Truth table for the component
    int numofelementsoftr;                 // Number of elements in the truth table
    int numofinputs;                       // Number of input ports
} DEC;

// Structure for a list of DEC components
typedef struct DEC_LIST {
    DEC* dec_list;        // Array of DEC components
    int decnum;           // Number of DEC components
} DEC_LIST;

// Define the Component structure
typedef struct Component {
    int id;                                // Unique identifier for each component
    char type[MAX_CHARACTERS];             // Type of component
    char inputs[MAX_INPUTS][MAX_CHARACTERS]; // Input ports of the component
    int connections[MAX_DEFINITION];       // Array to store connections
    int num_connections;                   // Number of connections
    int numofinputs;                       // Number of input ports
} Component;

// Define the netlist structure
typedef struct {
    Component components[MAX_COMPONENTS];   // Array of components
    int num_components;                     // Number of components in the netlist
} comp_array;

// Structure for representing the output declaration of a component
typedef struct Output_Dec {
    char *Output;   // Output port name
    int origin;     // Index of the originating component
} Output_Dec;

// Define the OutputArray structure
typedef struct OutputArray {
    Output_Dec Output_Dec_array[MAX_COMPONENTS]; // Array of output declarations
    int output_num;                             // Number of outputs in the array
} OutputArray;

// Structure for representing the net system file
typedef struct netsystemFile {
    comp_array *subnetlist;    // Pointer to a netlist
    OutputArray *new_out_array; // Pointer to an array of output declarations
} netsystemFile;

// Structure for representing the test inputs
typedef struct testinputs {
    char input[MAX_CHARACTERS];    // Input port name
    int values[MAX_DEFINITION];    // Values for the input port
    int num_values;                // Number of values
} testinputs;

// Structure for representing an array of test inputs
typedef struct test_array {
    testinputs tinput[MAX_INPUTS]; // Array of test inputs
    int num_tests;                 // Number of tests
} test_array;

// Structure for representing the testbench
typedef struct testbench {
    test_array* tb;                 // Pointer to a test array
    int maxtestbenchvalues;         // Maximum number of test bench values
    char out[MAX_INPUTS][MAX_CHARACTERS]; // Output port names
    int outnum;                     // Number of output ports
} testbench;

// Structure for representing the results of a test
typedef struct result {
    char** name;      // Array of input port names
    int *inputs;      // Array of input values
    int numinput;     // Number of input ports
    char** outputname; // Array of output port names
    int *out;         // Array of output values
    int numofout;     // Number of output ports
    int calculate;    // Calculation flag
} result;

// Structure for representing an array of results
typedef struct resultarray {
    result* resarray; // Array of results
    int resnum;       // Number of results
} resultarray;

// Structure for representing the index
typedef struct index {
    int result;  // Result index
    int couner;  // Counter
}index;


/*
 * Create a new result struct with the given number of inputs and outputs.
 * Allocate memory for the struct and its arrays.
 * Return a pointer to the created result struct.
 */
result* createResult(int numinput, int numofout) {
    result* res = malloc(sizeof(result));
    res->numinput = numinput;
    res->numofout = numofout;
    res->name = malloc(numinput * sizeof(char*));
    res->outputname = malloc(numofout * sizeof(char*));
    res->inputs = malloc(numinput * sizeof(int));
    res->out = malloc(numofout * sizeof(int));
    return res;
}

/*
 * Add a name to the specified position in the result struct's name array.
 * Check if the counter is within bounds before adding the name.
 */
void addName(result* res, const char* name, int counter) {
    if (counter >= 0 && counter < res->numinput) {
        res->name[counter] = strdup(name);
    }
}

/*
 * Add a name to the specified position in the result struct's outputname array.
 * Check if the counter is within bounds before adding the name.
 */
void addNameOut(result* res, const char* name, int counter) {
    if (counter >= 0 && counter < res->numofout) {
        res->outputname[counter] = strdup(name);
    }
}

/*
 * Add a result struct to the resultarray.
 * Reallocate memory for the resultarray's resarray to accommodate the new result.
 * Add the result to the resarray at the next available position.
 * Increment the resnum to reflect the new addition.
 */
void addResulttoArray(resultarray* resarr, result res) {
    resarr->resarray = (result*)realloc(resarr->resarray, (resarr->resnum + 1) * sizeof(result));
    resarr->resarray[resarr->resnum] = res;
    resarr->resnum++;
}

/*
 * Check if a line is empty or contains only whitespace characters.
 * Return true if the line is empty or contains only whitespace, false otherwise.
 */
bool isLineEmpty(const char* line) {
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return false; // Line contains non-whitespace characters
        }
        line++;
    }
    
    return true; // Line is empty or contains only whitespace
}

/*
 * Add a component to the component array.
 * Check if the component array is valid and if the maximum number of components has been reached.
 * Create a new component pointer and copy the data from the new_component to it.
 * Increment the num_components to reflect the new addition.
 */
void addComponent(comp_array* array, Component new_component) {
    if (array == NULL) {
        printf("Invalid comp_array.\n");
        return;
    }

    if (array->num_components >= MAX_COMPONENTS) {
        printf("Maximum number of components reached.\n");
        return;
    }

    Component* component_ptr = &(array->components[array->num_components]);
    component_ptr->id = new_component.id;
    strcpy(component_ptr->type, new_component.type);
    component_ptr->num_connections = new_component.num_connections;
    component_ptr->numofinputs = new_component.numofinputs;

    for (int i = 0; i < new_component.num_connections; i++) {
        component_ptr->connections[i] = new_component.connections[i];
    }

    for (int i = 0; i < new_component.numofinputs; i++) {
        strcpy(component_ptr->inputs[i], new_component.inputs[i]);
    }

    array->num_components++;
}

/*
 * Add an output to the OutputArray.
 * Check if the OutputArray is valid and if the maximum number of outputs has been reached.
 * Create a new Output_Dec pointer and allocate memory for the Output field.
 * Copy the output_name and origin to the allocated memory.
 * Increment the output_num to reflect the new addition.
 */
void addOutput(OutputArray* array, Output_Dec newoutDec) {
    if (array == NULL) {
        printf("Invalid OutputArray.\n");
        return;
    }

    if (array->output_num >= MAX_COMPONENTS) {
        printf("Maximum number of outputs reached.\n");
        return;
    }

    Output_Dec* output_ptr = &(array->Output_Dec_array[array->output_num]);

    // Allocate memory for the Output field
    output_ptr->Output = malloc(strlen(newoutDec.Output) + 1);
    if (output_ptr->Output == NULL) {
        printf("Failed to allocate memory for Output.\n");
        return;
    }

    // Copy the output_name into the allocated memory
    strcpy(output_ptr->Output, newoutDec.Output);

    output_ptr->origin = newoutDec.origin;

    array->output_num++;
}

/*
 * Add an input and its corresponding value to the testinputs struct.
 * Check if the num_values is within bounds before adding the input and value.
 */
void add_to_testinputs(testinputs* ti, char* input, int value) {
    if (ti->num_values < MAX_DEFINITION) {
        strcpy(ti->input, input);
        ti->values[ti->num_values-1] = value;
    } else {
        printf("The testinputs struct is full.\n");
    }
}

/*
 * Add a testinputs struct to the test_array.
 * Check if the num_tests is within bounds before adding the testinputs struct.
 */
void add_to_test_array(test_array* ta, testinputs ti) {
    if (ta->num_tests < MAX_INPUTS) {
        ta->tinput[ta->num_tests] = ti;
        ta->num_tests++;
    } else {
        printf("The test_array struct is full.\n");
    }
}

/*
 * Add a test_array and an output to the testbench.
 * Assign the test_array to the testbench's tb field.
 * Copy the output name to the next available position in the out array.
 * Increment the outnum to reflect the new addition.
 */
void add_to_testbench(testbench* tb, test_array* ta, char* output) {
    tb->tb = ta;
    strcpy(tb->out[tb->outnum], output);
    tb->outnum++;
}

/*
 * Add a new DEC to the DEC_LIST.
 * Check if the DEC_LIST is valid and if it has reached its maximum capacity.
 * Reallocate memory for the DEC_LIST to accommodate the new DEC.
 * Copy the new DEC data to the allocated memory.
 * Increment the decnum to reflect the new addition.
 */
void addDEC(DEC_LIST* dec_list, DEC new_dec) {
    // Check if the dec_list is NULL or reached its maximum capacity
    if (dec_list == NULL || dec_list->decnum >= MAX_DEFINITION) {
        printf("DEC_LIST is full or invalid.\n");
        return;
    }
    
    // Reallocate memory for the dec_list to accommodate the new DEC
    dec_list->dec_list = realloc(dec_list->dec_list, (dec_list->decnum + 1) * sizeof(DEC));
    if (dec_list->dec_list == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    
    // Copy the new DEC data to the allocated memory
    DEC* new_dec_ptr = &(dec_list->dec_list[dec_list->decnum]);
    strcpy(new_dec_ptr->name, new_dec.name);
    
    // Copy each input string individually
    for (int i = 0; i < new_dec.numofinputs; i++) {
        strcpy(new_dec_ptr->inputs[i], new_dec.inputs[i]);
    }
    
    memcpy(new_dec_ptr->truth_table, new_dec.truth_table, sizeof(new_dec.truth_table));
    new_dec_ptr->numofelementsoftr = new_dec.numofelementsoftr;
    new_dec_ptr->numofinputs = new_dec.numofinputs;
    
    // Increment the decnum to reflect the new addition
    dec_list->decnum++;
}

/*
 * Print the contents of the resultarray.
 */
void printResultArray(resultarray resarr) {
    printf("Result Array:\n");
    printf("Number of Results: %d\n", resarr.resnum);
    
    for (int i = 0; i < resarr.resnum; i++) {
        printf("Result %d:\n", i+1);
        printf("  Number of Inputs: %d\n", resarr.resarray[i].numinput);
        printf("  Number of Outputs: %d\n", resarr.resarray[i].numofout);
        
        printf("  Inputs:\n ");
        for (int j = 0; j < resarr.resarray[i].numinput; j++) {
            printf(" %s  ", resarr.resarray[i].name[j], resarr.resarray[i].inputs[j]);
        }
        printf("\n");
        
        for (int j = 0; j < resarr.resarray[i].numinput; j++) {
            printf("   %d  ", resarr.resarray[i].inputs[j]);
        }
        printf("\n");
        
        printf("Outputs:\n ");
        for (int j = 0; j < resarr.resarray[i].numofout; j++) {
            printf(" %s   ", resarr.resarray[i].outputname[j]);
        }
        printf("\n");
        
        for (int j = 0; j < resarr.resarray[i].numofout; j++) {
            printf("   %d  ",  resarr.resarray[i].out[j]);
        }
        printf("\n");
    }
}


// Function to find the truth table index based on the given inputs
// Returns a pointer to an index structure containing the result and counter
index* findtruthtableindex(netsystemFile* file, int i, int* array, int arraySize, index* id) {
    int total = 0;
    int flag = 0;
    int counter = 0;

    comp_array* subnetlist = file->subnetlist;

    const Component* component = &(subnetlist->components[i]);

    // Modify the array values
    for (int j = 0; j < component->numofinputs && component->numofinputs == arraySize; j++) {
        flag = 1;

        counter++;
        total += array[j] * pow(2, j);
    }

    if (flag == 0) { 
        // If no inputs are found, set the result to -1 and update the counter
        id->result = -1;
        id->couner= id->couner + counter;
        return id;
    }

    id->result = total;
    id->couner = id->couner + counter;

    return id;
}

// Function to parse a file and store the component declarations in a DEC_LIST
DEC_LIST CutandStoreComponentFile(FILE* file) {
    DEC_LIST dec_list;
    dec_list.dec_list = NULL;
    dec_list.decnum = 0;

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        int input_count = 0;
        int i = 0;

        if (strncmp(line, "COMP", 4) == 0) {
            DEC new_dec;
            char* token = strtok(line, ";");

            char* token1 = strtok(NULL, ";");

            char* token2 = strtok(NULL, ";");
            strtok(token, " ");
            char* name = strtok(NULL, " ");
            strcpy(new_dec.name, name);
            strtok(token1, ":");
            char* input = strtok(NULL, ":");

            if (strstr(input, ",")) {
                // If there are multiple inputs separated by commas
                input = strtok(input, ",");
                strcpy(new_dec.inputs[input_count], input);
                input_count++;

                while (input != NULL) {
                    input = strtok(NULL, ",");
                    if (input != NULL) {
                        strcpy(new_dec.inputs[input_count], input);
                        input_count++;
                    }
                }
            } else {
                // If there is only one input
                strcpy(new_dec.inputs[input_count], input);
                input_count++;
            }

            new_dec.numofinputs = input_count;

            if (strstr(token2, ",")) {
                // If there are multiple truth table values separated by commas
                char* value = strtok(token2, ",");
                new_dec.truth_table[i] = atoi(value);
                i++;

                while (value != NULL) {
                    value = strtok(NULL, ",");
                    if (value != NULL) {
                        new_dec.truth_table[i] = atoi(value);
                        i++;
                    }
                }
            } else {
                // If there is only one truth table value
                new_dec.truth_table[i] = atoi(token2);
                i++;
            }

            new_dec.numofelementsoftr = i;

            addDEC(&dec_list, new_dec);
        }
    }

    fclose(file);

    return dec_list;
}

// Function to trim leading and trailing whitespace from a string
void trimWhitespace(char* str) {
    int len = strlen(str);
    int start = 0, end = len - 1;

    // Find the start index of non-whitespace characters
    while (isspace(str[start])) {
        start++;
    }

    // Find the end index of non-whitespace characters
    while (end >= start && isspace(str[end])) {
        end--;
    }

    // Shift the non-whitespace characters to the beginning of the string
    int i;
    for (i = 0; i <= end - start; i++) {
        str[i] = str[start + i];
    }

    // Null-terminate the trimmed string
    str[i] = '\0';
}

// Function to parse a file and store the network system declarations in a netsystemFile structure
netsystemFile parseFileAndStoreNetsystem(const char* filename) {
    netsystemFile file;
    file.subnetlist = malloc(sizeof(comp_array));
    file.subnetlist->num_components = 0;
    file.new_out_array = malloc(sizeof(OutputArray));
    file.new_out_array->output_num = 0;

    // Open the file for reading
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Failed to open the file.\n");
        return file;
    }

    char line[MAX_CHARACTERS];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "=") == 0 && (isLineEmpty(line) == false)) {
            // It's a component declaration
            Component new_component;

            // Parse the component data from the line
            int id;
            char type[MAX_CHARACTERS];
            char inputs[MAX_INPUTS][MAX_CHARACTERS];
            sscanf(line, "U%d %s %[^\n]", &id, type, inputs[0]);
            int input_count = 0;

            char* input = strtok(inputs[0], " ");

            while (input != NULL) {
                trimWhitespace(input);
                strcpy(new_component.inputs[input_count], input);

                input = strtok(NULL, " ");
                input_count++;
            }

            // Set the component properties
            new_component.id = id;
            strcpy(new_component.type, type);
            new_component.num_connections = 0;
            new_component.numofinputs = input_count;
            int counter = 0;

            // Store the connections in the Component's connections array
            for (int i = 0; i < input_count; i++) {
                int connection_index;

                char* input1 = new_component.inputs[i];
                if (strstr(input1, "U") == 0) {
                    connection_index = -1;
                } else {
                    connection_index = atoi(input1 + 1); // Skip the first character (e.g., 'U') and convert the remaining substring to an integer
                    new_component.connections[counter] = connection_index; // Store the connection index in the connections array
                    counter++;
                }
            }
            new_component.num_connections = counter;

            // Add the new component to the subnetlist
            addComponent(file.subnetlist, new_component);
        } else {
            // It's an output declaration
            if (isLineEmpty(line) == false) {
                char output_name[MAX_CHARACTERS];
                char origin[MAX_CHARACTERS];
                Output_Dec new_out;

                sscanf(line, "%s = %s", output_name, origin);
                int origin_index = atoi(origin + 1);

                new_out.origin = origin_index;
                new_out.Output = malloc(strlen(output_name) + 1); // Allocate memory for the Output field
                strcpy(new_out.Output, output_name); // Copy the output_name into the allocated memory

                // Add the new output to the output array
                addOutput(file.new_out_array, new_out);
            }
        }
    }

    fclose(fp);
    return file;
}


testbench parseFileAndStoreTestBench(const char* filename) {
    testbench tb;
    tb.tb = malloc(sizeof(test_array));
    
    // Initialize the output array
    for (int i = 0; i < MAX_INPUTS; i++) {
        tb.out[i][0] = '\0';
    }
    tb.tb->num_tests = 0;
    tb.outnum = 0;

    // Open the file for reading
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Failed to open the file.\n");
        return tb;
    }

    char line[256];
    
    int is_input_declared = 0;
    int is_output_declared = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "IN") != 0) {
            // It's an input declaration line
            is_input_declared = 1;
            is_output_declared = 0;
        }
        if (strstr(line, "OUT") != 0) {
            // It's an output declaration
            is_output_declared = 1;
            is_input_declared = 0;
        }  
        if (is_input_declared == 1) {
            if (strstr(line, ",") != 0) {
                testinputs tinput;
                tinput.num_values = 0;

                // It's the line with input name and values
                sscanf(line, "%s", tinput.input);

                strtok(line, " ,");
                char* value = strtok(NULL, " ,");

                // Tokenize the values and store them in the testinputs struct
                tinput.num_values++;

                while (value != NULL) {
                    add_to_testinputs(&tinput, tinput.input, atoi(value));
                    value = strtok(NULL, " ,");
                    if (value != NULL) {
                        tinput.num_values++;
                    }
                }
           
                // Add the test inputs to the test_array
                add_to_test_array(tb.tb, tinput);
            } 
        }
        
        if (is_output_declared == 1) {
            if (line[0] != 'O' && line[1] != 'U'){
                // It's the line with the output value
                char output[MAX_CHARACTERS];
                sscanf(line, "%s", output);
                  
                // Add the test inputs and output to the testbench
                add_to_testbench(&tb, tb.tb, output);
            }   
        }
    }
    
    // Calculate the maximum number of values in the testbench
    if (tb.tb->num_tests >= 0) {
        int maxNumValues = tb.tb->tinput[0].num_values;
      
        for (int i = 0; i < tb.tb->num_tests; i++) {
            if (tb.tb->tinput[i].num_values > maxNumValues) {
                maxNumValues = tb.tb->tinput[i].num_values;
            }
        }
        
        tb.maxtestbenchvalues = maxNumValues;
    }
    
    fclose(fp);
    return tb;
}

/* 
   Function: makethearrayforInputs
   
   Description:
   Creates an array of input values from the testbench for a specific test case.

   Parameters:
   - tb: The testbench object.
   - c: The index of the input value to extract.

   Returns:
   - A dynamically allocated array of input values.
*/
int* makethearrayforInputs(testbench tb, int c) {
    // Allocate memory for the array
    int* array = (int*)malloc(tb.tb->num_tests * sizeof(int));

    // Fill the array with values from the testbench
    for (int i = 0; i < tb.tb->num_tests; i++) {
        array[i] = tb.tb->tinput[i].values[c];
    }

    return array;
}


/*
   Function: init_theOutputArray

   Description:
   Initializes the  nextBuffer arrays with input values and component IDs.

   Parameters:
   
   - nextBuffer: The next buffer array.
   - i: The index of the component being processed.
   - nfile: The network system file object.
   - dec_list: The decision list object.
   - tb: The testbench object.
   - c: The index of the input value to extract.
   

   Returns:
   - The updated nextBuffer array.
*/
int** init_theOutputArray(int** nextBuffer, int i, netsystemFile nfile, DEC_LIST dec_list, testbench tb, int c) {
    int* array1 = makethearrayforInputs(tb, c);

    // Store the component ID in the first column of the currentBuffer
    nextBuffer[i][0] = nfile.subnetlist->components[i].id;

    for (int m = 0; m < nfile.subnetlist->components[i].numofinputs; m++) {
        if (nfile.subnetlist->components[i].num_connections == 0) {
            // If the component has no connections, assign input values directly
            for (int t = 0; t < tb.tb->num_tests; t++) {
                if (strcmp(nfile.subnetlist->components[i].inputs[m], tb.tb->tinput[t].input) == 0) {
                    nextBuffer[i][m + 1] = array1[t];

                    
                    
                }
            }
        } else if (nfile.subnetlist->components[i].num_connections == nfile.subnetlist->components[i].numofinputs) {
            // If the component has all connections, mark inputs as -1
            nextBuffer[i][m + 1] = -1;
        } else {
            // If the component has some connections
            for (int t = 0; t < tb.tb->num_tests; t++) {
                if (strcmp(nfile.subnetlist->components[i].inputs[m], tb.tb->tinput[t].input) == 0) {
                    nextBuffer[i][m + 1] = array1[t];

                   
                    break;
                } else {
                    nextBuffer[i][m + 1] = -1;
                }
            }
        }
       
    }

    return nextBuffer;
}


/*
   Function: CompleteArray

   Description:
   Completes the connection values in the currentBuffer array based on the specified value.

   Parameters:
   - currentBuffer: The current buffer array.
   - i: The index of the component being processed.
   - value: The value to assign to missing connections.
   - dec_list: The decision list object.
   - nfile: The network system file object.

  
*/
void CompleteArray(int** currentBuffer, int i, int value, DEC_LIST dec_list, netsystemFile nfile) {
    for (int j = 0; j < nfile.subnetlist->num_components; j++) {
        int counter = 0;
        for (int k = 0; k < nfile.subnetlist->components[j].num_connections; k++) {
            if (nfile.subnetlist->components[j].connections[k] == currentBuffer[i][0]) {
                if (currentBuffer[j][k + 1] != -1) {
                    k = k + 1;
                }
                counter++;

                // Update the connections with the specified value
                for (int s = 0; s < counter; s++) {
                    if (currentBuffer[j][k + 1] == -1) {
                        currentBuffer[j][k + 1] = value;
                    }
                }
            }
        }
    }
}


/*
   Function: takeallthathastodowithmissing

   Description:
   Recursively collects all components that have missing connections to the specified component.

   Parameters:
   - array: The array to store component IDs.
   - i: The index of the component being processed.
   - circuit: The circuit component array.
   - counter: The current counter value.

   Returns:
   - The updated counter value.
*/
int takeallthathastodowithmissing(int array[MAX_COMPONENTS], int i, comp_array* circuit, int counter) {
    for (int j = 0; j < circuit->num_components; j++) {
        for (int h = 0; h < circuit->components[j].num_connections; h++) {
            if (circuit->components[i].id == circuit->components[j].connections[h]) {
                array[counter] = circuit->components[j].id;
                counter++;

                // Recursive call to handle missing connections
                counter = takeallthathastodowithmissing(array, j, circuit, counter);
            }
        }
    }
    return counter;
}


/*
   Function: checkMissingConnections

   Description:
   Checks if a component has any missing connections.

   Parameters:
   - circuit: The circuit component array.
   - componentIndex: The index of the component being checked.

   Returns:
   - True if all connections are found, False otherwise.
*/
bool checkMissingConnections(comp_array* circuit, int componentIndex) {
    for (int j = 0; j < circuit->components[componentIndex].num_connections; j++) {
        int connectionID = circuit->components[componentIndex].connections[j];
        bool connectionFound = false;

        // Check if the connection is found in other components
        for (int k = 0; k < circuit->num_components; k++) {
            if (k != componentIndex && connectionID == circuit->components[k].id) {
                connectionFound = true;
                break;
            }
        }

        if (!connectionFound) {
            return false;
        }
    }
    return true;
}

/*
   Function: checkarray

   Description:
   Processes the currentBuffer arrays to update the missing connections and truth table values.

   Parameters:
   - currentBuffer: The current buffer array.
   - counter1: The counter for missing connections.
   - dec_list: The decision list object.
   - nfile: The network system file object.
   - idx: The index object.

   Returns:
   - The updated index object.
*/
index* checkarray(int** currentBuffer, int counter1, DEC_LIST dec_list, netsystemFile nfile, index* idx) {
    int c = 0;
    int array2[MAX_COMPONENTS];

    // Iterate over components in the subnetlist
    for (int i = 0; i < nfile.subnetlist->num_components; i++) {
        int counter = 0;
        int* array1 = (int*)malloc(nfile.subnetlist->components[i].numofinputs * sizeof(int));

        // Check if the component in the currentBuffer has been processed
        if (currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1] == -1) {
            // Iterate over inputs of the component
            for (int j = 0; j < nfile.subnetlist->components[i].numofinputs; j++) {
                if (currentBuffer[i][j + 1] != -1) {
                    array1[j] = currentBuffer[i][j + 1];
                    counter++;
                } else {
                    counter1++;
                }
            }
            
            // Find the truthtable index for the component
            index* id = findtruthtableindex(&nfile, i, array1, counter, idx);
            
            if (id->result != -1 && counter1 == 0) {
                // Find the corresponding truth table value and update nextBuffer
                for (int k = 0; k < dec_list.decnum; k++) {
                    if (strcmp(dec_list.dec_list[k].name, nfile.subnetlist->components[i].type) == 0) {
                        currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1] = dec_list.dec_list[k].truth_table[id->result];

                        // Process the updated values in the buffers
                        CompleteArray(currentBuffer, i, currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1], dec_list, nfile);
                        idx->couner++;
                    }
                }
            } else {
                // Check if missing connections exist
                bool Cexists = checkMissingConnections(nfile.subnetlist, i);
                if (Cexists == false) {
                    // Collect component IDs for missing connections
                    array2[c] = currentBuffer[i][0];
                    c++;
                    c = takeallthathastodowithmissing(array2, i, nfile.subnetlist, c);
                }
            }
        }
    }
    
    for (int d = 0; d < nfile.subnetlist->num_components; d++) {
        if (c > 0) {
            for (int s = 0; s < c; s++) {
                // Check if component ID is in array2
                if (array2[c] == nfile.subnetlist->components[s].id) {
                    // do nothing
                } else {
                    // Process components with missing connections
                    for (int i = 0; i < nfile.subnetlist->num_components; i++) {
                        int counter = 0;
                        int* array1 = (int*)malloc(nfile.subnetlist->components[i].numofinputs * sizeof(int));
                        if (currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1] == -1) {
                            for (int j = 0; j < nfile.subnetlist->components[i].numofinputs; j++) {
                                if (currentBuffer[i][j + 1] != -1) {
                                    array1[j] = currentBuffer[i][j + 1];
                                    counter++;
                                }
                            }
                            index* id = findtruthtableindex(&nfile, i, array1, counter, idx);
                            if (id->result != -1) {
                                for (int k = 0; k < dec_list.decnum; k++) {
                                    if (strcmp(dec_list.dec_list[k].name, nfile.subnetlist->components[i].type) == 0) {
                                        currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1] = dec_list.dec_list[k].truth_table[id->result];
                                        
                                        // Process the updated values in the buffers
                                        CompleteArray(currentBuffer, i, currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1], dec_list, nfile);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            // Process components without missing connections
            for (int i = 0; i < nfile.subnetlist->num_components; i++) {
                int counter = 0;
                int* array1 = (int*)malloc(nfile.subnetlist->components[i].numofinputs * sizeof(int));
                if (currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1] == -1) {
                    for (int j = 0; j < nfile.subnetlist->components[i].numofinputs; j++) {
                        if (currentBuffer[i][j + 1] != -1) {
                            array1[j] = currentBuffer[i][j + 1];
                            counter++;
                        }
                    }
                    index* id = findtruthtableindex(&nfile, i, array1, counter, idx);
                    if (id->result != -1) {
                        for (int k = 0; k < dec_list.decnum; k++) {
                            if (strcmp(dec_list.dec_list[k].name, nfile.subnetlist->components[i].type) == 0) {
                                currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1] = dec_list.dec_list[k].truth_table[id->result];
                                
                                // Process the updated values in the buffers
                                CompleteArray(currentBuffer, i, currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1], dec_list, nfile);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return idx;
}








/* 
 * Function: update_theOutput
 * --------------------------
 * Updates the output values in the nextBuffer based on the currentBuffer and component information.
 *
 * tb: The testbench structure.
 * currentBuffer: The current buffer containing the input values.
 * dec_list: The list of available components.
 * nfile: The netsystemFile structure containing the subnetlist and new_out_array.
 * idx: The index structure to keep track of calculations.
 *
 * returns: The updated index structure.
 */
index* update_theOutput(testbench tb, int** currentBuffer,  DEC_LIST dec_list, netsystemFile nfile, index* idx) {
    int counter1 = 0;

    // Iterate over components in the subnetlist
    for (int i = 0; i < nfile.subnetlist->num_components; i++) {
        int* array1 = (int*)malloc(nfile.subnetlist->components[i].numofinputs * sizeof(int));
        int counter = 0;

        // Iterate over inputs of the current component
        for (int j = 0; j < nfile.subnetlist->components[i].numofinputs; j++) {
            if (currentBuffer[i][j + 1] == -1) {
                // Exit the loop if the input is -1
                break;
            }
            counter++;
            array1[j] = currentBuffer[i][j + 1];
        }

        // Find the index in the truth table for the given inputs
        index* id = findtruthtableindex(&nfile, i, array1, counter, idx);

        if (id->result != -1) {
            // Find the appropriate truth table entry and update the nextBuffer
            for (int k = 0; k < dec_list.decnum; k++) {
                if (strcmp(dec_list.dec_list[k].name, nfile.subnetlist->components[i].type) == 0) {
                    currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1] = dec_list.dec_list[k].truth_table[id->result];

                    // Call CompleteArray function to update the currentBuffer
                    CompleteArray(currentBuffer, i, currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1], dec_list, nfile);
                }
            }
        }
        free(array1);
    }

    // Call checkarray function to perform additional calculations
    checkarray(currentBuffer, counter1, dec_list, nfile, idx);
    return idx;
}

/* 
 * Function: finalout
 * ------------------
 * Executes the final output calculation process using the given testbench and component information.
 *
 * nfile: The netsystemFile structure containing the subnetlist and new_out_array.
 * dec_list: The list of available components.
 * tb: The testbench structure.
 */
void finalout(netsystemFile nfile, DEC_LIST dec_list, testbench tb) {
    int k = -1;
    int c = 0;
    int **currentBuffer;
    int **nextBuffer;
    int **temp;

    // Allocate memory for buffers
    currentBuffer = (int **)malloc(nfile.subnetlist->num_components * sizeof(int *));
    nextBuffer = (int **)malloc(nfile.subnetlist->num_components * sizeof(int *));
    for (int i = 0; i < nfile.subnetlist->num_components; i++) {
        int columns = nfile.subnetlist->components[i].numofinputs;

        currentBuffer[i] = (int *)malloc((columns + 2) * sizeof(int));
        nextBuffer[i] = (int *)malloc((columns + 2) * sizeof(int));
    }

    // Initialize buffers
    for (int i = 0; i < nfile.subnetlist->num_components; i++) {
        for (int j = 0; j < nfile.subnetlist->components[i].numofinputs + 2; j++) {
            currentBuffer[i][j] = -1;
            nextBuffer[i][j] = -1;
        }
    }
    int total_testbench_calculator = 0;
    resultarray resarr;
    resarr.resarray = NULL;
    resarr.resnum = 0;
    double totalduration = 0.0;
    result *res;
   
        
      // Iterate over components in the subnetlist
        for (int i = 0; i < nfile.subnetlist->num_components; i++) {
            int counter = 0;
            for (int j = 0; j < dec_list.decnum; j++) {
                if (strcmp(nfile.subnetlist->components[i].type, dec_list.dec_list[j].name) == 0) {
                    k = j;
                    counter++;
                    break;
                }
            }
            if (counter == 0) {
                printf("The component: %s does not exist in the component library.\n", nfile.subnetlist->components[i].type);
            }

            // Call init_theOutputArray function to initialize the output array
            currentBuffer = init_theOutputArray( currentBuffer, i, nfile, dec_list, tb, c);
        }

    
     
    while (c < tb.maxtestbenchvalues) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        index *idx = malloc(sizeof(index));
        idx->couner = 0;
        idx->result = 0;

        res = createResult(tb.tb->num_tests, tb.outnum);

        int counter1 = 0;
        int counter2 = 0;

        // Add input and output names to the result
        for (int d = 0; d < tb.tb->num_tests; d++) {
            addName(res, tb.tb->tinput[d].input, counter1);
            counter1++;
        }

        for (int d = 0; d < tb.outnum; d++) {
            addNameOut(res, tb.out[d], counter2);
            counter2++;
        }

        // Iterate over components in the subnetlist
        for (int i = 0; i < nfile.subnetlist->num_components; i++) {
            int counter = 0;
            for (int j = 0; j < dec_list.decnum; j++) {
                if (strcmp(nfile.subnetlist->components[i].type, dec_list.dec_list[j].name) == 0) {
                    k = j;
                    counter++;
                    break;
                }
            }
            if (counter == 0) {
                printf("The component: %s does not exist in the component library.\n", nfile.subnetlist->components[i].type);
            }

            // Call init_theOutputArray function to initialize the output array
            nextBuffer = init_theOutputArray( nextBuffer, i, nfile, dec_list, tb, c+1);
        }

        // Call update_theOutput function to update the output values in currentBuffer
        int* array1 = makethearrayforInputs(tb, c);
        for(int d=0;d<res->numinput;d++){
            res->inputs[d]=array1[d];
        }
        
        idx = update_theOutput(tb, currentBuffer, dec_list, nfile, idx);

       

        int flag[res->numofout];  // Flag array to track if the condition is satisfied for each d

        // Initialize flags to 0
        for (int i = 0; i < res->numofout; i++) {
            flag[i] = 0;
        }

        // Iterate over components in the subnetlist
        for (int i = 0; i < nfile.subnetlist->num_components; i++) {
            for (int d = 0; d < res->numofout; d++) {
                // Iterate over output connections in the new_out_array
                for (int n = 0; n < nfile.new_out_array->output_num; n++) {
                    if (strcmp(nfile.new_out_array->Output_Dec_array[n].Output, res->outputname[d]) == 0) {
                        if (currentBuffer[i][0] == nfile.new_out_array->Output_Dec_array[n].origin) {
                            res->out[d] = currentBuffer[i][nfile.subnetlist->components[i].numofinputs + 1];
                            flag[d] = 1;  // Set the flag to indicate the condition is satisfied for this d
                        }
                    }
                }
            }
        }

        // Set res->out[d] to -1 for any d where the condition was not satisfied
        for (int i = 0; i < res->numofout; i++) {
            if (flag[i] == 0) {
                res->out[i] = -1;
            }
        }
         // Swap the buffers
        temp = currentBuffer;
        currentBuffer = nextBuffer;
        nextBuffer = temp;
        for (int i = 0; i < nfile.subnetlist->num_components; i++) {
        for (int j = 0; j < nfile.subnetlist->components[i].numofinputs + 2; j++) {
            
            nextBuffer[i][j] = -1;
        }
    }

        printf("Number of calculations: %d for test: %d\n", idx->couner, c + 1);
        total_testbench_calculator += idx->couner;

        // Add the result to the result array
        addResulttoArray(&resarr, *res);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double duration = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / (double)BILLION;
        totalduration += duration;
        printf("Time taken to execute in seconds: %lf for test: %d\n", duration, c + 1);
        c++;
        free(idx);
    }

    // Print the result array
    printResultArray(resarr);
    printf("Total calculations for all tests: %d\n", total_testbench_calculator);
    printf("Total time taken to execute in seconds: %lf \n", totalduration);

    // Free memory for buffers
    for (int i = 0; i < nfile.subnetlist->num_components; i++) {
        free(currentBuffer[i]);
        free(nextBuffer[i]);
    }
    free(currentBuffer);
    free(nextBuffer);
}

// Perform Depth-First Search (DFS) on the circuit starting from the given component
void dfs(int current, comp_array* circuit, bool visited[], int sorted[], int* index) {
    visited[current] = true;

    // Iterate through the connections of the current component
    for (int i = 0; i < circuit->components[current].num_connections; i++) {
        int id = circuit->components[current].connections[i];

        // Find the next component based on the connection ID
        for (int j = 0; j < circuit->num_components; j++) {
            if (id == circuit->components[j].id) {
                int next = j;

                // If the next component is not visited, recursively call dfs on it
                if (!visited[next]) {
                    dfs(next, circuit, visited, sorted, index);
                }
            }
        }
    }

    // Store the current component in the sorted array and decrement the index
    sorted[*index] = current;
    (*index)--;
}

// Perform topological sort on the circuit components
comp_array topological_sort(comp_array* circuit) {
    bool visited[MAX_COMPONENTS] = { false };
    int sorted[MAX_COMPONENTS];
    int index = circuit->num_components - 1;

    // Perform DFS on unvisited components
    for (int i = 0; i < circuit->num_components; i++) {
        if (!visited[i]) {
            dfs(i, circuit, visited, sorted, &index);
        }
    }

    // Reverse the order of the sorted array
    int start = 0;
    int end = circuit->num_components - 1;
    while (start < end) {
        int temp = sorted[start];
        sorted[start] = sorted[end];
        sorted[end] = temp;
        start++;
        end--;
    }

    // Create a new comp_array with sorted components
    comp_array sorted_circuit;
    sorted_circuit.num_components = circuit->num_components;

    for (int i = 0; i < circuit->num_components; i++) {
        sorted_circuit.components[i] = circuit->components[sorted[i]];
    }

    return sorted_circuit;
}

int main() {
    /* Open and read the component library file */
    FILE* file = fopen("component_library.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return -1;
    }

    /* Cut and store the component file into DEC_LIST structure */
    DEC_LIST dec_list = CutandStoreComponentFile(file);

    /* Parse and store the net system file */
    netsystemFile nfile;
    nfile = parseFileAndStoreNetsystem("netlist.txt");

    /* Parse and store the test bench file */
    const char* filename = "testbench.txt"; // Replace with your input file name
    testbench tb = parseFileAndStoreTestBench(filename);

    /* Perform final output using the original net system file */
    finalout(nfile, dec_list, tb);

    /* Perform topological sort on the circuit */
    comp_array circuit;
    circuit.num_components = nfile.subnetlist->num_components;
    for (int i = 0; i < circuit.num_components; i++) {
        circuit.components[i] = nfile.subnetlist->components[i];
    }
    comp_array sorted = topological_sort(&circuit);

    /* Create a new netsystemFile with the sorted circuit */
    netsystemFile nfile1;
    nfile1.subnetlist = &sorted;
    nfile1.new_out_array = nfile.new_out_array;
    printf("\n");
    printf("After the dfs algorithm:\n");
    printf("\n");

    /* Perform final output using the sorted circuit */
    finalout(nfile1, dec_list, tb);

    // Clean up memory
    free(nfile.subnetlist);
    // Free the allocated memory for the dec_list
    free(dec_list.dec_list);
    free(tb.tb);

    return 0;
}
