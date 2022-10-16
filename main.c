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
#include <stdarg.h>

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
} Note;

typedef struct {
    Matiere * pMatiere;
    unsigned int uNbNotes;
    Note notes[MAX_EPREUVES*MAX_MATIERES];
} NotesMatiere;

typedef struct {
    unsigned int uNbMatieres;
    NotesMatiere matieres[MAX_MATIERES];
} NotesSemestre;

typedef struct {
    char szNom[MAX_CHAR];
    NotesSemestre semestres[NB_SEMESTRES];
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

__attribute__((unused)) void printNodeValues() {
    node *clLocalCurrentNode = clCurrentNode;
    unsigned int uNbIndex = 0;
    while (clLocalCurrentNode != NULL) {
        printf("index %d: \"%s\"\n", uNbIndex, clLocalCurrentNode->szValue);
        ++uNbIndex;
        clLocalCurrentNode = (node *) clLocalCurrentNode->clpNext;
    }
}

// Parses the input, and puts it in the linked list. (from the start)
__attribute__((unused)) void parseInputAsc(const char szUserInput[]) {
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
    }
    return i;
}

char * getNodeValue(unsigned int uIndex) {
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

float getNodeValueAsFloat(unsigned int uIndex) {
    char *szValue = getNodeValue(uIndex);
    if (szValue == NULL || szValue[0] == 45) { // 45 = "-"
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
            fValue += (float)(szValue[i]-48)*powf(10.f,(float)-(i-iDotIndex));
        } else if (i < iDotIndex) {
            // multiply by 10^1, 10^2... if we're after the dot
            fValue += (float)(szValue[i]-48)*powf(10.f, (float)(iDotIndex-1-i));
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


bool areCoefCorrect(unsigned int uNbUE) {
    for (int i = 4; i < uNbUE+4; ++i) {
        // Checks if there's at least one coefficient greater than 0
        if (getNodeValueAsFloat(i) > 0) {
            break;
        } else if (i == uNbUE+4-1 || getNodeValueAsFloat(i) < 0) {
            return false; // There's no coefficient greater than 0
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
    nvEtudiant->semestres[0].uNbMatieres = 0;
    nvEtudiant->semestres[1].uNbMatieres = 0;

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
    }
    return NULL;
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
    }
    return NULL;
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


Etudiant * getEtudiantByName(Promotion * promotion,
                             const char szValue[MAX_CHAR],
                             bool createEtudiant) {
    for (int i = 0; i < promotion->uNbEtudiants; ++i) {
        if (strcmp(promotion->etudiants[i].szNom, szValue) == 0) {
            return &promotion->etudiants[i];
        }
    }

    if (createEtudiant) {
        newEtudiant(promotion);
        return &promotion->etudiants[promotion->uNbEtudiants-1];
    }

    return NULL;
}


void newMatiereEtudiant(Semestre * semestre, NotesSemestre * etudiant) {
    unsigned int uIndexMatiere = etudiant->uNbMatieres++;
    NotesMatiere * nvMatiere = &etudiant->matieres[uIndexMatiere];

    nvMatiere->pMatiere = getMatiereByName(semestre,
                                           getNodeValue(3),
                                           0);
    nvMatiere->uNbNotes = 0;
}


void newNoteEtudiant(Epreuve * epreuve, NotesMatiere * matiere, float fNote) {
    unsigned int uIndexNote = matiere->uNbNotes++;
    Note * note = &matiere->notes[uIndexNote];
    note->pEpreuve = epreuve;
    note->fNote = fNote;
}


NotesMatiere * getEtudiantMatiereByName(Semestre * semestre,
                                        NotesSemestre * nSemestre,
                                        const char szValue[MAX_CHAR],
                                        bool createMatiere) {
    Matiere * matiere = NULL;
    for (int i = 0; i < nSemestre->uNbMatieres; ++i) {
        matiere = nSemestre->matieres[i].pMatiere;
        if (strcmp(matiere->szNom, szValue) == 0) {
            return &nSemestre->matieres[i];
        }
    }

    if (createMatiere) {
        newMatiereEtudiant(semestre, nSemestre);
        return &nSemestre->matieres[nSemestre->uNbMatieres-1];
    }
    return NULL;

}


Note * getNoteByEpreuveName(Formation * form,
                             NotesMatiere * matiere,
                             const char szValue[MAX_CHAR],
                             bool createNote) {
    Epreuve * epreuve = NULL;
    for (int i = 0; i < matiere->uNbNotes; ++i) {
        epreuve = matiere->notes[i].pEpreuve;
        if (strcmp(epreuve->szNom, szValue) == 0) {
            return &matiere->notes[i];
        }
    }

    if (createNote) {
        newNoteEtudiant(epreuve, matiere, getNodeValueAsFloat(5));
        return &matiere->notes[matiere->uNbNotes - 1];
    }

    return NULL;
}


bool checkNotes(Semestre * semestre, NotesSemestre * nSemestre) {
    if (nSemestre->uNbMatieres == semestre->uNbMatieres) {
        for (int i = 0; i < nSemestre->uNbMatieres; ++i) {
            Matiere * matiere = &semestre->matieres[i];
            NotesMatiere * notesMatiere = &nSemestre->matieres[i];
            if (notesMatiere->uNbNotes != matiere->uNbEpreuves) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}


int max(int i1, int i2) {
    return (i1 < i2) ? i2
                     : i1;
}


int getLongestMatiereName(Semestre * semestre) {
    int iLength = 0;
    for (int i = 0; i < semestre->uNbMatieres; ++i) {
        Matiere * matiere = &semestre->matieres[i];
        iLength = max(iLength, (int)strlen(matiere->szNom));
    }
    return iLength;
}


float trimNumber(float fNb) {
    return floorf(fNb*10)/10;
}


float getMoyenneMatiere(NotesMatiere * nMatiere, unsigned int uIndexUE,
                        bool trim) {
    float divid = 0, divis = 0;
    for (int i = 0; i < nMatiere->uNbNotes; ++i) {
        divid += nMatiere->notes[i].fNote*nMatiere->notes[i]
                .pEpreuve->fCoef[uIndexUE];
        divis += nMatiere->notes[i].pEpreuve->fCoef[uIndexUE];
    }
    if (divis == 0) {
        return -1.f;
    }
    if (trim) {
        return trimNumber(divid / divis);
    } else {
        return divid / divis;
    }
}

float getMoyenneUE(NotesSemestre * nSemestre, unsigned int uIndexUE, bool trim) {
    float iDivid = 0;
    float iDivis = 0;

    for (int iIndMat = 0; iIndMat < nSemestre->uNbMatieres; ++iIndMat) {
        NotesMatiere * nMatiere = &nSemestre->matieres[iIndMat];
        for (int iIndNote = 0; iIndNote < nMatiere->uNbNotes; ++iIndNote) {
            Note * note = &nMatiere->notes[iIndNote];
            iDivid += note->fNote * note->pEpreuve->fCoef[uIndexUE];
            iDivis += note->pEpreuve->fCoef[uIndexUE];
        }
    }
    if (iDivis == 0) {
        return -1.f;
    }
    if (trim) {
        return trimNumber(iDivid / iDivis);
    } else {
        return iDivid / iDivis;
    }

}


void printUELine(unsigned int iMaxName, unsigned int uNbUE) {
    for (int i = 0; i <= iMaxName; ++i) {
        printf(" ");
    }
    for (int i = 1; i <= uNbUE; ++i) {
        printf(" UE%d ", i);
    }
    printf("\n");
}


void printReleveLine(Matiere * matiere, NotesMatiere * nMatiere,
                     unsigned int iMaxName, unsigned int uNbUE) {
    printf("%s", matiere->szNom);
    for (int i = 0; i <= iMaxName - strlen(matiere->szNom); ++i) {
        printf(" ");
    }
    for (int i = 0; i < uNbUE; ++i) {
        float iMoyenne = getMoyenneMatiere(nMatiere, i, true);
        if (iMoyenne != -1) {
            if (iMoyenne < 10) {
                printf(" ");
            }
            printf("%0.1f ", iMoyenne);
        } else {
            printf("  ND ");
        }
    }
    printf("\n");
}


void printMoyenneUELine(NotesSemestre * nSemestre, unsigned int iMaxName,
                        unsigned int uNbUE, const char szTitle[]) {
    printf("%s", szTitle);
    for (int i = 0; i <= iMaxName - 8; ++i) {
        printf(" ");
    }
    for (int iUE = 0; iUE < uNbUE; ++iUE) {
        float fMoyenne = getMoyenneUE(nSemestre, iUE, true);
        if (fMoyenne < 10) {
            printf(" ");
        }
        printf("%0.1f ", fMoyenne);
    }
}


void printMoyenneSem(Etudiant * etudiant, bool isUEValidated[MAX_UE]) {
    printf("Moyennes annuelles ");
    for (int iIndUE = 0; iIndUE < 2; ++iIndUE) {
        NotesSemestre * nSemestre1 = &etudiant->semestres[0];
        NotesSemestre * nSemestre2 = &etudiant->semestres[1];
        float fMoySem = trimNumber((getMoyenneUE(nSemestre1, iIndUE, false)
                        +getMoyenneUE(nSemestre2, iIndUE, false))/2);
        if (fMoySem < 10) {
            printf(" ");
        }
        printf("%0.1f ", fMoySem);
        isUEValidated[iIndUE] = (fMoySem > 10) ? true
                                               : false;
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


int epreuve(Formation * form, int iIndSemestre, char szMatiereName[MAX_CHAR],
            char szEpreuveName[MAX_CHAR], float coef, ...) {
    va_list coefficients;
    float coefs = coef;

    if (iIndSemestre > NB_SEMESTRES ||
        iIndSemestre < 1) {
        return 1; // Number is out of range.
    }

    Semestre * semestre = &form->semestres[iIndSemestre-1];

    /*
    if (getLinkedListLength() != 4+form->uNbUE) {
        return 2; // Missing argument.
    }*/

    Matiere * matiere = getMatiereByName(semestre,szMatiereName, false);

    Epreuve * epreuve = (matiere == NULL) ? NULL
            : getEpreuveByName(matiere,szEpreuveName, false);

    va_start(coefficients, coef);

    if (epreuve != NULL) {
        va_end(coefficients);
        return 4;
    } else {
        if (!areCoefCorrect(form->uNbUE)) {
            va_end(coefficients);
            return 3; // There's no coefficient greater than 0.
        } else {
            if (matiere == NULL) {
                newMatiere(semestre, szMatiereName);
                matiere = &semestre->matieres[semestre->uNbMatieres-1];
            }
            newEpreuve(matiere, szEpreuveName);
            epreuve = &matiere->epreuves[matiere->uNbEpreuves-1];
        }
    }

    for (int iUE = 0; iUE < form->uNbUE; ++iUE) {
        epreuve->fCoef[iUE] = getNodeValueAsFloat(4+iUE);
    }
    va_end(coefficients);
    return 0; // Everything went well
}


int coefficients(Formation * form, unsigned int iIndSemestre) {
    if (iIndSemestre < 1 || iIndSemestre > 2) {
        return 1;
    }

    float fCoefs[MAX_UE] = {0};
    unsigned int iCountEpreuve = 0;

    Semestre * semestre = &form->semestres[iIndSemestre-1];


    for (int iM = 0; iM < semestre->uNbMatieres; ++iM) {
        Matiere * matiere = &semestre->matieres[iM];
        for (int iE = 0; iE < matiere->uNbEpreuves; ++iE) {
            ++iCountEpreuve;
            Epreuve * epreuve = &matiere->epreuves[iE];
            for (int iUE = 0; iUE < form->uNbUE; ++iUE) {
                fCoefs[iUE] += epreuve->fCoef[iUE];
            }
        }
    }

    if (iCountEpreuve == 0 || semestre->uNbMatieres == 0) {
        return 2;
    }

    for (int iC = 0; iC < form->uNbUE; ++iC) {
        if (fCoefs[iC] == 0) {
            return 3;
        }
    }

    return 0;
}


int note(Formation * form, Promotion * promotion, int iIndSemestre,
         char szEtudiantName[MAX_CHAR], char szMatiereName[MAX_CHAR],
         char szEpreuveName[MAX_CHAR], float iNote) {

    // Checks if the semestre number is correct
    if (iIndSemestre < 1 ||
        iIndSemestre > 2) {
        return 1;
    }

    Semestre * semestre = &form->semestres[iIndSemestre-1];

    Matiere * matiere = getMatiereByName(semestre,
                                         szMatiereName,
                                         0);

    if (matiere == NULL) { // Checks if the matiere exists
        return 2;
    }

    Epreuve * epreuve = getEpreuveByName(matiere,
                                         szEpreuveName,
                                         0);

    if (epreuve == NULL) { // Checks if the epreuve exists
        return 3;
    }


    // Checks if the grade is between
    if (iNote < NOTE_MIN ||
        iNote > NOTE_MAX) {
        return 4;
    }

    Etudiant * etudiant = getEtudiantByName(promotion,
                                            szEtudiantName,
                                            1);


    NotesSemestre * nSemestre = &etudiant->semestres[iIndSemestre-1];

    NotesMatiere * notesMatiere = getEtudiantMatiereByName(
                                                 semestre,
                                                 nSemestre,
                                                 szMatiereName,
                                                 0);

    if (notesMatiere != NULL) {
        Note * note = getNoteByEpreuveName(form, notesMatiere,
                                    szEpreuveName,
                                  0);
        if (note != NULL) {
            return 5;
        } else {
            newNoteEtudiant(getEpreuveByName(matiere,
                                           szEpreuveName,
                                           0),
                            notesMatiere,
                            getNodeValueAsFloat(5));
        }
    } else {
        notesMatiere = getEtudiantMatiereByName(semestre, nSemestre,
                                           szMatiereName,
                                           1);
        newNoteEtudiant(getEpreuveByName(matiere,
                                         szEpreuveName,
                                         0),
                                         notesMatiere,
                        getNodeValueAsFloat(5));
    }

    return 0;
}


int notes(Formation * form, Promotion * promotion, unsigned int iIndSemestre,
          char szEtudiantName[MAX_CHAR]) {
    if (iIndSemestre < 1 || iIndSemestre > 2) {
        return 1;
    }

    Semestre * semestre = &form->semestres[iIndSemestre-1];

    Etudiant * etudiant = getEtudiantByName(promotion, szEtudiantName,
                                            false);

    if (etudiant == NULL) {
        return 2;
    }

    NotesSemestre * nSemestre = &etudiant->semestres[iIndSemestre-1];

    if (!checkNotes(semestre, nSemestre)) {
        return 3;
    }

    return 0;
}


int releve(Formation * form, Promotion * promotion, int iIndSemestre) {
    if (iIndSemestre < 1 || iIndSemestre > 2) {
        return 1;
    }

    Semestre * semestre = &form->semestres[iIndSemestre-1];

    Etudiant * etudiant = getEtudiantByName(promotion, getNodeValue(2)
                                            , 0);

    NotesSemestre * nSemestre = &etudiant->semestres[iIndSemestre-1];

    if (etudiant == NULL) return 2;

    if (coefficients(form, iIndSemestre) != 0) {
        return 3;
    }

    if (!checkNotes(semestre, nSemestre)) {
        return 4;
    }

    unsigned int iMaxName = max(getLongestMatiereName(semestre), 8);

    printUELine(iMaxName, form->uNbUE);
    for (int i = 0; i < semestre->uNbMatieres; ++i) {
        Matiere * matiere = &semestre->matieres[i];
        printReleveLine(matiere,
                        getEtudiantMatiereByName(semestre, nSemestre,
                                                 matiere->szNom, false),
                        iMaxName, form->uNbUE);
    }
    printf("--\n");
    printMoyenneUELine(nSemestre, iMaxName, form->uNbUE, "Moyennes");

    return 0;
}


int decision(Formation * form, Promotion * promotion,
             char szEtudiantName[]) {
    Etudiant * etudiant = getEtudiantByName(promotion, szEtudiantName, false);

    if (etudiant == NULL) {
        return 1;
    }

    if (coefficients(form, 1) != 0 || coefficients(form, 1)) {
        return 2;
    }

    if (notes(form, promotion, 1, szEtudiantName) != 0 ||
        notes(form, promotion, 2, szEtudiantName) != 0) {
        return 3;
    }

    printUELine(16, form->uNbUE);

    for (int iIndSem = 0; iIndSem < 2; ++iIndSem) {
        NotesSemestre *nSemestre = &etudiant->semestres[iIndSem];
        char szSemestre[] = "S ";
        szSemestre[1] = (char) iIndSem+49;
        printMoyenneUELine(nSemestre, 16, form->uNbUE, szSemestre);
        printf("\n");
    }
    printf("--\n");

    bool isUEValidated[MAX_UE] = { false };
    unsigned int uNbUEValidated = 0;
    printMoyenneSem(etudiant, isUEValidated);
    for (int iUE = 0; iUE < form->uNbUE; ++iUE) {
        if (isUEValidated[iUE]) {
            ++uNbUEValidated;
        }
    }
}


void callCommand(Formation * form, Promotion * promotion) {
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
                    break;
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
            switch(coefficients(form, getNodeValueAsInt(1))) {
                case 0:
                    printf("Coefficients corrects");
                    break;
                case 1:
                    printf("Le numero de semestre est incorrect");
                    break;
                case 2:
                    printf("Le semestre ne contient aucune epreuve");
                    break;
                case 3:
                    printf("Les coefficients d'au moins une UE de ce semestre "
                           "sont tous nuls");
                    break;
            }
            break;
        case 438: // note
            switch(note(form, promotion, getNodeValueAsInt(1),
                        getNodeValue(2), getNodeValue(3),
                        getNodeValue(4), getNodeValueAsFloat(5))) {
                case 0:
                    printf("Note ajoutee a l'etudiant");
                    break;
                case 1:
                    printf("Le numero de semestre est incorrect");
                    break;
                case 2:
                    printf("Matiere inconnue");
                    break;
                case 3:
                    printf("Epreuve inconnue");
                    break;
                case 4:
                    printf("Note incorrecte");
                    break;
                case 5:
                    printf("Une note est deja definie pour cet etudiant");
                    break;
            }
            break;
        case 553: // notes
            switch(notes(form, promotion,
                         getNodeValueAsInt(1),
                         getNodeValue(2))) {
                case 0:
                    printf("Notes correctes");
                    break;
                case 1:
                    printf("Le numero de semestre est incorrect");
                    break;
                case 2:
                    printf("Etudiant inconnu");
                    break;
                case 3:
                    printf("Il manque au moins une note pour cet etudiant");
                    break;
            }
            break;
        case 643: // releve
            switch(releve(form, promotion, getNodeValueAsInt(1))) {
                case 0:
                    break;
                case 1:
                    printf("Le numero de semestre est incorrect");
                    break;
                case 2:
                    printf("Etudiant inconnu");
                    break;
                case 3:
                    printf("Les coefficients de ce semestre sont incorrects");
                    break;
                case 4:
                    printf("Il manque au moins une note pour cet etudiant");
                    break;
            }
            break;
        case 846: // decision
            printf("decision");
            break;
        default: // unrecognized
            printf("Cette commande n'existe pas.");
            break;
    }
    printf("\n");
}


void routine(Formation * pf, Promotion * promotion) {
    do {
        char szaUserInput[255] = {0};
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

        (pf->uNbUE > 2 && pf->uNbUE < 7 ||getStringId(getNodeValue(0)) == 975)
                ? callCommand(pf, promotion)
                : printf("Le nombre d'UE n'est pas defini\n");
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