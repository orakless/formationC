/************************************
 SAE 1.01 (C)
    Interpréteur de commande & modélisation
 d'une formation.

 Créé le 28/09/2022.

 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

enum {
    MAX_CHAR = 31,
    NB_SEMESTRES = 2,
    MIN_UE = 3,
    MAX_UE = 6,
    MAX_MATIERES = 10,
    MAX_EPREUVES = 5,
    MAX_ETUDIANTS = 100,
    NOTE_MIN = 0,
    NOTE_MAX = 20
};


/* Linked list structure.
 * Each "node" of the list is composed of :
 *      - a string, szValue, with a max size of MAX_CHAR char.
 *      - a pointer, clpNext, which points towards the next node in the list.
 */
typedef struct {
    char szValue[MAX_CHAR];
    struct node *clpNext;
} node;

typedef struct {
    int min;
    int max;
} limit;

/* Structure formation */

typedef struct {
    char szNom[MAX_CHAR];
    float fCoef[MAX_UE];
} Epreuve;

typedef struct {
    char szNom[MAX_CHAR];
    unsigned int uNbEpreuves;
    Epreuve epreuves[MAX_EPREUVES];
} Matiere;

typedef struct {
    unsigned int uNbMatieres;
    Matiere matieres[MAX_MATIERES];
} Semestre;

typedef struct {
    unsigned int uNbUE;
    Semestre semestres[NB_SEMESTRES];
} Formation;


typedef struct {
    Epreuve * pEpreuve;
    float fNote;
} Notes;

typedef struct {
    Matiere * pMatiere;
    unsigned int uNbNotes;
    Notes notes[MAX_EPREUVES*MAX_MATIERES];
} NotesMatiere;

typedef struct {
    char szNom[MAX_CHAR];
    unsigned int uNbMatieres;
    NotesMatiere matieres[MAX_MATIERES];
} Etudiant;

typedef struct {
    unsigned int uNbEtudiants;
    Etudiant etudiants[MAX_ETUDIANTS];
} Promotion;

// Creates a node at the beginning of the linked list.
void newNode(node * clCurrentNode, const char szValue[MAX_CHAR]) {
    // Allocates enough memory for the new node
    node *clNewNode = (node *) malloc(sizeof(node));

    // Cleans the area and then copies the value given by the user in the new
    // node.
    //memset(&clNewNode->szValue, 0, sizeof(char) * 30);
    strcpy(clNewNode->szValue, szValue);

    // Links the current node to the new node.
    clNewNode->clpNext = (struct node *) clCurrentNode;

    // Makes the new node the current one, so we don't lose track of it.
    clCurrentNode = clNewNode;
}

// Removes the node at the beginning of the linked list.
void delNode(node * clCurrentNode) {
    // A pointer that points toward the current node.
    node *clNodeToDelete = clCurrentNode;

    // Sets the next node in the linked list as the current one.
    clCurrentNode = (node *)clCurrentNode->clpNext;

    // Deletes the node.
    free(clNodeToDelete);
}

// Removes EVERY node in the linked list.
void delAllNodes(node * clCurrentNode) {
    node *clNextNode = NULL;
    while (clCurrentNode != NULL) {
        clNextNode = (node *)clCurrentNode->clpNext;
        delNode(clCurrentNode);
        clCurrentNode = clNextNode;
    }
}

void printNodeValues(node * clCurrentNode) {
    node *clLocalCurrentNode = clCurrentNode;
    unsigned int uNbIndex = 0;
    while (clLocalCurrentNode != NULL) {
        printf("index %d: \"%s\"\n", uNbIndex, clLocalCurrentNode->szValue);
        ++uNbIndex;
        clLocalCurrentNode = (node *) clLocalCurrentNode->clpNext;
    }
}

// Parses the input, and puts it in the linked list. (from the start)
void parseInputAsc(node * clCurrentNode, const char szUserInput[]) {
    char szCurrentStr[MAX_CHAR] = {0};
    limit limStr = {0, 1};

    for (int i=0; i < strlen(szUserInput); ++i) {
        // Cuts the string at every " " char. (32 is ' 's code ASCII point)
        if (szUserInput[i] == 32) {
            limStr.max = i;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min) * sizeof(char));
                newNode(clCurrentNode, szCurrentStr);
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
                newNode(clCurrentNode, szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * MAX_CHAR);
            }
        }
    }
}

// Parses the input, and puts it in the linked list. (from the end)
void parseInputDes(node * clCurrentNode, const char szUserInput[]) {
    char szCurrentStr[MAX_CHAR] = {0};
    // Explicit int conversion since the user will surely NOT need more than an
    // int for his input.
    limit limStr = {(int)strlen(szUserInput)-2,
                    (int)strlen(szUserInput)-1};

    for (int i = (int) strlen(szUserInput); i > -1; --i) {
        // Cuts the string at every " ". (32 is " "'s code ASCII point)
        if (szUserInput[i] == 32) {
            limStr.min = i+1;
            if (limStr.min <= limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min)+1 * sizeof(char));
                newNode(clCurrentNode, szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * MAX_CHAR);
            }
            limStr.min = i-2;
            limStr.max = i-1;

            // Checks if we're at the end of the string.
            // If true, we put the last word in the linked list.
        } else if (i == 1) {
            limStr.min = 0;
            if (limStr.min < limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min)+1 * sizeof(char));
                newNode(clCurrentNode, szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * MAX_CHAR);
            }
        }
    }
}

// Get linked list length.
int getLinkedListLength(node * clCurrentNode) {
    int i = 1;
    node * clLocalCurrentNode = clCurrentNode;
    for (i; clLocalCurrentNode->clpNext != NULL; ++i) {
        clLocalCurrentNode = (node *)clLocalCurrentNode->clpNext;
    };
    return i;
}

char *getNodeValue(node * clCurrentNode, unsigned int uIndex) {
    node *clCurrentLocalNode = clCurrentNode;

    for (int i = 0; i < uIndex; ++i) {
        if (clCurrentLocalNode->clpNext != NULL) {
            clCurrentLocalNode = (node *) clCurrentLocalNode->clpNext;
        } else {
            return NULL;
        }
    }
    return clCurrentLocalNode->szValue;
}

float getNodeValueAsFloat(node * clCurrentNode, unsigned int uIndex) {
    char *szValue = getNodeValue(clCurrentNode, uIndex);
    if (szValue == NULL) {
        return -1.f;
    }
    float fValue = 0;
    int i = 0;
    int iDotIndex = -1;

    // Get number max index, and dot index if there's a period.
    for (i; szValue[i+1] != 0; ++i) {
        if (szValue[i] == 46) { // 46 = period (".") ASCII code.
            iDotIndex = i;
        }
    }

    // if there's no period, put its index after the max index.
    if (iDotIndex == -1) iDotIndex = i+1;

    // -48 = conversion between ASCII codes and actual numbers.
    for (i; i >= 0; --i) {
        if (i > iDotIndex) {
            // multiply by 10^-1, -2... if we're before the dot
            fValue += (float)
                      ((double)(szValue[i]-48)*pow(10.f,-(i-iDotIndex)));
        } else if (i < iDotIndex) {
            // multiply by 10^1, 10^2... if we're after the dot
            fValue += (float)
                      ((szValue[i]-48)*pow(10.f, iDotIndex-1-i));
        }
    }
    return fValue;
}

int getNodeValueAsInt(node * clCurrentNode, unsigned int uIndex) {
    return (int)getNodeValueAsFloat(clCurrentNode, uIndex);
}

/*******************
 End of node struct
********************/

unsigned int getStringId(char szUserInput[]) {
    unsigned int uWeightString = 0;

    for(int i=0; i < strlen(szUserInput); i++) {
        uWeightString += szUserInput[i];
    }
    return uWeightString;
}


bool isEpreuveNameTaken(node * clCurrentNode, Matiere * matiere) {
    for (int iEpreuveIndex = 0;
         iEpreuveIndex < matiere->uNbEpreuves; ++iEpreuveIndex) {
        if (strcmp(matiere->epreuves->szNom, getNodeValue(clCurrentNode, 3)) == 0) {
            return true; // There's already an Epreuve called like that.
        }
    }
    return false;
}


bool areCoefCorrect(node * clCurrentNode, unsigned int uNbUE) {
    for (int i = 5; i < uNbUE+5; ++i) {
        // Checks if there's at least one coefficient greater than 0
        if (i == uNbUE - 1) {
            return false; // There's no coefficient greater than 0

        } else if (getNodeValue(clCurrentNode, i) > 0) {
            break;
        }
    }
    return true;
}


void newMatiere(node * clCurrentNode, Semestre * semestre, const char szValue[MAX_CHAR]) {
    char *szDest = semestre->matieres[semestre->uNbMatieres++].szNom;
    strcpy(szDest, szValue);
    semestre->matieres[semestre->uNbMatieres-1].uNbEpreuves = 0;
    printf("Matiere ajoutee a la formation\n");
}


void newEpreuve(node * clCurrentNode, Matiere * matiere, const char szValue[MAX_CHAR]) {
    char *szDest = matiere->epreuves[matiere->uNbEpreuves++].szNom;
    strcpy(szDest, szValue);
}


void newEtudiant(node * clCurrentNode, Promotion * promotion) {
    unsigned int uIndexEtudiant = promotion->uNbEtudiants++;
    Etudiant * nvEtudiant = &promotion->etudiants[uIndexEtudiant];

    strcpy(nvEtudiant->szNom, getNodeValue(clCurrentNode, 2));
    nvEtudiant->uNbMatieres = 0;

    printf("Etudiant ajoute a la formation\n");
}


Matiere * getMatiereByName(node * clCurrentNode,
                           Semestre * semestre,
                           const char szValue[MAX_CHAR],
                           bool createMatiere) {
    for (int i = 0; i < semestre->uNbMatieres; ++i) {
        if (strcmp(semestre->matieres[i].szNom, szValue) == 0) {
            return &semestre->matieres[i];
        }
    }
    if (createMatiere) {
        newMatiere(clCurrentNode, semestre, szValue);
        return &semestre->matieres[semestre->uNbMatieres - 1];
    } else {
        return NULL;
    }
}


Epreuve * getEpreuveByName(node * clCurrentNode,
                           Matiere * matiere,
                           const char szValue[MAX_CHAR],
                           bool createEpreuve) {
    for (int i = 0; i < matiere->uNbEpreuves; ++i) {
        if (strcmp(matiere->epreuves[i].szNom, szValue) == 0) {
            return &matiere->epreuves[i];
        }
    }
    if (createEpreuve) {
        newEpreuve(clCurrentNode, matiere, szValue);
        return &matiere->epreuves[matiere->uNbEpreuves - 1];
    } else {
        return NULL;
    }
}


Epreuve * searchAllEpreuveByName(node * clCurrentNode,
                                 Semestre * semestre,
                                 const char szValue[MAX_CHAR]) {
    Epreuve * epreuve = NULL;
    for (int i = 0; i < semestre->uNbMatieres; ++i) {
        Matiere * matiere = &semestre->matieres[i];
        epreuve = getEpreuveByName(clCurrentNode, matiere, szValue, false);
        if (epreuve != NULL) break;
    }
    return NULL;
}


Etudiant * getEtudiantByName(node * clCurrentNode,
                             Formation * form,
                             Promotion * promotion,
                             const char szValue[MAX_CHAR],
                             bool createEtudiant) {
    for (int i = 0; i < promotion->uNbEtudiants; ++i) {
        if (strcmp(promotion->etudiants[i].szNom, szValue) == 0) {
            return &promotion->etudiants[i];
        }
    }

    if (createEtudiant) {
        newEtudiant(clCurrentNode, promotion);
        return &promotion->etudiants[promotion->uNbEtudiants-1];
    }

    return NULL;
}


void newMatiereEtudiant(node * clCurrentNode, Semestre * semestre, Etudiant * etudiant) {
    unsigned int uIndexMatiere = etudiant->uNbMatieres++;
    NotesMatiere * nvMatiere = &etudiant->matieres[uIndexMatiere];

    nvMatiere->pMatiere = getMatiereByName(clCurrentNode, semestre,
                                           getNodeValue(clCurrentNode, 3),
                                           0);
    nvMatiere->uNbNotes = 0;
}


void newNoteEtudiant(node * clCurrentNode, Epreuve * epreuve, NotesMatiere * matiere) {
    unsigned int uIndexNote = matiere->uNbNotes++;
    Notes * note = &matiere->notes[uIndexNote];
    note->pEpreuve = epreuve;
    note->fNote = getNodeValueAsFloat(clCurrentNode, 5);
}

NotesMatiere * getEtudiantMatiereByName(node * clCurrentNode,
                                        Semestre * semestre,
                                        Etudiant * etudiant,
                                        const char szValue[MAX_CHAR],
                                        bool createMatiere) {
    Matiere * matiere = NULL;
    for (int i = 0; i < etudiant->uNbMatieres; ++i) {
        matiere = etudiant->matieres[i].pMatiere;
        if (strcmp(matiere->szNom, szValue) == 0) {
            return &etudiant->matieres[i];
        }
    }

    if (createMatiere) {
        newMatiereEtudiant(clCurrentNode, semestre, etudiant);
        return &etudiant->matieres[etudiant->uNbMatieres-1];
    }
    return NULL;

}


Notes * getNoteByEpreuveName(node * clCurrentNode,
                             Formation * form,
                             NotesMatiere * matiere,
                             const char szValue[MAX_CHAR],
                             bool createNote) {
    for (int i = 0; i < matiere->uNbNotes; ++i) {
        Epreuve * epreuve = matiere->notes[i].pEpreuve;
        if (strcmp(epreuve->szNom, szValue) == 0) {
            return &matiere->notes[i];
        }
    }
    if (createNote) {

    }
}


int formation(node * clCurrentNode, Formation * form) {
    int uNbUE = getNodeValueAsInt(clCurrentNode, 1);
    if (form->uNbUE == 0) {
        if (uNbUE < MIN_UE || uNbUE > MAX_UE) {
            return 1; // Number is out of range
        }
        form->uNbUE = uNbUE;
        return 0; // Everything went well
    } else {
        if (uNbUE == -1) {
            return -1; // Prints UE number
        } else {
            return 2; // Number of UE already defined
        }
    }
}


int epreuve(node * clCurrentNode, Formation * form) {
    if (getNodeValueAsInt(clCurrentNode, 1) > NB_SEMESTRES ||
        getNodeValueAsInt(clCurrentNode, 1) < 1) {
        return 1; // Number is out of range.
    }

    if (getLinkedListLength(clCurrentNode) != 4+form->uNbUE) {
        return 2; // Missing argument.

    } else {
        int iSemestreIndex = getNodeValueAsInt(clCurrentNode, 1)-1;

        if (!areCoefCorrect(clCurrentNode, form->uNbUE)) {
            return 3; // There's no coefficient greater than 0.
        }

        Matiere * matiere = getMatiereByName(clCurrentNode,
                                             &form->semestres[iSemestreIndex],
                                             getNodeValue(clCurrentNode, 2),
                                             true);

        if (isEpreuveNameTaken(clCurrentNode, matiere)) {
            return 4; // There's already an Epreuve called like that.
        }

        Epreuve * epreuve = getEpreuveByName(clCurrentNode, matiere,
                                             getNodeValue(clCurrentNode, 3),
                                             true);

        for (int iUE = 0; iUE < form->uNbUE; ++iUE) {
            epreuve->fCoef[iUE] = getNodeValueAsFloat(clCurrentNode, 4+iUE);
        }

        return 0; // Everything went well
    }
}


int note(node * clCurrentNode, Formation * form, Promotion * promotion) {

    // Checks if the semestre number is correct
    if (getNodeValueAsInt(clCurrentNode, 1) < 1 ||
        getNodeValueAsInt(clCurrentNode, 2) > 2) {
        return 1;
    }

    Semestre * semestre = &form->semestres[getNodeValueAsInt(clCurrentNode, 1)];

    Matiere * matiere = getMatiereByName(clCurrentNode,
                                         semestre,
                                         getNodeValue(clCurrentNode, 3),
                                         0);

    if (matiere == NULL) { // Checks if the matiere exists
        return 2;
    }

    Epreuve * epreuve = getEpreuveByName(clCurrentNode,
                                         matiere,
                                         getNodeValue(clCurrentNode, 4),
                                         0);

    if (epreuve == NULL) { // Checks if the epreuve exists
        return 3;
    }


    // Checks if the grade is between
    if (getNodeValueAsInt(clCurrentNode, 5) < NOTE_MIN ||
        getNodeValueAsInt(clCurrentNode, 5) > NOTE_MAX) {
        return 4;
    }

    Etudiant * etudiant = getEtudiantByName(clCurrentNode, form, promotion,
                                            getNodeValue(clCurrentNode, 1),
                                            1);

    if (etudiant->uNbMatieres > 0) {
        NotesMatiere * notesMatiere = getEtudiantMatiereByName(clCurrentNode,
                                                 semestre,
                                                 etudiant,
                                                 getNodeValue(clCurrentNode, 3),
                                                 0);
        if (notesMatiere != NULL) {
            Notes * note = getNoteByEpreuveName(clCurrentNode, form, notesMatiere,
                                        getNodeValue(clCurrentNode, 4),
                                      0);
            if (note != NULL) {
                return 5;
            } else {
                newNoteEtudiant(clCurrentNode,
                                getEpreuveByName(clCurrentNode, matiere,
                                               getNodeValue(clCurrentNode, 4),
                                               0),
                                notesMatiere);
            }
        } else {
            notesMatiere = getEtudiantMatiereByName(clCurrentNode, semestre, etudiant,
                                               getNodeValue(clCurrentNode, 3),
                                               1);
        }
    }

    return 0;
}

void callCommand(node * clCurrentNode, Formation * form, Promotion * promotion) {
    switch (getStringId(getNodeValue(clCurrentNode, 0))) {
        case 975: // formation
            switch (formation(clCurrentNode, form)) {
                case -1:
                    printf("Le nombre d'UE est de : %d", form->uNbUE);
                    break;
                case 0:
                    printf("Le nombre d'UE est defini");
                    break;
                case 1:
                    printf("Le nombre d'UE est incorrect");
                    break;
                case 2:
                    printf("Le nombre d'UE est deja defini");
            }
            break;
        case 764: // epreuve
            switch (epreuve(clCurrentNode, form)) {
                case 0:
                    printf("Epreuve ajoutee a la formation");
                    break;
                case 1:
                    printf("Le numero de semestre est incorrect");
                    break;
                case 2:
                case 3:
                    printf("Au moins un des coefficients est incorrect");
                    break;
                case 4:
                    printf("Une meme epreuve existe deja");
                    break;
            }
            break;
        case 1266: // coefficients
            printf("coefficients");
            break;
        case 438: // note
            note(clCurrentNode, form, promotion);
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


void routine(node * clCurrentNode, Formation * pf, Promotion * promotion) {
    do {
        char szaUserInput[255] = {0};
        printf("$ ");

        /* %[^\n]s gets every char until it encounters a \n. The space before
         * is to circumvent an issue we can see in while loops : the
         * scanf will try to register the newline before the input area. So we
         * use a space to "refresh" the input field.
         */
        scanf(" %[^\n]s", &szaUserInput);

        parseInputDes(clCurrentNode, szaUserInput);
        if (strcmp(getNodeValue(clCurrentNode, 0), "exit") == 0) {
            break;
        }

        //printf("%d\n", getLinkedListLength());

        callCommand(clCurrentNode, pf, promotion);
        delAllNodes(clCurrentNode);
    } while (1);
}


int main() {
    // Initialization of the current node, as NULL.
    node *clCurrentNode = NULL;

    Formation f;
    Formation * pf = &f;

    Promotion promotion;
    Promotion * ppromotion = &promotion;

    promotion.uNbEtudiants = 0;

    f.semestres[0].uNbMatieres = 0;
    f.semestres[1].uNbMatieres = 0;
    f.uNbUE = 0;

    routine(clCurrentNode, pf, ppromotion);
}