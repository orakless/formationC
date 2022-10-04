#include <stdio.h>
#include <string.h>
#include <malloc.h>

enum {
    MAX_CHAR = 30,
    NB_SEMESTRES = 2,
    MIN_UE = 3,
    MAX_UE = 6,
    MAX_MATIERES = 10,
    MAX_EPREUVES = 5,
    MAX_ETUDIANTS = 100
};


/* Linked list structure.
 * Each "node" of the list is composed of :
 *      - a string, with a max size of 30 char.
 *      - a pointer, which points towards the next node in the list.
 */
typedef struct {
    char szValue[MAX_CHAR];
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
void newNode(const char szValue[MAX_CHAR]) {
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
    char szCurrentStr[MAX_CHAR] = {0};
    limit limStr = {0, 1};

    for (int i=0; i < strlen(szUserInput); ++i) {
        // Cuts the string at every " " char. (32 is ' 's code ASCII point)
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
    char szCurrentStr[MAX_CHAR] = {0};
    // Explicit int conversion since the user will surely NOT need more than an
    // int for his input.
    limit limStr = {(int)strlen(szUserInput)-2,
                    (int)strlen(szUserInput)-1};

    for (int i = (int) strlen(szUserInput); i > -1; --i) {
        // Cuts the string at every " ". (32 is " "'s code ASCII point)
        if (szUserInput[i] == 32) {
            limStr.min = i+1;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min) * sizeof(char));
                newNode(szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * 30);
            }
            limStr.min = i;
            limStr.max = i+1;

            // Checks if we're at the end of the string.
            // If true, we put the last word in the linked list.
        } else if (i == 1) {
            limStr.min = 0;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min)+1 * sizeof(char));
                newNode(szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * 30);
            }
        }
    }
}

char * getNodeValue(unsigned int uIndex) {
    node *clCurrentLocalNode = clCurrentNode;
    for (int i = 0; i <= uIndex; ++i) {
        if (clCurrentLocalNode->clpNext != NULL) {
            clCurrentLocalNode = (node *) clCurrentLocalNode->clpNext;
        } else break;
    }
    return clCurrentLocalNode->szValue;
}

unsigned int getStringId(char szUserInput[]) {
    unsigned int uWeightString = 0;

    for(int i=0; i<strlen(szUserInput); i++) {
        uWeightString += szUserInput[i];
    }

    return uWeightString;
}


void callCommand() {
    printf("You wanted to use: ");
    switch (getStringId(getNodeValue(0))) {
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
    printf("\n");
}

void getUserInput(char szaDest[]) {
    scanf("%s", szaDest);
}

void routine() {
    do {
        printf("$ ");
        char szaUserInput[255] = {0};
        getUserInput(szaUserInput);
        parseInputDes(szaUserInput);
        if (strcmp(getNodeValue(0), "exit") == 0) {
            break;
        }
        callCommand();
        delAllNodes();
    } while (1);
}

int main() {
    routine();
}