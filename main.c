/**
 @name Modelisation Formation
 @authors Eva GENTILHOMME, Isra DHIAB
 @copyright Copyright (c) 2022, Eva GENTILHOMME, Isra DHIAB

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:\n
 1. Redistributions of source code must retain the above copyright notice, this list of conditions
   and the following disclaimer.\n
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
   and the following disclaimer in the documentation and/or other materials provided with the
   distribution.\n\n

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#pragma warning (disable:4996)

/**
 * Limits for every var and structures. \n
 * MAX_*, MIN_*, NB_*, *_MIN, *_MAX = limits for each value. \n
 * *_SID = IDs generated for each command name. \n
 * CODE_* = error codes.
 */
enum
{
    MAX_CHAR = 31,
    NB_SEMESTRES = 2,
    MIN_UE = 3,
    MAX_UE = 6,
    MAX_MATIERES = 10,
    MAX_EPREUVES = 5,
    MAX_ETUDIANTS = 100,
    NOTE_MIN = 0,
    NOTE_MAX = 20,

    // str IDs
    FORMATION_SID = 975,
    EPREUVE_SID = 764,
    COEFFICIENTS_SID = 1266,
    NOTE_SID = 438,
    NOTES_SID = 553,
    RELEVE_SID = 643,
    DECISION_SID = 846,

    // error codes
    CODE_SUCCESS = 0,

    CODE_SEM_INCORRECT = 1,
    CODE_COEFF_INCORRECT = 2,
    CODE_MAT_INCORRECT = 4,
    CODE_UE_INCORRECT = 3,
    CODE_ETU_INCORRECT = 7,
    CODE_NOTE_MISSING = 8,
    CODE_EPR_INCORRECT = 5,
    CODE_NOTE_INCORRECT = 6,

    CODE_UE_ALREADY_DEFINED = 9,
    CODE_EPR_ALREADY_DEFINED = 10,
    CODE_NOTE_ALREADY_DEFINED = 11,
    CODE_COEFF_ALL_ZERO = 12,

    CODE_SEM_EMPTY = 13
};


/**
 * A linked list structure, where each element is called a "node".
 * Each node is composed of its value (a string with a max
 * size of MAX_CHAR) and a pointer that points towards the next
 * node.
 */
typedef struct Node
{
    /*@{*/
    char szValue[MAX_CHAR]; /**< value of the node */
    struct Node * ndpNext; /**< address of the next node */
    /*@}*/
} Node;


/**
 * Epreuve structure.
 */
typedef struct {
    /*@{*/
    char szNom[MAX_CHAR]; /**< name of the Epreuve */
    float fCoef[MAX_UE]; /**< list of the coefficients of the Epreuve */
    /*@}*/
} Epreuve;

/**
 * Matiere structure.
 */
typedef struct {
    char szNom[MAX_CHAR]; /**< name of the Matiere */
    unsigned int uNbEpreuves; /**< number of Epreuve in the Matiere */
    Epreuve epreuves[MAX_EPREUVES]; /**< list of Epreuve in the Matiere */
} Matiere;

/**
 * Semestre structure.
 */
typedef struct {
    unsigned int uNbMatieres; /**< number of Matiere in the Semestre */
    Matiere matieres[MAX_MATIERES]; /**< list of Matiere in the Semestre */
} Semestre;

/**
 * Formation structure.
 */
typedef struct {
    unsigned int uNbUE; /**< Number of UE in the Formation */
    Semestre semestres[NB_SEMESTRES]; /**< list of Semestre in the Formation */
} Formation;

/**
 * Note structure.
 */
typedef struct {
    Epreuve * pEpreuve; /**< address of the Epreuve referenced by the Note */
    float fNote; /**< the grade */
} Note;

/**
 * EtuMatiere structure.
 */
typedef struct {
    Matiere * pMatiere; /**< address of the Matiere referenced by the EtuMatiere */
    unsigned int uNbNotes; /**< number of Notes in the EtuMatiere */
    Note notes[MAX_EPREUVES]; /**< list of Notes in the EtuMatiere */
} EtuMatiere;

/**
 * EtuSemestre structure.
 */
typedef struct {
    unsigned int uNbMatieres; /**< number of Matiere in the EtuSemestre */
    EtuMatiere matieres[MAX_MATIERES]; /**< list of Matiere in the EtuSemestre */
} EtuSemestre;

/**
 * Etudiant structure.
 */
typedef struct {
    char szNom[MAX_CHAR]; /**< name of the Etudiant */
    EtuSemestre semestres[NB_SEMESTRES]; /**< number of EtuSemestre */
} Etudiant;

/**
 * Promotion structure.
 */
typedef struct {
    unsigned int uNbEtudiants; /**< number of Etudiant */
    Etudiant etudiants[MAX_ETUDIANTS]; /**< list of Etudiant */
} Promotion;


/** Creates a new node in the linked list.
 * @param ndp The address of the node to link.
 * @param szValue The value to add in the new node
 * @return The node created by the function
 */
Node * newNode(Node * ndp, const char szValue[MAX_CHAR])
{
    // Memory allocation for the new node
    Node * ndNewNode = (Node *) malloc(sizeof(Node));

    // Fill the memory allocated for the node value by zeros.
    memset(&ndNewNode->szValue, 0, sizeof(char[MAX_CHAR]));
    strcpy(ndNewNode->szValue, szValue);

    // Adds the address of the next node in the current node.
    ndNewNode->ndpNext = ndp;

    return ndNewNode;
}

/** Deletes the last node put in the linked list.
 * @param ndp The address of the node to delete.
 * @return The node after the one deleted.
 */
Node * delNode(Node * ndp)
{
    Node * ndpNodeToDelete = ndp;

    // Sets the next node in the linked list as the current one
    ndp = (ndp->ndpNext != NULL) ? ndp->ndpNext
                                 : NULL;

    // Deletes the node.
    free(ndpNodeToDelete);

    return ndp;
}

/** Deletes every node in the linked list.
 *  @param ndp : The address of the first node to delete.
 *  @return NULL
 */
Node * delAllNodes(Node * ndp)
{
    while (ndp != NULL) {
        ndp = delNode(ndp);
    }
    return NULL;
}

/** Creates a node with a trimmed string as the value.
 *  @param ndp the first node's address of a linked list
 *  @param szInput the string to trim
 *  @param iMin the address of the value where the first index of the new string
 *           is stored. Will change if iMin is > 2.
 *  @param iMax the address of the value where the last index of the new string
 *           is stored. Will change if iMin is > 2.
 *  @return the address of the last node added to the linked list
 *
 */
Node * newNodeValueFromStr(Node * ndp, const char szInput[], int * iMin, int * iMax)
{
    char szCurrentStr[MAX_CHAR] = {0};
    assert (*iMax - *iMin < MAX_CHAR);

    if (*iMin <= *iMax) {
        memcpy(&szCurrentStr, &szInput[*iMin], (*iMax - *iMin)+1 * sizeof(char));
        ndp = newNode(ndp, szCurrentStr);
        memset(&szCurrentStr, 0, sizeof(char) * MAX_CHAR);
    }

    if (*iMin >= 3) {
        *iMax = *iMin-2;
        *iMin = *iMax-1;
    }

    return ndp;
}

/** Parses a string: separates every value (defined by every character
 * sequence that doesn't include the cSeparator)
 * @param ndp The first node's address of a linked list
 * @param szInput the string to parse
 * @param cSeparator the delimitation of every value
 * @return the address of the last node added to the linked list
 *
 */
Node * parseInput(Node * ndp, const char szInput[], char cSeparator)
{
    int iMin = (int) strlen(szInput)-1,
        iMax = (int) strlen(szInput);

    int iDelimiterCount = 0;

    for (int i = iMax; i >= 0; --i) {
        iMin = i+1;

        if (szInput[i] == 39 || szInput[i] == 34)
        {
            ++iDelimiterCount;
            if (iDelimiterCount%2 != 0) --iMax;
        }

        if (szInput[i] == cSeparator && iDelimiterCount%2 == 0 || i == 1) {
            if (i == 1) {
                iMin = 0;
            }

            bool isQuoted = (((szInput[i+1] == 39 || szInput[i+1] == 34) && i > 1)
                          || ((szInput[i] == 39 || szInput[i] == 34) && i == 1));

            if (isQuoted) ++iMin;

            ndp = newNodeValueFromStr(ndp, szInput, &iMin, &iMax);

            if (isQuoted) {
                --iMin;
                --iMax;
            }
        }
    }
    return ndp;
}

/** Returns the length of a linked list.
 * @param ndp: the first node's address of a linked list
 * @return the length of the linked list
 */
unsigned int getLinkedListLength(const Node * ndp)
{
    unsigned int uCount = 0;
    while (ndp != NULL) {
        ndp = ndp->ndpNext;
        ++uCount;
    }
    return uCount;
}


/**
 * Gives the value of the node uIndex, and returns NULL if the nodes doesn't exist.
 * @param ndp the first node's address of a linked list
 * @param uIndex the index of the node needed
 * @return the value of the node uIndex, or NULL if the node doesn't exist.
 *
 * @pre uIndex >= 0
 *
 * @related getNodeValueAsFloat()
 * @related getNodeValueAsInt()
*/
char * getNodeValue(Node * ndp, unsigned int uIndex)
{
    assert (uIndex >= 0);
    while (uIndex --> 0 && ndp != NULL) {
        ndp = ndp->ndpNext;
    }
    if (ndp != NULL) {
        return ndp->szValue;
    } else return NULL;
}

/**
 * Gives the value of the node uIndex as a float.
 * @param ndp the first node's address of a linked list
 * @param uIndex the index of the node needed
 * @return the value of the node uIndex as a float.
 *
 * @pre uIndex >= 0
 * @pre You must check if the node's value is a correct representation of a number before!!
 */
float getNodeValueAsFloat(Node * ndp, unsigned int uIndex)
{
    assert (uIndex >= 0);
    char * szValue = getNodeValue(ndp, uIndex);
    int i = 0, iDotIndex = -1;
    float fValue = 0;
    bool isNegative = (szValue[0] == 45);

    for (i; szValue[i+1] != 0; ++i) {
        assert (szValue[i] >= 48 && szValue[i] <= 59);
        if (szValue[i] == 46) {
            iDotIndex = i;
        }
    }

    if (iDotIndex == -1) iDotIndex = i+1;

    for (i; i >= (isNegative) ? 1 : 0; --i) {
        if (i > iDotIndex)
        {
            // multiply by 10^-1, -2... if we're in the decimal part
            fValue += (float)(szValue[i]-48)*powf(10.f,(float)-(i-iDotIndex));
        }
        else if (i < iDotIndex)
        {
            // multiply by 9^1, 2... if we're in the integer part
            fValue += (float)(szValue[i]-48)*powf(10.f, (float)(iDotIndex-1-i));
        }
    }
    return (isNegative) ? -fValue : fValue;
}

/**
 * Gives the value of the node uIndex as an int.
 * @param ndp the first node's address of a linked list
 * @param uIndex the index of the node needed
 * @return the value of the node uIndex as an int.
 * @pre uIndex >= 0
 * @pre You must check if the node's value is a correct representation of a number before!!
 */
int getNodeValueAsInt(Node * ndp, unsigned int uIndex)
{
    // preconditions will be checked in getNodeValueAsFloat()
    return (int) getNodeValueAsFloat(ndp, uIndex);
}

/** Gives an ID to the string specified
 * @param szUserInput the string
 * @return an ID (as an unsigned long)
 *
 * @warning
 *   This function is not secure. Multiple sentences can have the same ID, which makes it
 *   unreliable for a lot of usecases.
 */
unsigned long getStringId(const char szUserInput[])
{
    unsigned long uWeightString = 0;
    for (int i = 0; i < strlen(szUserInput); i++)
    {
        uWeightString += szUserInput[i];
    }
    return uWeightString;
}

/**
 * Checks if there's at least one coefficient greater than 0
 * @param uNbUE the number of UE (= number of relevant items in fCoefs array)
 * @param fCoefs array of float
 * @return true, false depending on the validity of the coefficients
 */

bool coefCorrect(unsigned int uNbUE, const float fCoefs[MAX_UE])
{
    for (int iCoef = 0; iCoef < uNbUE; ++iCoef) {
        if (fCoefs[iCoef] < 0) return false;
        if (fCoefs[iCoef] > 0) return true;
    }
    return false;
}


/**
 * Search a Matiere in a semestre, and returns its address if there's a match.
 * @param sp pointer to the Semestre to check
 * @param szMatiereName the name to search in the semestre
 * @return pointer to the Epreuve (or NULL if it doesn't exist)
 */
Matiere * getMatiereByName(Semestre * semp, const char szMatiereName[MAX_CHAR])
{
    for (int iMat = 0; iMat < semp->uNbMatieres; ++iMat) {
        if (!strcmp(semp->matieres[iMat].szNom, szMatiereName))
            return &semp->matieres[iMat];
    }
    return NULL;
}

/**
 * Creates a Matiere in a given Semestre.
 * @param semp pointer to the Semestre
 * @param szMatiereName the name of the new matiere
 * @return the newly created Matiere
 */
Matiere * newMatiere(Semestre * semp, const char szMatiereName[MAX_CHAR])
{
    Matiere * matp = &semp->matieres[semp->uNbMatieres++];

    char * szDest = matp->szNom;
    strcpy(szDest, szMatiereName);

    matp->uNbEpreuves = 0;
    printf("Matiere ajoutee a la formation\n");

    return matp;
}


/**
 * Search a Matiere in a semestre, and returns its address if there's a match.
 * @param matp pointer to the Matiere to check
 * @param szMatiereName the name to search in the semestre
 * @return pointer to the Epreuve (or NULL if it doesn't exist)
 */
Epreuve * getEpreuveByName(Matiere * matp, const char szEpreuveName[MAX_CHAR])
{
    for (int iEpr = 0; iEpr < matp->uNbEpreuves; ++iEpr) {
        if (!strcmp(matp->epreuves[iEpr].szNom, szEpreuveName))
            return &matp->epreuves[iEpr];
    }
    return NULL;
}

/**
 * Creates an Epreuve in a given Matiere.
 * @param matp pointer to the Matiere
 * @param szEpreuveName the name of the new Epreuve
 * @return the newly created Epreuve
 */
Epreuve * newEpreuve(Matiere * matp, const char szEpreuveName[MAX_CHAR],
                     const float fCoef[MAX_UE], unsigned int uNbUE)
{
    Epreuve * eprp = &matp->epreuves[matp->uNbEpreuves++];

    char * szDest = eprp->szNom;
    strcpy(szDest, szEpreuveName);

    for (int iUE = 0; iUE < uNbUE; ++iUE)
        eprp->fCoef[iUE] = fCoef[iUE];

    return eprp;
}

/**
 * Search an Etudiant in a Promotion, and returns its address if there's a match.
 * @param pp pointer to the Promotion to check
 * @param szEtudiantName the name to search in the Promotion
 * @return pointer to the Etudiant (or NULL if it doesn't exist)
 */
Etudiant * getEtudiantByName(Promotion * pp, const char szEtudiantName[MAX_CHAR])
{
    for (int iEtu = 0; iEtu < pp->uNbEtudiants; ++iEtu)
    {
        if (!strcmp(pp->etudiants[iEtu].szNom, szEtudiantName))
            return &pp->etudiants[iEtu];
    }
    return NULL;
}

/**
 * Creates an Etudiant in a given Promotion.
 * @param pp pointer to the Promotion
 * @param szEtudiantName the Etudiant name
 * @return the newly created Etudiant
 */
Etudiant * newEtudiant(Promotion * pp, const char szEtudiantName[MAX_CHAR])
{
    Etudiant * etup = &pp->etudiants[pp->uNbEtudiants++];
    strcpy(etup->szNom, szEtudiantName);
    for (int iSem = 0; iSem < NB_SEMESTRES; ++iSem) {
        etup->semestres[iSem].uNbMatieres = 0;
    }
    printf("Etudiant ajoute a la formation\n");
    return etup;
}

/**
 * Search an EtuMatiere in an EtuSemestre, and returns its address if there's a match.
 * @param etuSemp pointer to the EtuSemestre to check
 * @param szMatiereName the name to search in the EtuSemestre
 * @return pointer to the EtuMatiere (or NULL if it doesn't exist)
 */
EtuMatiere * getEtuMatiereByName(EtuSemestre * etuSemp,
                                 const char szMatiereName[MAX_CHAR])
{
    for (int iEtuMat = 0; iEtuMat < etuSemp->uNbMatieres; ++iEtuMat)
    {
        Matiere * matp = etuSemp->matieres[iEtuMat].pMatiere;
        if (!strcmp(matp->szNom, szMatiereName))
            return &etuSemp->matieres[iEtuMat];
    }
    return NULL;
}

/**
 * Creates an EtuMatiere in a given EtuSemestre, corresponding to an existing Matiere.
 * @param matp pointer to the Matiere
 * @param EtuSemestre pointer to the EtuSemestre
 * @return the newly created Epreuve
 */
EtuMatiere * newEtuMatiere(Matiere * matp, EtuSemestre * etuSemp)
{
    unsigned int uIndexMatiere = etuSemp->uNbMatieres++;
    EtuMatiere * etuMatp = &etuSemp->matieres[uIndexMatiere];

    etuMatp->pMatiere = matp;
    etuMatp->uNbNotes = 0;

    return etuMatp;
}

/**
 * Search an EtuMatiere in an EtuSemestre, and returns its address if there's a match.
 * @param etuSemp pointer to the EtuSemestre to check
 * @param szMatiereName the name to search in the EtuSemestre
 * @return pointer to the EtuMatiere (or NULL if it doesn't exist)
 */
Note * getNoteByEpreuveName(EtuMatiere * etuMatp,
                            const char szEpreuveName[MAX_CHAR])
{
    for (int iNote = 0; iNote < etuMatp->uNbNotes; ++iNote)
    {
        Epreuve * eprp = etuMatp->notes[iNote].pEpreuve;
        if (!strcmp(eprp->szNom, szEpreuveName))
            return &etuMatp->notes[iNote];
    }
    return NULL;
}

/**
 * Creates a Note in a given EtuMatiere, corresponding to an existing Epreuve.
 * @param eprp pointer to the Epreuve to link
 * @param etuMatp pointer to the EtuMatiere
 * @param fNote: the Note to add
 * @return the newly created Note
 */
Note * newNote(Epreuve * eprp, EtuMatiere * etuMatp, float fNote) {
    unsigned int uIndexNote = etuMatp->uNbNotes++;
    Note * note = &etuMatp->notes[uIndexNote];
    note->pEpreuve = eprp;
    note->fNote = fNote;
    return note;
}

/**
 * Counts the number of Epreuve in a Semestre, and adds every coefficients of
 * each UE in an array.
 * @param fp pointer to the Formation
 * @param semp pointer to the Semestre
 * @param fCoefs array of floats, representing a sum of every coefficients in each UE. Is changed by this function.
 * @return the number of epreuves, as an unsigned int
 */
unsigned int countEpreuve(const Formation * fp, const Semestre * semp,
                 float fCoefs[MAX_UE])
{
    unsigned int uCountEpreuve = 0;
    for (int iMat = 0; iMat < semp->uNbMatieres; ++iMat)
    {
        const Matiere * matp = &semp->matieres[iMat];
        for (int iEpr = 0; iEpr < matp->uNbEpreuves; ++iEpr)
        {
            ++uCountEpreuve;
            const Epreuve * eprp = &matp->epreuves[iEpr];

            for (int iUE = 0; iUE < fp->uNbUE; ++iUE)
            {
                fCoefs[iUE] += eprp->fCoef[iUE];
            }
        }
    }
    return uCountEpreuve;
}

/**
 * Checks if the Etudiant have a Note for every Epreuve
 * @param semp
 * @param etuSemp
 * @return true or false
 */
bool checkNotes(Semestre * semp, EtuSemestre * etuSemp) {
    if (semp->uNbMatieres == etuSemp->uNbMatieres)
    {
        for (int iMat = 0; iMat < etuSemp->uNbMatieres; ++iMat)
        {
            Matiere * matp = &semp->matieres[iMat];
            EtuMatiere * etuMatp = &etuSemp->matieres[iMat];
            if (etuMatp->uNbNotes != matp->uNbEpreuves) return false;
        }
    } else return false;
    return true;
}

/**
 * Trims a number.
 * @param fValue The number to trim
 * @param trimAfter The number of digits to trim. 0 = the dot
 * @return trimmed number
 */
float trim(float fValue, int trimAfter)
{
    float fRank = powf(10, (float)trimAfter);
    return floorf(fValue*fRank)/fRank;
}

/**
 * Returns the moyenne for a given UE and EtuMatiere.
 * @param etuMatp
 * @param uIndexUE
 * @return moyenne (float)
 */
float getMoyenneMatiere(EtuMatiere * etuMatp, unsigned int uIndexUE)
{
    float fDivid = 0, fDivis = 0;
    for (int iNotes = 0; iNotes < etuMatp->uNbNotes; iNotes++)
    {
        fDivid += etuMatp->notes[iNotes].fNote
                 * etuMatp->notes[iNotes].pEpreuve->fCoef[uIndexUE];
        fDivis += etuMatp->notes[iNotes].pEpreuve->fCoef[uIndexUE];
    }
    if (fDivis == 0) return -1.f;
    return fDivid / fDivis;
}

/**
 * Returns the moyenne of every Epreuve in a Semestre for a given UE
 * @param etuSemp
 * @param uIndexUE
 * @return moyenne (float)
 */
float getMoyenneUE(EtuSemestre * etuSemp, unsigned int uIndexUE)
{
    float fDivid = 0, fDivis = 0;

    for (int iIndMat = 0; iIndMat < etuSemp->uNbMatieres; ++iIndMat)
    {
        EtuMatiere *etuMatp = &etuSemp->matieres[iIndMat];
        for (int iIndNote = 0; iIndNote < etuMatp->uNbNotes; ++iIndNote)
        {
            Note *note = &etuMatp->notes[iIndNote];
            fDivid += note->fNote * note->pEpreuve->fCoef[uIndexUE];
            fDivis += note->pEpreuve->fCoef[uIndexUE];
        }
    }
    if (fDivis == 0) return -1.f;
    return fDivid / fDivis;
}

/**
 * Returns the moyenne of every Semestre for a given UE
 * @param etup
 * @param uIndexUE
 * @return moyenne (float)
 */
float getMoyenneUEYear(Etudiant * etup, unsigned int uIndexUE)
{
    float fDivid = 0;

    for (int iSem = 0; iSem < NB_SEMESTRES; ++iSem)
    {
        EtuSemestre * etuSemp = &etup->semestres[iSem];
        fDivid += getMoyenneUE(etuSemp, uIndexUE);
    }
    return fDivid / NB_SEMESTRES;
}

/**
 * Returns the maximum between two unsigned int
 * @param u1
 * @param u2
 * @return The maximum between u1 and u2
 */
unsigned int maxi(unsigned int u1, unsigned int u2) 
{
    if (u1 > u2)
        return u1;
    return u2;
}

/**
 * Gets the length of the longest name for a Matiere in a Semestre
 * @param semp Pointer to Semestre
 * @return Length of the longest Matiere name in semp
 */
unsigned int getLongestMatiereName(Semestre * semp)
{
    unsigned int uLength = 0;
    for (int iMat = 0; iMat < semp->uNbMatieres; ++iMat)
    {
        Matiere * matp = &semp->matieres[iMat];
        uLength = maxi(uLength, strlen(matp->szNom));
    }
    return uLength;
}

/** Sub-function of releve().
 *  Prints the UE line
 * @param uMaxName The longest matiere name
 * @param uNbUE Number of UE
 */
void printUELine(unsigned int uMaxName, unsigned int uNbUE) {
    printf("%*s", uMaxName+1, " ");
    for (int iUE = 1; iUE <= uNbUE; ++iUE)
        printf(" UE%d ", iUE);
    printf("\n");
}

/**
 * Sub-function of releve().
 * Prints a moyenne line for a given Matiere
 * @param matp
 * @param etuMatp
 * @param uMaxName
 * @param uNbUE
 */
void printReleveLine(Matiere * matp, EtuMatiere * etuMatp, unsigned int uMaxName,
                     unsigned int uNbUE)
{
    printf("%s%*s", matp->szNom, (int)(uMaxName-strlen(matp->szNom)+1), " ");

    for (int iUE = 0; iUE < uNbUE; ++iUE)
    {
        float fMoyenne = trim(getMoyenneMatiere(etuMatp, iUE), 1);
        if (fMoyenne != -1) {
            printf("%4.1f ", fMoyenne);
        } else {
            printf("  ND ");
        }
    }
    printf("\n");
}

/** Sub-function of decision() and releve().
 * Prints UE moyenne for a Semestre.
 * @param etuSemp
 * @param uMaxName
 * @param uNbUE
 * @param szTitle Name of this line
 */
void printMoyenneUELine(EtuSemestre * etuSemp, unsigned int uMaxName,
                        unsigned int uNbUE, const char szTitle[])
{
    printf("%s%*s", szTitle, (int)(uMaxName-strlen(szTitle)+1), " ");
    for (int iUE = 0; iUE < uNbUE; ++iUE)
    {
        float fMoyenne = trim(getMoyenneUE(etuSemp, iUE), 1);
        printf("%4.1f ", fMoyenne);
    }
    printf("\n");
}

/** Sub-function of decision().
 * Prints every UE moyenne.
 * @param etup
 * @param isUEValidated
 * @param uNbUEValidated
 * @param uNbUE
 */
void printGenMoyenne(Etudiant * etup, bool isUEValidated[MAX_UE], unsigned int * uNbUEValidated,
                     unsigned int uNbUE)
{
    printf("Moyennes annuelles ");
    for (int iIndUE = 0; iIndUE < uNbUE; ++iIndUE)
    {
        float fMoySem = trim(getMoyenneUEYear(etup, iIndUE), 1);
        printf("%4.1f ", fMoySem);
        isUEValidated[iIndUE] = (fMoySem >= 10) ? true : false;
        if (isUEValidated[iIndUE]) ++*uNbUEValidated;
    }
    printf("\n");
}

/** Sub-function of decision().
 * Prints every UE validated.
 * @param uMaxName
 * @param uNbUEValidated
 * @param isUEValidated
 * @param uNbUE
 */
void printAcquisitionLine(unsigned int uMaxName, unsigned int uNbUEValidated,
                          const bool isUEValidated[MAX_UE], unsigned int uNbUE)
{
    printf("Acquisition%*s", (int)(uMaxName-strlen("Acquisition")+1), " ");

    if (uNbUEValidated == 0)
    {
        printf("Aucune");
    }
    else
    {
        unsigned int uCount = 0;
        for (int iUE = 0; iUE < uNbUE; ++iUE)
        {
            if (isUEValidated[iUE])
            {
                ++uCount;
                char szName[4] = {'U', 'E', (char) (iUE + 49), 0};
                printf("%s%s", szName, (uNbUEValidated != uCount) ? ", " : "");
            }
        }
    }
    printf("\n");
}

/** Sub-function of decision().
 * Prints every line corresponding to Semestre's moyennes.
 * @param etup
 * @param uMaxName
 * @param uNbUE
 */
void printSemestreLines(Etudiant * etup, unsigned int uMaxName, unsigned int uNbUE) {
    for (int iIndSem = 0; iIndSem < 2; ++iIndSem)
    {
        char szSemestre[3] = {'S', (char)(iIndSem+49), 0};
        EtuSemestre * etuSemp = &etup->semestres[iIndSem];
        printMoyenneUELine(etuSemp, uMaxName, uNbUE, szSemestre);
    }
}


/** Defines the formation's UE number if it's not already defined.
 * @param fp Pointer to Formation
 * @param uArgNbUe Number of UE, between MIN_UE and MAX_UE.
 * @return error code
 */
int formation(Formation * fp, unsigned int uArgNbUE)
{
    if (fp->uNbUE < MIN_UE || fp->uNbUE > MAX_UE)
    { // UE number is not defined in the formation
        if (uArgNbUE < MIN_UE || uArgNbUE > MAX_UE)
            return CODE_UE_INCORRECT;

        fp->uNbUE = uArgNbUE;
        return CODE_SUCCESS;
    } else return CODE_UE_ALREADY_DEFINED;
}

/**
 * Creates a new Epreuve.
 *
 * @param fp Pointer to Formation
 * @param uArgIndSemestre the semestre number (starts at 1)
 * @param szArgMatiereName matiere name
 * @param szArgEpreuveName epreuve name
 * @param fCoef coefficients list
 *
 * @return error code
 */
int epreuve(Formation * fp, unsigned int uArgIndSemestre, const char szArgMatiereName[MAX_CHAR],
            const char szArgEpreuveName[MAX_CHAR], const float fCoef[MAX_UE])
{   // Semestre check
    if (uArgIndSemestre > NB_SEMESTRES || uArgIndSemestre < 1)
        return CODE_SEM_INCORRECT;

    Semestre * semp = &fp->semestres[uArgIndSemestre-1];

    Matiere * matp = getMatiereByName(semp, szArgMatiereName);

    // Matiere check, if it doesn't exist, create it
    if (matp != NULL) {
        // Checks if the Epreuve already exists
        if (getEpreuveByName(matp, szArgEpreuveName) != NULL)
            return CODE_EPR_ALREADY_DEFINED;
    } else {
        // Coefficients check
        if (!coefCorrect(fp->uNbUE, fCoef))
            return CODE_COEFF_INCORRECT;
        matp = newMatiere(semp, szArgMatiereName);
    }

    Epreuve * eprp = newEpreuve(matp, szArgEpreuveName, fCoef, fp->uNbUE);

    return CODE_SUCCESS;
}

/**
 * Checks the validity of a Semestre's coefficients.
 * @param fp Pointer to Formation
 * @param uArgIndSemestre Semestre to check
 * @return error code
 */
int coefficients(const Formation * fp, unsigned int uArgIndSemestre) {   
    // Semestre check
    if (uArgIndSemestre > NB_SEMESTRES || uArgIndSemestre < 1)
        return CODE_SEM_INCORRECT;

    const Semestre * semp = &fp->semestres[uArgIndSemestre-1];

    float fCoefs[MAX_UE] = {0};
    unsigned int uCountEpreuve = countEpreuve(fp, semp, fCoefs);

    if (uCountEpreuve == 0 || semp->uNbMatieres == 0)
        return CODE_SEM_EMPTY;

    for (int iCoef = 0; iCoef < fp->uNbUE; ++iCoef)
        if (fCoefs[iCoef] == 0) return CODE_COEFF_ALL_ZERO;

    return CODE_SUCCESS;
}

/**
 * Adds a new note to an Etudiant
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 * @param uArgIndSemestre Semestre to add
 * @param szArgEtudiantName Etudiant name
 * @param szArgMatiereName Matiere name
 * @param szArgEpreuveName Epreuve name
 * @param fArgNote The grade to add
 * @return error code
 */
int note(Formation * fp, Promotion * pp, unsigned int uArgIndSemestre,
         const char szArgEtudiantName[MAX_CHAR], const char szArgMatiereName[MAX_CHAR],
         const char szArgEpreuveName[MAX_CHAR], float fArgNote)
{   // Semestre check
    if (uArgIndSemestre > NB_SEMESTRES || uArgIndSemestre < 1)
        return CODE_SEM_INCORRECT;

    Semestre * semp = &fp->semestres[uArgIndSemestre-1];

    // Matiere check
    Matiere * matp = getMatiereByName(semp, szArgMatiereName);
    if (matp == NULL) return CODE_MAT_INCORRECT;

    // Epreuve check
    Epreuve * eprp = getEpreuveByName(matp, szArgEpreuveName);
    if (eprp == NULL) return CODE_EPR_INCORRECT;

    // Checks if the note is between NOTE_MIN and NOTE_MAX
    if (fArgNote < NOTE_MIN || fArgNote > NOTE_MAX) return CODE_NOTE_INCORRECT;

    // Etudiant check
    // Creates the Etudiant if it doesn't already exist
    Etudiant * etup = getEtudiantByName(pp, szArgEtudiantName);
    if (etup == NULL) etup = newEtudiant(pp, szArgEtudiantName);

    EtuSemestre * etuSemp = &etup->semestres[uArgIndSemestre-1];

    // EtuMatiere check
    // Creates the EtuMatiere if it doesn't already exist
    EtuMatiere * etuMatp = getEtuMatiereByName(etuSemp, szArgMatiereName);
    if (etuMatp == NULL) etuMatp = newEtuMatiere(matp, etuSemp);

    // Note check
    // Returns an error code if the Note already exists
    Note * notep = getNoteByEpreuveName(etuMatp, szArgEpreuveName);
    if (notep != NULL) return CODE_NOTE_ALREADY_DEFINED;

    notep = newNote(eprp, etuMatp, fArgNote);

    return CODE_SUCCESS;
}

/**
 * Checks the validity of an Etudiant's school report
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 * @param uArgIndSemestre Semestre to check
 * @param szArgEtudiantName Etudiant name
 * @return error code
 */
int notes(Formation * fp, Promotion * pp,
          unsigned int uArgIndSemestre, const char szArgEtudiantName[MAX_CHAR])
{
    if (uArgIndSemestre < 1 || uArgIndSemestre > NB_SEMESTRES)
        return CODE_SEM_INCORRECT;

    Semestre * semp = &fp->semestres[uArgIndSemestre-1];

    Etudiant * etup = getEtudiantByName(pp, szArgEtudiantName);

    if (etup == NULL) return CODE_ETU_INCORRECT;

    EtuSemestre * etuSemp = &etup->semestres[uArgIndSemestre-1];

    if (!checkNotes(semp, etuSemp)) return CODE_NOTE_MISSING;

    return CODE_SUCCESS;
}


/**
 * Prints the Semestre school report of an Etudiant
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 * @param uArgIndSemestre Semestre to check
 * @param szArgEtudiantName Etudiant name
 * @return error code
 */
int releve(Formation * fp, Promotion * pp,
           unsigned int uArgIndSemestre, const char szArgEtudiantName[MAX_CHAR])
{
   if (uArgIndSemestre < 1 || uArgIndSemestre > NB_SEMESTRES)
       return CODE_SEM_INCORRECT;

   Semestre * semp = &fp->semestres[uArgIndSemestre-1];

   Etudiant * etup = getEtudiantByName(pp, szArgEtudiantName);
   if (etup == NULL) return CODE_ETU_INCORRECT;

   EtuSemestre * etuSemp = &etup->semestres[uArgIndSemestre-1];

   if (coefficients(fp, uArgIndSemestre) != 0)
       return CODE_COEFF_INCORRECT;

   if (!checkNotes(semp, etuSemp))
       return CODE_NOTE_MISSING;

   unsigned int uMaxName = maxi(getLongestMatiereName(semp), strlen("Matieres"));

   printUELine(uMaxName, fp->uNbUE);
   for (int iMat = 0; iMat < semp->uNbMatieres; ++iMat)
   {
       Matiere * matp = &semp->matieres[iMat];
       printReleveLine(matp, getEtuMatiereByName(etuSemp, matp->szNom), uMaxName, fp->uNbUE);
   }
   printf("--\n");
   printMoyenneUELine(etuSemp, uMaxName, fp->uNbUE, "Moyennes");

   return CODE_SUCCESS;
}


/**
 * Prints the year's school report of an Etudiant, and the jury's decision
 *
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 * @param szArgEtudiantName Etudiant name
 * @return error code
 */
int decision(Formation * fp, Promotion * pp, const char szArgEtudiantName[MAX_CHAR])
{
    Etudiant * etup = getEtudiantByName(pp, szArgEtudiantName);
    if (etup == NULL) return CODE_ETU_INCORRECT;

    if (coefficients(fp, 1) != CODE_SUCCESS
     || coefficients(fp, 2) != CODE_SUCCESS)
        return CODE_SEM_INCORRECT;

    if (notes(fp, pp, 1, szArgEtudiantName) != CODE_SUCCESS
     || notes(fp, pp, 2, szArgEtudiantName) != CODE_SUCCESS)
        return CODE_NOTE_INCORRECT;

    unsigned int uMaxName = (unsigned int)strlen("Moyennes annuelles");
    printUELine(uMaxName, fp->uNbUE);

    printSemestreLines(etup, uMaxName, fp->uNbUE);
    printf("--\n");

    bool isUEValidated[MAX_UE] = { false };
    unsigned int uNbUEValidated = 0;
    printGenMoyenne(etup, isUEValidated, &uNbUEValidated, fp->uNbUE);

    bool isValidated = (uNbUEValidated > fp->uNbUE/2);

    printAcquisitionLine(uMaxName, uNbUEValidated, isUEValidated, fp->uNbUE);

    printf("Devenir%*s", (int)(uMaxName-strlen("Devenir")+1), " ");
    printf((isValidated) ? "Passage" : "Redoublement");
    printf("\n");

    return CODE_SUCCESS;
}

/** Interprets the result of formation().
 *
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 */
void callFormation(Node * ndp, Formation * fp)
{
    switch (formation(fp, getNodeValueAsInt(ndp, 1)))
    {
        case CODE_SUCCESS:
            printf("Le nombre d'UE est defini\n");
            break;
        case CODE_UE_INCORRECT:
            printf("Le nombre d'UE est incorrect\n");
            break;
        case CODE_UE_ALREADY_DEFINED:
            printf("Le nombre d'UE est deja defini\n");
            break;
        default:
            printf("Code erreur inconnu.\n");
    }
}

/** Interprets the result of epreuve().
 *
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 */
void callEpreuve(Node * ndp, Formation * fp)
{
    float fCoefs[MAX_UE] = {-1};
    for (int iUE = 0; iUE < fp->uNbUE; ++iUE)
        fCoefs[iUE] = getNodeValueAsFloat(ndp, 4+iUE);


    switch (epreuve(fp, getNodeValueAsInt(ndp, 1), getNodeValue(ndp, 2),
                    getNodeValue(ndp, 3), fCoefs))
                    {
        case CODE_SUCCESS:
            printf("Epreuve ajoutee a la formation\n");
            break;
        case CODE_SEM_INCORRECT:
            printf("Le numero de semestre est incorrect\n");
            break;
        case CODE_COEFF_INCORRECT:
            printf("Au moins un des coefficients est incorrect\n");
            break;
        case CODE_EPR_ALREADY_DEFINED:
            printf("Une meme epreuve existe deja\n");
            break;
        default:
            printf("Code erreur inconnu.\n");
    }
}

/** Interprets the result of coefficients().
 *
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 */
void callCoefficients(Node * ndp, Formation * fp)
{
    switch (coefficients(fp, getNodeValueAsInt(ndp, 1)))
    {
        case CODE_SUCCESS:
            printf("Coefficients corrects\n");
            break;
        case CODE_SEM_INCORRECT:
            printf("Le numero de semestre est incorrect\n");
            break;
        case CODE_SEM_EMPTY:
            printf("Le semestre ne contient aucune epreuve\n");
            break;
        case CODE_COEFF_ALL_ZERO:
            printf("Les coefficients d'au moins une UE de ce semestre sont tous nuls\n");
            break;
        default:
            printf("Code erreur inconnu.\n");
    }
}

/** Interprets the result of note().
 *
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 */
void callNote(Node * ndp, Formation * fp, Promotion * pp)
{
    switch (note(fp, pp, getNodeValueAsInt(ndp, 1), getNodeValue(ndp, 2), getNodeValue(ndp, 3),
                 getNodeValue(ndp, 4), getNodeValueAsFloat(ndp, 5)))
    {
        case CODE_SUCCESS:
            printf("Note ajoutee a l'etudiant\n");
            break;
        case CODE_SEM_INCORRECT:
            printf("Le numero de semestre est incorrect\n");
            break;
        case CODE_MAT_INCORRECT:
            printf("Matiere inconnue\n");
            break;
        case CODE_EPR_INCORRECT:
            printf("Epreuve inconnue\n");
            break;
        case CODE_NOTE_INCORRECT:
            printf("Note incorrecte\n");
            break;
        case CODE_NOTE_ALREADY_DEFINED:
            printf("Une note est deja definie pour cet etudiant\n");
            break;
        default:
            printf("Code erreur inconnu.\n");
    }
}

/** Interprets the result of notes().
 *
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 */
void callNotes(Node * ndp, Formation * fp, Promotion * pp)
{
    switch (notes(fp, pp, getNodeValueAsInt(ndp, 1),
                  getNodeValue(ndp, 2))) {
        case CODE_SUCCESS:
            printf("Notes correctes\n");
            break;
        case CODE_SEM_INCORRECT:
            printf("Le numero de semestre est incorrect\n");
            break;
        case CODE_ETU_INCORRECT:
            printf("Etudiant inconnu\n");
            break;
        case CODE_NOTE_MISSING:
            printf("Il manque au moins une note pour cet etudiant\n");
            break;
        default:
            printf("Code erreur inconnu.\n");
    }
}

/**
 * Interprets the result of releve().
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 */
void callReleve(Node * ndp, Formation * fp, Promotion * pp)
{
    switch (releve(fp, pp, getNodeValueAsInt(ndp, 1), getNodeValue(ndp, 2)))
    {
        case CODE_SUCCESS:
            break;
        case CODE_SEM_INCORRECT:
            printf("Le numero de semestre est incorrect\n");
            break;
        case CODE_ETU_INCORRECT:
            printf("Etudiant inconnu\n");
            break;
        case CODE_COEFF_INCORRECT:
            printf("Les coefficients de ce semestre sont incorrects\n");
            break;
        case CODE_NOTE_MISSING:
            printf("Il manque au moins une note pour cet etudiant\n");
            break;
        default:
            printf("Code erreur inconnu\n");
    }
}

/**
 * Interprets the result of decision().
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 */
void callDecision(Node * ndp, Formation * fp, Promotion * pp)
{
    switch (decision(fp, pp, getNodeValue(ndp, 1)))
    {
        case CODE_SUCCESS:
            break;
        case CODE_ETU_INCORRECT:
            printf("Etudiant inconnu\n");
            break;
        case CODE_SEM_INCORRECT:
            printf("Les coefficients d'au moins un semestre sont incorrects\n");
            break;
        case CODE_NOTE_INCORRECT:
            printf("Il manque au moins une note pour cet etudiant\n");
            break;
        default:
            printf("Code erreur inconnu\n");
    }
}

/**
 * Calls each command depending on the user's input.
 * @param ndp Pointer to Node
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 */
void callCommand(Node * ndp, Formation * fp, Promotion * pp)
{
    switch (getStringId(getNodeValue(ndp, 0)))
    {
        case FORMATION_SID:
            callFormation(ndp, fp);
            break;
        case EPREUVE_SID:
            callEpreuve(ndp, fp);
            break;
        case COEFFICIENTS_SID:
            callCoefficients(ndp, fp);
            break;
        case NOTE_SID:
            callNote(ndp, fp, pp);
            break;
        case NOTES_SID:
            callNotes(ndp, fp, pp);
            break;
        case RELEVE_SID:
            callReleve(ndp, fp, pp);
            break;
        case DECISION_SID:
            callDecision(ndp, fp, pp);
            break;
        default:
            printf("Cette commande n'existe pas.\n");
    }
}

/**
 * The main loop.
 * @param fp Pointer to Formation
 * @param pp Pointer to Promotion
 */
void routine(Formation * fp, Promotion * pp)
{
    do
    {
        Node * ndp = NULL;
        char szUserInput[255] = {0};
        scanf(" %[^\n]s", &szUserInput);

        ndp = parseInput(ndp, szUserInput, ' ');

        if (!strcmp(getNodeValue(ndp, 0), "exit"))
        {
            break;
        }

        if (fp->uNbUE >= MIN_UE && fp->uNbUE <= MAX_UE
           || getStringId(getNodeValue(ndp, 0)) == FORMATION_SID)
        {
            callCommand(ndp, fp, pp);
        } else {
            printf("Le nombre d'UE n'est pas defini\n");
        }
        delAllNodes(ndp);
    } while (1);
}


int main() {
    
    Formation f;
    f.uNbUE = 0;
    for (int iSem = 0; iSem < NB_SEMESTRES; ++iSem) {
        f.semestres[iSem].uNbMatieres = 0;
    }

    Promotion p;
    p.uNbEtudiants = 0;

    
    Formation * fp = &f;
    Promotion * pp = &p;

    routine(fp, pp);
    return 0;
}
