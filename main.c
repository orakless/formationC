#include <stdio.h>
#include <string.h>
#include <malloc.h>

enum {
    iSzMax = 30,
    iUEMax = 6,
    iSemMax = 2
};

typedef struct {
    char szValue[iSzMax];
    struct node *clpNext;
} node;


typedef struct {
    int min;
    int max;
} limit;


// Initialization of the last and of the current node, as NULL.
node *clHeadNode = NULL;
node *clCurrentNode = NULL;

// Creates a node at the beginning of the linked list.
void newNode(const char szValue[iSzMax]) {
    node *clNewNode = (node *)malloc(sizeof(node));
    node *clPrevNode = clCurrentNode;

    strcpy(clNewNode->szValue, szValue);
    clNewNode->clpNext = (struct node *)clPrevNode;
    clCurrentNode = clNewNode;
}

// Removes the node at the beginning of the linked list.
void delNode() {
    node *clNodeToDelete = clCurrentNode;
    clCurrentNode = (node *)clCurrentNode->clpNext;
    free(clNodeToDelete);
}

// Removes EVERY node in the linked list.
void delAllNodes() {
    node *clNextNode = NULL;
    while (clCurrentNode != NULL) {
        clNextNode = (node *)clCurrentNode->clpNext;
        delNode();
        clCurrentNode = clNextNode;
    }
}

void printNodeValues() {
    node *clLocalCurrentNode = clCurrentNode;
    unsigned int uNbIndex = 0;
    while (clLocalCurrentNode != NULL) {
        printf("index %d: \"%s\"\n", uNbIndex, clLocalCurrentNode->szValue);
        ++uNbIndex;
        clLocalCurrentNode = (node *) clLocalCurrentNode->clpNext;
    }
}

// Parses the input, and puts it in the linked list. (from the start)
void parseInputAsc(const char szUserInput[]) {
    char szCurrentStr[iSzMax] = {0};
    int i = 0;
    limit limStr = {0, 1};

    for (i=0; i < strlen(szUserInput); ++i) {
        // Cuts the string at every " " char. (32 is " "'s code ASCII point)
        if (szUserInput[i] == 32) {
            limStr.max = i;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min) * sizeof(char));
                newNode(szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * 30);
            }
            limStr.min = i+1;
            limStr.max = i+2;

            // Checks if we're at the end of the string.
            // If true, we put the last word in the linked list.
        } else if (i == strlen(szUserInput)-1) {
            limStr.max = i+1;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min) * sizeof(char));
                newNode(szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * 30);
            }
        }
    }
}


// Parses the input, and puts it in the linked list. (from the end)
void parseInputDes(const char szUserInput[]) {
    char szCurrentStr[iSzMax] = {0};
    // Explicit int conversion since the user will surely NOT need more than a
    // int for his input.
    int i = (int) strlen(szUserInput);
    limit limStr = {(int)strlen(szUserInput)-2,
                    (int)strlen(szUserInput)-1};

    for (i; i > -1; --i) {
        // Cuts the string at every " ". (32 is " "'s code ASCII point)
        if (szUserInput[i] == 32) {
            limStr.min = i+1;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min) * sizeof(char));
                newNode(szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * 30);
            }
            limStr.min = i-1;
            limStr.max = i;

            // Checks if we're at the end of the string.
            // If true, we put the last word in the linked list.
        } else if (i == 1) {
            limStr.min = 0;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min) * sizeof(char));
                newNode(szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * 30);
            }
        }
    }
}


unsigned int getStringId(char szUserInput[]) {
    int i = 0;
    unsigned int uWeightString = 0;

    for(i=0; i<strlen(szUserInput); i++) {
        uWeightString += szUserInput[i];
    }

    return uWeightString;
}


void getCommand() {
    switch (getStringId(clCurrentNode->szValue)) {
        case 975: // formation
            printf("formation");
            break;
        case 764: // epreuve
            printf("epreuve");
            break;
        case 1151: // coefficient
            printf("coefficient");
            break;
        case 438: // note
            printf("note");
            break;
        case 553: // notes
            printf("notes");
            break;
        case 643: // releve
            printf("releve");
            break;
        case 846: // decision
            printf("decision");
            break;
        default: // unrecognized
            printf("This command doesn't exist.");
            break;
    }
}

/*
void checkCommand(char szTest[]) {
    unsigned int iCommandID = getStringId(szTest);
    printf("%s ID: %u\n", szTest, iCommandID);
}*/

int main() {
    parseInputDes("formatioddn 3 1 test  def de 1 2 ecole www 145 =!d");
    getCommand();
    delAllNodes();
}