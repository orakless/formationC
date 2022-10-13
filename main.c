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
    Notes notes[MAX_EPREUVES];
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

// Initialization of the current node, as NULL.
node *clCurrentNode = NULL;

// Creates a node at the beginning of the linked list.
void newNode(const char szValue[MAX_CHAR]) {
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
void delNode() {
    // A pointer that points toward the current node.
    node *clNodeToDelete = clCurrentNode;

    // Sets the next node in the linked list as the current one.
    clCurrentNode = (node *)clCurrentNode->clpNext;

    // Deletes the node.
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
                memset(&szCurrentStr, 0, sizeof(char) * MAX_CHAR);
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
            if (limStr.min <= limStr.max) {
                memcpy(&szCurrentStr, &szUserInput[limStr.min],
                       (limStr.max - limStr.min)+1 * sizeof(char));
                newNode(szCurrentStr);
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
                newNode(szCurrentStr);
                memset(&szCurrentStr, 0, sizeof(char) * MAX_CHAR);
            }
        }
    }
}

// Get linked list length.
int getLinkedListLength() {
    int i = 1;
    node * clLocalCurrentNode = clCurrentNode;
    for (i; clLocalCurrentNode->clpNext != NULL; ++i) {
        clLocalCurrentNode = (node *)clLocalCurrentNode->clpNext;
    };
    return i;
}

char *getNodeValue(unsigned int uIndex) {
    node *clCurrentLocalNode = clCurrentNode;

    for (int i = 0; i < uIndex; ++i) {
        if (clCurrentLocalNode->clpNext != NULL) {
            clCurrentLocalNode = (node *) clCurrentLocalNode->clpNext;
        } else {
            return NULL;
            break;
        }
    }
    return clCurrentLocalNode->szValue;
}

float getNodeValueAsFloat(unsigned int uIndex) {
    char *szValue = getNodeValue(uIndex);
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

int getNodeValueAsInt(unsigned int uIndex) {
    return (int)getNodeValueAsFloat(uIndex);
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


bool isEpreuveNameTaken(Matiere * matiere) {
    for (int iEpreuveIndex = 0;
         iEpreuveIndex < matiere->uNbEpreuves; ++iEpreuveIndex) {
        if (strcmp(matiere->epreuves->szNom, getNodeValue(3)) == 0) {
            return true; // There's already an Epreuve called like that.
        }
    }
    return false;
}


bool areCoefCorrect(unsigned int uNbUE) {
    for (int i = 5; i < uNbUE+5; ++i) {
        // Checks if there's at least one coefficient greater than 0
        if (i == uNbUE - 1) {
            return false; // There's no coefficient greater than 0

        } else if (getNodeValue(i) > 0) {
            break;
        }
    }
    return true;
}


void newMatiere(Semestre * semestre, const char szValue[MAX_CHAR]) {
    char *szDest = semestre->matieres[semestre->uNbMatieres++].szNom;
    strcpy(szDest, szValue);
    semestre->matieres[semestre->uNbMatieres-1].uNbEpreuves = 0;
    printf("Matiere ajoutee a la formation\n");
}


void newEpreuve(Matiere * matiere, const char szValue[MAX_CHAR]) {
    char *szDest = matiere->epreuves[matiere->uNbEpreuves++].szNom;
    strcpy(szDest, szValue);
}


void newEtudiant(Promotion * promotion) {
    unsigned int uIndexEtudiant = promotion->uNbEtudiants++;
    Etudiant * nvEtudiant = &promotion->etudiants[uIndexEtudiant];

    strcpy(nvEtudiant->szNom, getNodeValue(2));
    nvEtudiant->uNbMatieres = 0;

    printf("Etudiant ajoute a la formation\n");
}


Matiere * getMatiereByName(Semestre * semestre,
                           const char szValue[MAX_CHAR],
                           bool createMatiere) {
    for (int i = 0; i < semestre->uNbMatieres; ++i) {
        if (strcmp(semestre->matieres[i].szNom, szValue) == 0) {
            return &semestre->matieres[i];
        }
    }
    if (createMatiere) {
        newMatiere(semestre, szValue);
        return &semestre->matieres[semestre->uNbMatieres - 1];
    } else {
        return NULL;
    }
}


Epreuve * getEpreuveByName(Matiere * matiere,
                           const char szValue[MAX_CHAR],
                           bool createEpreuve) {
    for (int i = 0; i < matiere->uNbEpreuves; ++i) {
        if (strcmp(matiere->epreuves[i].szNom, szValue) == 0) {
            return &matiere->epreuves[i];
        }
    }
    if (createEpreuve) {
        newEpreuve(matiere, szValue);
        return &matiere->epreuves[matiere->uNbEpreuves - 1];
    } else {
        return NULL;
    }
}


Epreuve * searchAllEpreuveByName(Semestre * semestre,
                                 const char szValue[MAX_CHAR]) {
    Epreuve * epreuve = NULL;
    for (int i = 0; i < semestre->uNbMatieres; ++i) {
        Matiere * matiere = &semestre->matieres[i];
        epreuve = getEpreuveByName(matiere, szValue, false);
        if (epreuve != NULL) break;
    }
    return NULL;
}


Etudiant * getEtudiantByName(Formation * form,
                             Promotion * promotion,
                             const char szValue[MAX_CHAR],
                             bool createEtudiant) {
    for (int i = 0; i < promotion->uNbEtudiants; ++i) {
        if (strcmp(promotion->etudiants[i].szNom, szValue) == 0) {
            return &promotion->etudiants[i];
        }
    }

    if (createEtudiant) {
        /*Etudiant *pNewEtud = &promotion->etudiants[promotions->uNbEtudiants];
        pNewEtud->uNbMatieres = 0;
        strcpy(pNewEtud->szNom, szValue);*/
        newEtudiant(promotion);
        return &promotion->etudiants[promotion->uNbEtudiants-1];
    }

    return NULL;
}


void newMatiereEtudiant(Semestre * semestre, Etudiant * etudiant) {
    unsigned int uIndexMatiere = etudiant->uNbMatieres++;
    NotesMatiere * nvMatiere = &etudiant->matieres[uIndexMatiere];

    nvMatiere->pMatiere = getMatiereByName(semestre,
                                           getNodeValue(3),
                                           0);
    nvMatiere->uNbNotes = 0;
}


void newNoteEtudiant(Epreuve * epreuve, NotesMatiere * matiere) {

}

NotesMatiere * getEtudiantMatiereByName(Semestre * semestre,
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
        newMatiereEtudiant(semestre, etudiant);
        return &etudiant->matieres[etudiant->uNbMatieres-1];
    }
    return NULL;

}


Notes * getNoteByEpreuveName(Formation * form,
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


int formation(Formation * form) {
    int uNbUE = getNodeValueAsInt(1);
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


int epreuve(Formation * form) {
    if (getNodeValueAsInt(1) > NB_SEMESTRES ||
        getNodeValueAsInt(1) < 1) {
        return 1; // Number is out of range.
    }

    if (getLinkedListLength() != 4+form->uNbUE) {
        return 2; // Missing argument.

    } else {
        int iSemestreIndex = getNodeValueAsInt(1)-1;

        if (!areCoefCorrect(form->uNbUE)) {
            return 3; // There's no coefficient greater than 0.
        }

        Matiere * matiere = getMatiereByName(&form->semestres[iSemestreIndex],
                                             getNodeValue(2), true);

        if (isEpreuveNameTaken(matiere)) {
            return 4; // There's already an Epreuve called like that.
        }

        Epreuve * epreuve = getEpreuveByName(matiere, getNodeValue(3), true);

        for (int iUE = 0; iUE < form->uNbUE; ++iUE) {
            epreuve->fCoef[iUE] = getNodeValueAsFloat(4+iUE);
        }

        return 0; // Everything went well
    }
}


int note(Formation * form, Promotion * promotion) {
    if (getNodeValueAsInt(1) < 1 || getNodeValueAsInt(2) > 2) {
        return 1;
    }

    Semestre * semestre = &form->semestres[getNodeValueAsInt(1)];

    if (getMatiereByName(semestre,
                         getNodeValue(3), 0) == NULL) {
        return 2;
    }
    if (searchAllEpreuveByName(semestre,
                               getNodeValue(4)) == NULL) {
        return 3;
    }
    if (getNodeValueAsInt(5) < NOTE_MIN || getNodeValueAsInt(5) > NOTE_MAX) {
        return 4;
    }

    Etudiant * etudiant = getEtudiantByName(form, promotion,
                                            getNodeValue(1), 1);

    if (etudiant->uNbMatieres > 0) {
        NotesMatiere * matiere = getEtudiantMatiereByName(semestre, etudiant,
                                                          getNodeValue(3),
                                                          0);
        if (matiere != NULL) {

        } else {
            matiere = getEtudiantMatiereByName(semestre, etudiant,
                                               getNodeValue(3), 1);
        }

    }

    return 0;
}

void callCommand(Formation * form) {
    //printf("You wanted to use: ");
    switch (getStringId(getNodeValue(0))) {
        case 975: // formation
            switch (formation(form)) {
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
            switch (epreuve(form)) {
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


void routine(Formation * pf, Promotion * promotion) {
    do {
        char szaUserInput[255] = {0};
        printf("$ ");

        /* %[^\n]s gets every char until it encounters a \n. The space before
         * is to circumvent an issue we can see in while loops : the
         * scanf will try to register the newline before the input area. So we
         * use a space to "refresh" the input field.
         */
        scanf(" %[^\n]s", &szaUserInput);

        parseInputDes(szaUserInput);
        if (strcmp(getNodeValue(0), "exit") == 0) {
            break;
        }

        //printf("%d\n", getLinkedListLength());

        callCommand(pf);
        delAllNodes();
    } while (1);
}


int main() {
    Formation f;
    Formation * pf = &f;

    Promotion promotion;
    Promotion * ppromotion = &promotion;

    promotion.uNbEtudiants = 0;

    f.semestres[0].uNbMatieres = 0;
    f.semestres[1].uNbMatieres = 0;
    f.uNbUE = 0;

    routine(pf, ppromotion);
}