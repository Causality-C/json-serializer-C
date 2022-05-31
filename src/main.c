#include <stdio.h>
#include <stdlib.h>
#include "serializer.h"
#include <string.h>
#define ARGS 2
/**
 * Inputs: a file containing a struct 
 * Output: formatted string that can be written as JSON
 */
int main(int argc, char * argv[]){
    // Need Correct Number of args
    if(argc != ARGS){
        printf("The number of arguments supplied is invalid\n");
        exit(1);
    }

    // Read File
    FILE * fptr; 
    if((fptr = fopen(argv[1], "r+"))== NULL){
        printf("The file \"%s\" was not found\n", argv[1]);
        exit(1);
    }
    // Scan for word "struct": we assume that there is at most one struct in a file for now
    char buf[256] = {0x0};
    while(fptr && fgets(buf, sizeof(buf),fptr)){
        // Find the occurance of struct
        if(strstr(buf,"struct")){
            // Search within the struct for fields
            int numFields = 0;
            long currPos = ftell(fptr);
            long bufSize = 0;

            // Find long this whole thing is
            while(fgets(buf,sizeof(buf),fptr) && !strstr(buf,"}")){
                bufSize+= strlen(buf);
            }
            // Rewind and allocate new buffer which is bufSize + 1 bc null termination
            // https://stackoverflow.com/questions/41928450/c-strtok-is-creating-an-invalid-read-of-size-1-unable-to-freetoken
            fseek(fptr,currPos,SEEK_SET);
            char * structFields = malloc((bufSize + 1) * sizeof(char));
            fread(structFields,bufSize,1,fptr);

            // Null Terminate the buffer 
            structFields[bufSize] = '\0'; 
            StructRep * rep = getStructRepresentation(structFields);

            // Get the JSON expression for the Struct
            char * jsonRep = getJSONFromStruct(rep);

            // Update and Save Current Position
            fgets(buf,sizeof(buf),fptr);
            currPos = ftell(fptr);
            fseek(fptr,0,SEEK_SET);

            // Get Contents at Beginning of File
            char * start = malloc(currPos * sizeof(char));
            fread(start,currPos,1,fptr);
            fseek(fptr,0,SEEK_END);
            long endPos = ftell(fptr);
            fseek(fptr,currPos,SEEK_SET);

            // GET new constant
            char * con = "#define _JSON_SERIALIZATION ";
            char * mid = combineMultipleStrings(5,"\n",con, "\"",jsonRep,"\"\n");

            // Get Contents after Beginning of File
            char * end = malloc((endPos - currPos) * sizeof(char));
            fread(end,endPos-currPos,1,fptr);

            // Inject the representation in the header file
            FILE * fptr_new;
            char * file_name = combineMultipleStrings(2,"_",argv[1]);

            if((fptr_new = fopen(file_name, "w+")) == NULL){
                printf("The file \"%s\" was not found\n", file_name);
                exit(1);
            }
            fwrite(start,currPos,1,fptr_new);
            fwrite(mid,strlen(mid),1,fptr_new);
            fwrite(end,endPos-currPos,1,fptr_new);

            fclose(fptr_new);

            // We assume one struct for now
            free(jsonRep);
            freeStructRep(rep);
            free(structFields);
            free(start);
            free(file_name);
            free(mid);
            free(end);
            break;
        }
    }
    fclose(fptr);
}