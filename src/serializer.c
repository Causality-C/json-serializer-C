#include "serializer.h"

StructRep * getStructRepresentation(char * structFields){
    // Strtok and split by ";"
    char * token;
    char * end_str;
    token = __strtok_r(structFields,";", &end_str);
    struct Member * members[20];
    int membersi = 0;

    while(token!=NULL){
        char * end_cpy;
        char * token_two = __strtok_r(token," ", &end_cpy);
        struct Member * mem = malloc(1 * sizeof(struct Member));
        char * args[2];
        int argsi = 0;
        while(token_two!=NULL){
            // TODO: add more parsing possibilities
            if(argsi < 2 && strcmp(token_two,"\n")){
                args[argsi++] = token_two;
            }
            token_two = __strtok_r(NULL," ", &end_cpy);
        }
        // Make sure we dont just read a "\n"
        if(argsi == 2){
            mem->type = getDataTypeFromString(args[0]);
            // Is an Array
            if(strstr(args[1],"[")){
                // Strip the arg 
                char * args_updated = strchr(args[1],'[');
                int diff = args_updated - args[1];
                mem->value = malloc((diff + 1) * sizeof(char));
                strncpy(mem->value,args[1],diff);
                mem->value[diff] = '\0';

                // TODO: Strip the number of entries in array 
                mem->isArray = 1;
            }
            else{
                mem->value = strdup(args[1]);
                mem->isArray = 0;
            }
            members[membersi++] = mem;
        }
        // We dont use the Member so free it
        else{
            free(mem);
        }
        token = __strtok_r(NULL,";", &end_str);
    }
    // Create a representation
    StructRep * rep = malloc(sizeof(StructRep));
    rep->numFields = membersi;
    rep->members = malloc(membersi * sizeof(struct Member *));
    for(int i = 0; i< membersi; i++){
        rep->members[i] = members[i];
    }
    return rep;
}

char * getJSONFromStruct(StructRep * rep){
    // Determine how many characters are needed
    int formatLength = 0;
    // Brackets at front and new line and new tab
    formatLength += 5;
    for(int i = 0; i< rep->numFields; i++){
        struct Member * mem = rep->members[i];
        formatLength += strlen(mem->value);

        // Add the formatter which should vary based on type
        formatLength += strlen(getFormatter(mem));

        // Add the Comma (if not end), Colon, New Line, Tab (if not end), Quotes for field
        formatLength += (i != rep->numFields - 1) ? 10: 7;
    }
    // Brackets at end + null termination
    formatLength += 3;

    // Alloc String and Reset formatLength
    char * ret = malloc(formatLength * sizeof(char));
    formatLength = 0;
    appendToString(&formatLength,"{\\n\\t",ret);
    for(int i = 0; i< rep->numFields; i++){
        struct Member * mem = rep->members[i];
        // Add Key with Quotes 
        appendToString(&formatLength, "\\\"",ret);
        appendToString(&formatLength, mem->value, ret);
        appendToString(&formatLength, "\\\"",ret);

        // Add Colon
        ret[formatLength++] = ':';
        char * format = getFormatter(mem);
        appendToString(&formatLength, format, ret);
        (i != rep->numFields - 1 ) ? appendToString(&formatLength,",\\n\\t",ret): appendToString(&formatLength,"\\n",ret);
    }
    ret[formatLength++] = '}';
    ret[formatLength++] = '\0';

    return ret;
}

void printStructRepresentation(StructRep * rep){
    printf("StructRep\nFields:%d\n", rep->numFields);
    for(int i = 0; i< rep->numFields; i++){
        printf("Field %d: %d-> %s\n",i,rep->members[i]->type, rep->members[i]->value);
    }
}

void freeStructRep(StructRep * rep){
    for(int i = 0; i< rep->numFields; i++){
        free(rep->members[i]->value);
        free(rep->members[i]);
    }
    free(rep->members);
    free(rep);
}

char * getFormatter(struct Member * mem){
    switch(mem->type){
        case CHAR:
            return mem->isArray ? "\\\"%s\\\"": "\\\'%c\\\'";
            break;
        case INT:
            return mem->isArray ? "[%s]": "%d";
            break;
        case FLOAT:
            return mem->isArray ? "[%s]": "%f";
        default:
            return "";
            break;
    }
}

DataTypes_t getDataTypeFromString(char * str){
    if(!strcmp(str,"char")){
        return CHAR;
    }
    else if(!strcmp(str,"int")){
        return INT;
    }
    else if(!strcmp(str,"float")){
        return FLOAT;
    }
    else{
        return UNKNOWN;
    }

}
// This may not be safe lol
void appendToString(int * ptr, char * src, char * dest){
    for(int i = 0; i< strlen(src); i++){
        dest[(*ptr)++] = src[i];
    }
}

/**
 * Combines multiple string arguments into one string
 * @param ... : char pointers only
 * @return char* 
 */
char * combineMultipleStrings(int num, ...){
    // Create References to all strings
    char ** pointers;
    pointers = malloc(num * sizeof(char *));
    int pind = 0;
    int numChars = 0;

    // Use the variable number of args 
    va_list valist;
    va_start(valist,num);
    for(int i = 0; i< num; i++){
        char * p = va_arg(valist,char *);
        pointers[pind++] = p;
        numChars += strlen(p);
    }
    va_end(valist);
    // Termination
    numChars++;
    char * ret = malloc(numChars * sizeof(char));
    int retind = 0;
    for(int i = 0; i< num; i++){
        char * p = pointers[i];
        for(int j = 0; j< strlen(p); j++){
            ret[retind++] = p[j];
        }
    }
    ret[retind++] = '\0';
    free(pointers);
    return ret;
}