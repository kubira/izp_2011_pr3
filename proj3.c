/**
 * Soubor:  proj3.c
 * Datum:   25.11.2011
 * Autor:   Radim Kubis, xkubis03@stud.fit.vutbr.cz
 * Projekt: Maticove operace
 * Popis:   Program provadi operace s maticemi obecnych rozmeru obsahujici
 *          cela cisla. Matice musi byt ulozeny v textovych souborech
 *          ve formatu:
 *          
 *          X Y
 *          a b c d
 *          e f g h
 *          i j k l
 *          m n o p
 *          
 *          kde X (pocet radku), Y (pocet sloupcu) jsou rozmery matice
 *          a cisla [a-p] jsou cela cisla v matici.
 *          Pokud je vysledkem operace matice, je vypsana ve stejnem formatu.  
 *         
 *          V programu jsou implementovane nasledujici operace:
 *            - kontrola vstupnich dat
 *            - soucet matic (A + B)
 *            - soucin matic (A * B)
 *            - transpozice matice (A^T)
 *            - maticovy vyraz (A * B^T + A)
 *            - oznaceni vodniho toku
 *            - cesta koule po kulecniku
 *
 *          Parametry prikazove radky pro volbu a pouziti operaci:
 *          
 *            -h - vypise napovedu programu
 * 
 *            --test - otestuje vstupni matici, jestli je spravne naformatovana
 *                     a vypise ji na vystup
 * 
 *            --add A.txt B.txt - secte matice v souborech A.txt a B.txt
 *                                a matici s vysledkem vypise 
 * 
 *            --mult A.txt B.txt - vynasobi matice v souborech A.txt a B.txt
 *                                 a matici s vysledkem vypise 
 * 
 *            --trans A.txt - provede transpozici matice v souboru A.txt
 *                            a vysledek vypise 
 * 
 *            --expr A.txt B.txt - vypocita vyraz (A * B^T + A) a vyslednou
 *                                 matici vypise 
 * 
 *            --water r s M.txt - oznaci hladinu vodniho toku v matici
 *                                v souboru M.txt prameniciho na souradnicich
 *            r, s (radek, sloupec) a vysledek vypise 
 *           
 *            --carom r s dir power M.txt - sleduje cestu kulecnikove koule
 *                                          po stole urcenem matici v souboru
 *            M.txt, r a s (radek, sloupec) jsou souradnice pocatecni pozice
 *            koule, dir je pocatecni smer koule, power udava silu uderu,
 *            vysledkem je posloupnost cisel na pozicich cesty koule   
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/**
 * Parametry prikazoveho radku
 */
const char *sides[6] = {"V", "JV", "JZ", "Z", "SZ",  "SV"};  /* Smery koule */
const char *funcs[7] = {"--test", "--add", "--mult", "--trans",
                        "--expr", "--water", "--carom"};  /* Funkce programu */

/**
 * Pole textovych retezcu vypisovanych funkci printErr
 */
const char *ERRMSG[] = {
  /* OK */
  "Vse je v poradku.",
  /* F_OPEN */
  "Nepodarilo se otevrit soubor: ",
  /* F_CLOSE */
  "Nepodarilo se uzavrit soubor: ",
  /* NO_MEM */
  "Nepodarilo se alokovat dostatek pameti pro beh programu.",
  /* BAD_MAT */
  "Spatny rozmer matic pro vypocet.",
  /* BAD_FMAT */
  "Spatna matice v souboru: ",
  /* NO_MAT */
  "Nejedna se o matici v souboru: ",
  /* BAD_CORDS */
  "Zadane souradnice jsou mimo rozsah matice v souboru: ",
  /* NO_ALLOC */
  "Nepodarilo se alokovat pamet pro matici v souboru: ",
  /* BAD_CMD */
  "Spatne zadane parametry prikazoveho radku.",
  /* NO_PARAM */
  "Nebyl zadan zadny parametr pozadovane funkce.",
  /* NO_CMD */
  "Byl zadan neznamy parametr: ",
  /* NEG_CORD_R */
  "Byla zadana zaporna souradnice radku: ",
  /* CORD_R_ERROR */
  "Byla zadana neplatna hodnota souradnice radku: ",
  /* NEG_CORD_S */
  "Byla zadana zaporna souradnice sloupce: ",
  /* CORD_S_ERROR */
  "Byla zadana neplatna hodnota souradnice sloupce: ",
  /* BAD_SIDE */
  "Byla zadana neexistujici strana sestiuhelniku: ",
  /* BAD_POWER */
  "Byla zadana zaporna hodnota sily odpalu: ",
  /* POWER_ERROR */
  "Byla zadana neplatna hodnota sily odpalu: ",
  /* CMD_COUNT */
  "Nespravny pocet zadanych parametru.",
  /* OTHER */
  "Doslo k neocekavane chybe.",
  /* FALSE */
  "false\n",
  /* HELP */
  "\nMaticove operace\n"
  "Autor: Radim Kubis, xkubis03 (c) 2011\n"
  "Program provadi maticove operace:\n\n"
  "- kontrola vstupnich dat\n"
  "- soucet matic (A + B)\n"
  "- soucin matic (A * B)\n"
  "- transpozice matice (A^T)\n"
  "- maticovy vyraz (A * B^T + A)\n"
  "- oznaceni vodniho toku\n"
  "- cesta koule po kulecniku\n\n"
  "Pouziti:\n"
  "  -h                - vypise napovedu\n"
  "  --test x          - provede kontrolu vstupnich dat v souboru x\n"
  "  --add x y         - soucet matic v souborech x a y\n"
  "  --mult x y        - soucin matic v souborech x a y\n"
  "  --trans x         - transpozice matice v souboru x\n"
  "  --expr x y        - vypocita vyraz (A * B^T + A) z matic souboru x a y\n"
  "  --water r s m     - oznaci vodni tok pramenici na souradnicich r, s\n"
  "                      v matici v souboru m\n"
  "  --carom r s d p m - vypise hodnoty na pozicich cesty koule v matici\n"
  "                      urcene souborem m, pocatecni pozice koule jsou\n"
  "                      souradnice r a s, smer koule je d, sila odpalu p\n\n"
  "Pokud je vysledkem operace matice, vypise ji, ale pokud nema operace\n"
  "legalni vysledek, vypise false.\n"
};

/**
 * Kody moznych zadanych funkci
 */ 
enum function {
  F_TEST = 0,  /* Test spravnosti matice v souboru */
  F_ADD,       /* Soucet matic */
  F_MULT,      /* Soucin matic */
  F_TRANS,     /* Transpozice matice */
  F_EXPR,      /* Maticovy vyraz */
  F_WATER,     /* Oznaceni vodniho toku */
  F_CAROM,     /* Cesta koule po kulecniku */
  F_END
};

/**
 * Kody chyb a stavu programu
 */
enum errors {
  OK = 0,    /* Bez chyby */
  F_OPEN,    /* Nepodarilo se otevrit soubor */
  F_CLOSE,   /* Nepodarilo se uzavrit soubor */
  NO_MEM,    /* Malo pameti */
  BAD_MAT,   /* Spatny rozmer matice */
  BAD_FMAT,  /* Spatna matice v souboru */
  NO_MAT,    /* Nejedna se o matici */
  BAD_CORDS, /* Spatne zadane souradnice */
  NO_ALLOC,  /* Nepodarilo se alokovat matici */
  BAD_CMD,   /* Spatny prepinac */
  NO_PARAM,  /* Nebyl zadan zadny parametr */
  NO_CMD,    /* Neznamy parametr */
  NEG_CORD_R,    /* Zaporna hodnota souradnice R */
  CORD_R_ERROR,  /* Neplatna hodnota souradnice R */
  NEG_CORD_S,    /* Zaporna hodnota souradnice S */
  CORD_S_ERROR,  /* Neplatna hodnota souradnice S */
  BAD_SIDE,      /* Spatne zadany smer koule */
  BAD_POWER,     /* Zaporna hodnota sily odpalu */
  POWER_ERROR,   /* Spatne zadana sila odpalu */
  CMD_COUNT,     /* Spatny pocet parametru */
  OTHER,     /* Neznama chyba */
  FALSE,     /* Nelegalni vysledek */
  HELP,      /* Napoveda */
  E_END,     /* Zarazka */
};

/**
 * Popis sten sestiuhelniku
 */
/*
        SZ /\ SV
        Z |  | V
        JZ \/ JV
*/

/**
 * Kody sten sestiuhelniku
 */
enum side {
  V = 0,  /* Prava stena */
  JV,     /* Prava stena dole */
  JZ,     /* Leva stena dole */
  Z,      /* Leva stena */
  SZ,     /* Leva stena nahore */
  SV,     /* Prava stena nahore */
  S_END   /* Zarazka seznamu */
};

/**
 * Popis sten a rohu matice sestiuhelniku
 */
/*
            D   4   A
              o---o
            3 | 0 | 1
              o---o
            C   2   B
*/

/**
 * Kody sten a rohu celkove matice s sestiuhelniky
 */
enum position {
  CENTER = 0,  /* Stred matice */ 
  WALL_1 = 2,  /* Prava stena */
  WALL_2 = 4,  /* Spodni stena */
  WALL_3 = 8,  /* Leva stena */
  WALL_4 = 1,  /* Horni stena */
  CORN_A = 3,  /* Pravy horni roh */
  CORN_B = 6,  /* Pravy dolni roh */
  CORN_C = 12, /* Levy dolni roh */
  CORN_D = 9   /* Levy horni roh */
};

/**
 * Struktura obsahujici hodnoty parametru prikazove radky
 */
typedef struct params {
  int row;    /* Souradnice radku */
  int col;    /* Souradnice sloupce */
  int code;   /* Chybovy/stavovy kod programu */
  int side;   /* Smer odpalu */
  int power;  /* Sila odpalu */
  int function; /* Kod zadane funkce */
  char *file1;  /* Cesta a nazev souboru 1 */
  char *file2;  /* Cesta a nazev souboru 2 */
  char *error;  /* Parametr, ve kterem nastala chyba */
} TParams;

/**
 * Struktura MATRIX pro uchovani poctu radku, sloupcu a prvku matice
 */
typedef struct {
  int rows;      /* Pocet radku */
  int cols;      /* Pocet sloupcu */
  int **matrix;  /* Ukazatel na prvky matice */
} MATRIX;

/**
 * Vytiskne text popisujici stav nebo chybu programu
 * @param code - kod chyby nebo stavu programu, ktery nastal
 */
void printErr(TParams p)
{
  if(p.code < OK || p.code >= E_END) {  // Pokud je kod chyby neznamy
    p.code = OTHER;  // Nastavim hodnotu OTHER
  }

  FILE *outstream = stdout;  // Pocatecni hodnota vystupu funkce je STDOUT
  
  if(p.code <= OTHER) {  // Pokud se jedna o chybu
    outstream = stderr;  // Nastavim vystup na STDERR
  }

  if(p.error == NULL) {  // Pokud nebyl nastaven popis chyby
    p.error = "";  // Nastavim jej na prazdny
  }

  // Pokud byla nastavena funkce a vystup pujde na STDERR
  if(p.function != -1 && outstream == stderr) {
    // Vypisi funkci, ve ktere nastala chyba
    fprintf(outstream, "Chyba v parametrech funkce %s:\n", funcs[p.function]);
  }

  if(p.code != FALSE) {  // Pokud neni chyba FALSE
    fprintf(outstream, "%s%s\n", ERRMSG[p.code], p.error);  // Tisknu chybu
  } else {
    printf("%s", ERRMSG[p.code]);  // Jinak tisknu false
  }
}

/**
 * Zpracuje parametry prikazoveho radku a vrati je ve strukture TParams
 * Pokud je format parametru chybny, ukonci program s chybovym kodem
 * @param argc - pocet parametru
 * @param argv - pole textovych retezcu s parametry
 * @return - vraci analyzovane parametry prikazoveho radku
 */
TParams getParams(int argc, char *argv[])
{
  
  TParams result =
  { // Inicializace struktury pro vysledek
   .row = 0,  // Radek nenastaven
   .col = 0,  // Sloupec nenastaven
   .code = OK,  // Kod stavu programu je OK - bez chyby
   .side = -1,  // Smer nenastaven
   .power = 0,  // Sila nenastavena
   .function = -1,  // Nebyla zadana funkce
   .file1 = NULL,   // Prvni soubor nezadan
   .file2 = NULL,   // Druhy soubor nezadan
   .error = NULL,   // Misto chyby
  };
  
  if(argc == 1) {  // Pokud je pocet parametru 1 - nebyl zadan zadny parametr
    result.code = NO_PARAM;  // Chyba
    return result;  // a funkce konci
  } else if(strcmp("-h", argv[1]) == 0) {  // Pokud se jedna o napovedu
    if(argc == 2) {  // Pokud byl zadan jen parametr napovedy
      result.code = HELP;  // vytiskne se napoveda
    } else {  // Pokud bylo zadano vice parametru
      result.code = CMD_COUNT;  // Chyba
    }
    return result;  // Konec funkce
  }
  
  for(int f = F_TEST; f < F_END; f++) {  // Hledam parametrem zadanou funkci
    if(strcmp(funcs[f], argv[1]) == 0) {  // Pokud jsem nasel odpovidajici,
      result.function = f;  // ulozim si jeji kod
      break;  // a koncim cyklus hledani
    }
  }
  
  switch(result.function) {  // Podle zadane funkce si ulozim dalsi parametry
    case F_TEST:       // Pro funkci test
    case F_TRANS: {    // a funkci transpozice
      if(argc != 3) {  // pokud neni pocet parametru 3
        result.code = CMD_COUNT;  // Nastala chyba parametru
      } else {  // Pokud jsou zadany parametry 3
        result.file1 = argv[2];  // Ulozim si nazev prvniho souboru
      }
      break;
    }
    case F_ADD:     // Pro funkci scitani,
    case F_MULT:    // nasobeni
    case F_EXPR: {  // a vyraz
      if(argc != 4) {  // Pokud pocet parametru neni 4
        result.code = CMD_COUNT;  // Chyba parametru
      } else {  // Pokud jsou parametry 4
        result.file1 = argv[2];  // Ulozim si prvni soubor
        result.file2 = argv[3];  // a druhy soubor
      }
      break;
    }
    case F_WATER: {    // Funkce pro vodni tok
      if(argc != 5) {  // Pokud neni zadano 5 parametru
        result.code = CMD_COUNT;  // Chyba
      } else {  // Pokud je pocet parametru spravny
        result.file1 = argv[4];  // Ulozim si ctvrty jako nazev souboru
        // Cyklus kontroly spravneho zadani souradnice radku
        for(int l = (strlen(argv[2])-1); l >= 0; l--) {  // Ctu postupne znaky
          if(isdigit(argv[2][l]) == 0) {  // Pokud znak neni cislice
            result.code = CORD_R_ERROR;   // Nastala chyba v zadane souradnici
            result.error = argv[2];       // Nastavim obsah chyby
            break;                        // Koncim vetev
          }
        }
        // Pokud byla souradnice radku v poradku
        if((result.row = atoi(argv[2])) < 0) {  // Zjistim, zda neni zaporna
          result.code = NEG_CORD_R;  // Chyba zapornosti souradnice
          result.error = argv[2];    // Nastavim hodnotu chyby
          break;                     // Koncim vetev
        }
        // Cyklus kontroly spravneho zadani souradnice sloupce
        for(int l = (strlen(argv[3])-1); l >= 0; l--) {  // Ctu postupne znaky
          if(isdigit(argv[3][l]) == 0) {  // Pokud znak neni cislice
            result.code = CORD_S_ERROR;   // Nastala chyba 
            result.error = argv[3];       // Ulozim si obsah chyby
            break;                        // Koncim vetev
          }
        }
        // Pokud byla souradnice sloupce v poradku
        if((result.col = atoi(argv[3])) < 0) {  // Zjistim, zda neni zaporna
          result.code = NEG_CORD_S;  // Nastavim chybu
          result.error = argv[3];    // Ulozim obsah chyby
        }
      }
      break;
    }
    case F_CAROM: {    // Funkce cesty koule po kulecniku
      if(argc != 7) {  // Pokud nebylo zadano 7 parametru
        result.code = CMD_COUNT;  // Nastala chyba v parametrech
      } else {  // Pokud je zadano 7 parametru
        result.file1 = argv[6];  // Ulozim si nazev souboru
        // Cyklus kontroly spravneho zadani souradnice radku
        for(int l = (strlen(argv[2])-1); l >= 0; l--) {  // Postupne ctu znaky
          if(isdigit(argv[2][l]) == 0) {  // Pokud nektery znak neni cislice
            result.code = CORD_R_ERROR;   // Chyba v parametru radku
            result.error = argv[2];       // Ukladam si chybu
            break;                        // Koncim vetev
          }
        }
        if((result.row = atoi(argv[2])) < 0) {  // Pokud je souradnice zaporna
          result.code = NEG_CORD_R;  // Chyba souradnice radku
          result.error = argv[2];    // Ulozim si chybu
          break;                     // Koncim vetev
        }
        // Cyklus kontroly spravneho zadani souradnice sloupce
        for(int l = (strlen(argv[3])-1); l >= 0; l--) {  // Ctu znaky parametru
          if(isdigit(argv[3][l]) == 0) {  // Pokud znak neni cislice
            result.code = CORD_S_ERROR;   // Chyba parametru
            result.error = argv[3];       // Chybu ulozim
            break;                        // Konec vetve
          }
        }
        if((result.col = atoi(argv[3])) < 0) {  // Kdyz je souradnice zaporna
          result.code = NEG_CORD_S;  // Nastavim priznak teto chyby
          result.error = argv[3];    // Ulozim si obsah chyby
          break;                     // Koncim vetev
        }
        // Cyklus kontroly spravneho zadani smeru odpalu
        for(int side = V; side < S_END; side++) {  // Prochazim retezce smeru
          if(strcmp(sides[side], argv[4]) == 0) {  // Pokud jsem nasel smer
            result.side = side;  // Ulozim si jeho hodnotu
            break;               // a koncim cyklus
          }
        }
        if(result.side == -1) {  // Pokud jsem nenasel smer odpalu
          result.code = BAD_SIDE;  // Chyba zadaneho smeru
          result.error = argv[4];  // Ukladam obsah
          break;                   // Koncim vetev
        }
        // Cyklus kontroly spravneho zadani sily odpalu
        for(int l = (strlen(argv[5])-1); l >= 0; l--) {  // Ctu znaku sily
          if(isdigit(argv[5][l]) == 0) {  // Pokud neni nektery znak cislice
            result.code = POWER_ERROR;    // Chyba v zadane sile
            result.error = argv[5];       // Ulozim si obsah sily
            break;                        // Konec vetve
          }
        }
        // Pokud byla zadana sila zaporna
        if((result.power = atoi(argv[5])) < 0) {
          result.code = BAD_POWER;  // Chyba v zadane sile
          result.error = argv[5];   // Ulozim si jeji obsah
        }
      }
      break;
    }
    default: {  // Pokud se doslo k nezname funkci
      result.code = NO_CMD;    // Chyba zadane funkce
      result.error = argv[1];  // Ulozim si jeji obsah
    }
  }

  return result;  // Vracim zpracovane parametry
}

/**
 * Funkce allocMatrix alokuje prostor pro prvky matice v pameti
 * pro strukturu MATRIX
 * @param *m - ukazatel na matici, ve ktere budeme alokovat prostor pro prvky
 * @return   - pokud nastane pri alokaci prostoru chyba, funkce vraci jeji kod,
 *             kdyz zadna chyba nenastane, vraci kod OK
*/
int allocMatrix(MATRIX *m) {
  if(m == NULL) {  // Pokud neni v parametru platna matice
    return OTHER;  // Vraci chybovy kod OTHER, tato skutecnost by za normalnich
  }                // podminek nemela nastat

  if((m->matrix = (int**)malloc(m->rows * sizeof(int*))) == NULL) {
    // Pokud alokace pro pocet radku nebyla uspesna
    return NO_MEM;  // Vraci chybovy kod NO_MEM - malo pameti
  }

  for(int i = 0; i < m->rows; i++) {  // Cyklus pro alokaci sloupcu v radku
    if((m->matrix[i] = (int*)malloc(m->cols * sizeof(int))) == NULL) {
      // Pokud alokace pro pocet sloupcu nebyla uspesna
      return NO_MEM;  // Vraci chybovy kod NO_MEM - malo pameti
    }
  }

  return OK;  // Pokud se alokace zdarila, vracim kod OK - vse je v poradku
}

/**
 * Funkce initMatrix inicializuje strukturu matice. Alokuje prostor pro prvky
 * matice v pameti a zinicializuje vsechny jeji prvky na 1
 * @param *m - ukazatel na matici, kterou inicializujeme
 * @return   - pokud nastane pri inicializaci chyba, funkce vraci jeji kod,
 *             kdyz zadna chyba nenastane, vraci kod OK
*/
int initMatrix(MATRIX *m) {

  int err = OK;  // Pomocna promenna pro kod chyby

  if((err = allocMatrix(m)) != OK) {  // Alokuji prostor pro matici
    return err;  // a pokud nastane chyba, vracim jeji kod
  }

  
  int howMany = m->rows * m->cols;  // Vypoèítám poèet prvkù v matici

  for(int i = 0; i < howMany; i++) {  // Ka¾dý prvek matice inicializuji na 1
    m->matrix[i/m->cols][i%m->cols] = 1;
  }

  return OK;  // Pokud vse probehlo v poradku, vracim kod OK
}

/*
 * Funkce freeMatrix uvolnuje alokovany prostor matice
 * @param *m - matice, po ktere chceme uvolnit prostor
 * Tato funkce nema navratovou hodnotu
 */
void freeMatrix(MATRIX *m) {
  if(m->matrix != NULL) {  // Pokud nebyla zadana matice, nic se neprovede
    for(int i = 0; i < m->rows; i++) {  // Cyklus pro prochazeni radku matice
      free(m->matrix[i]);  // Uvolneni jednoho radku matice
    }

    free(m->matrix);  // Uvolneni poctu radku matice
  }
}

/**
 * Dynamicky alokuje prostor v pameti pro matici a nacte do ni data ze souboru
 * vcetne poctu radku a sloupcu
 * @param *m     - ukazatel na matici, do ktere budeme nacitat data ze souboru
 * @param file[] - pole obsahujici retezec s nazvem souboru, ze ktereho cteme
 * @return       - pokud nastane pri alokaci prostoru chyba, funkce vraci jeji
 *                 kod, kdyz zadna chyba nenastane, vraci kod OK
 */
int loadMatrix(MATRIX *m, char file[]) {
  FILE *f;      // Ukazatel na soubor
  int howMany;  // Promenna pro pocet prvku matice

  if((f = fopen(file, "r")) == NULL) {  // Otevreni souboru a test jeho otevreni
    return F_OPEN;  // Pokud se nepodarilo soubor otevrit, vraci kod F_OPEN
  }

  if((fscanf(f, "%d", &m->rows)) == EOF) {  // Nacteni poctu radku matice ze
                                            // souboru a test na konec souboru
    fclose(f);        // Pri chybe uzavru soubor
    return BAD_FMAT;  // Pokud se nepodarilo nacist pocet radku, vraci BAD_FMAT
  }

  if(m->rows < 1) {  // Pokud je pocet radku mensi jak 1, nejedna se o matici
    fclose(f);       // Uzavru soubor
    return NO_MAT;   // Vraci kod NO_MAT - nejedna se o matici
  }

  if((fscanf(f, "%d", &m->cols)) == EOF) {  // Nacteni poctu sloupcu matice ze
                                            // souboru a test na konec souboru
    fclose(f);        // Pokud nebyl nacten pocet, uzavru soubor
    return BAD_FMAT;  // Pokud se nepodari nacist pocet sloupcu, vraci BAD_FMAT
  }

  if(m->cols < 1) {  // Pokud je pocet sloupcu mensi jedne, nejedna se o matici
    fclose(f);       // Uzavru soubor
    return NO_MAT;   // Vraci kod NO_MAT - nejedna se o matici
  }

  if(allocMatrix(m) != OK) {  // Alokace prostoru pro prvky matice
    fclose(f);      // Pri chybe uzavru soubor
    return NO_MEM;  // Pokud se nepodarilo prostor alokovat, vraci NO_MEM
  }

  howMany = m->rows * m->cols;  // Pokud je doposud vse bez problemu, spocitam,
                                // kolik bude prvku v matici

  for(int i = 0; i < howMany; i++) {  // Cyklus nacitani prvku matice ze souboru
    if((fscanf(f, "%d", &m->matrix[i/m->cols][i%m->cols])) == EOF) {
      // Pokud nastal konec souboru driv, nez se nacetli vsechny prvky
      fclose(f);        // Soubor uzavru
      return BAD_FMAT;  // Vraci chybovy kod BAD_FMAT - spatne zadana matice
    }
  }
  
  if((fscanf(f, "%d", &howMany)) != EOF) {
    // Pokud jsou v souboru data navic
    fclose(f);        // Uzavru soubor
    return BAD_FMAT;  // Vraci chybovy kod BAD_FMAT - spatne zadana matice
  }

  if(fclose(f) == EOF) { // Uzavreni souboru a test uzavreni
    return F_CLOSE;  // Pokud se nepodarilo soubor uzavrit, vraci kod F_CLOSE
  }

  return OK;  // Pokud funkce probehla bez problemu, vraci OK
}

/**
 * Funkce printMatrix vypisuje pocet radku, sloupcu a prvky matice
 * @param *m - ukazatel na matici, kterou budeme vypisovat
 * Tato funkce nema navratovou hodnotu
 */
void printMatrix(MATRIX *m) {
  int howMany = m->rows*m->cols;  // Promenna pro pocet prvku matice

  printf("%d %d\n", m->rows, m->cols);  // Vypis poctu radku a sloupcu matice

  for(int i = 0; i < howMany; i++) {  // Cyklus vypisu prvku matice po radcich
    printf("%d", m->matrix[i/m->cols][i%m->cols]);  // Vypis jednoho prvku

    if((i%m->cols) == (m->cols - 1)) {  // Pokud jsem na konci radku
      printf("\n");  // Vypisuji znak konce radku
    } else {  // Kdyz nejsem na konci radku
      printf(" ");  // Vypisuji mezeru mezi cisly
    }
  }
}

/**
 * Funkce addMatrix secte dve matice o stejnych rozmerech,
 * vysledek ulozi do matice treti
 * @param *mFirst  - ukazatel na 1. matici pro secteni
 * @param *mSecond - ukazatel na 2. matici pro secteni
 * @param *mResult - ukazatel na matici, ve ktere bude vysledek
 * @return - pokud nastane chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
 */
int addMatrix(MATRIX *mFirst, MATRIX *mSecond, MATRIX *mResult) {
  int howMany;  // Promenna pro pocet prvku matice

  if(mFirst == NULL || mSecond == NULL || mResult == NULL) {  // Test parametru
    return OTHER;  // Pokud nektera matice neodpovida, vraci OTHER,
                   // neznama chyba - nemela by za normalnich podminek nastat
  }

  if(mFirst->cols != mSecond->cols) {  // Pokud nemaji stejny pocet sloupcu
    //return BAD_MAT;  // Vraci kod BAD_MAT - spatny rozmer matice
    return FALSE;  // change
  }
  if(mFirst->rows != mSecond->rows) {  // Pokud nemaji matice stejny pocet radku
    //return BAD_MAT;  // Vraci kod BAD_MAT - spatny rozmer matice
    return FALSE;  // change
  }

  mResult->rows = mFirst->rows;  // Matice mResult s vysledkem ma stejny rozmer
  mResult->cols = mFirst->cols;  // jako obe matice, proto muzu nastavit
                                 // parametry teto matice z matice prvni

  if(allocMatrix(mResult) != OK) {  // Pokud bylo doposud vse v poradku, alokuji
                                    // prostor pro prvky matice s vysledkem
    return NO_MEM;  // Kdyz se alokace nezdari, vracim kod NO_MEM - malo pameti
  };

  howMany = mResult->rows * mResult->cols;  // Pokud je dosud vse bez problemu,
                                            // vypocitam pocet prvku v matici

  for(int i = 0; i < howMany; i++) {  // Cyklus pro secitani prvku prvni matice
                                      // s prvky druhe matice do matice mResult
    mResult->matrix[i/mResult->cols][i%mResult->cols]   =
      mFirst->matrix[i/mFirst->cols][i%mFirst->cols]    +
      mSecond->matrix[i/mSecond->cols][i%mSecond->cols];
  }

  return OK;  // Pokud funkce predcasne neskoncila, vracim kod OK - bez chyby
}

/**
 * Funkce mulMatrix vynasobi dve matice o spravnych rozmerech a vysledek ulozi
 * do treti matice mResult
 * @param *mFirst  - ukazatel na 1. matici pro nasobeni
 * @param *mSecond - ukazatel na 2. matici pro nasobeni
 * @param *mResult - ukazatel na matici s vysledkem
 * @return - pokud nastane chyba, funkce vraci jeji kod,
 *           kdyz zadna chyba nenastane, vraci kod OK
 */
int mulMatrix(MATRIX *mFirst, MATRIX *mSecond, MATRIX *mResult) {
  int howMany;

  if(mFirst->cols != mSecond->rows) {  // Pokud pocet sloupcu prvni matice
                                       // neodpovidaji poctu radku druhe matice
    //return BAD_MAT;  // Vraci kod BAD_MAT - spatny rozmer matice
    return FALSE;  // change
  }

  mResult->rows = mFirst->rows;   // Pokud pocet sloupcu a radku je spravny,
  mResult->cols = mSecond->cols;  // nastavim vysledne matici odpovidajici
                                  // rozmery

  if(allocMatrix(mResult) != OK) {  // Pokud bylo doposud vse v poradku, alokuji
                                    // prostor pro prvky matice s vysledkem
    return NO_MEM;  // Kdyz se alokace nezdari, vracim kod NO_MEM - malo pameti
  };

  howMany = mResult->rows * mResult->cols;  // Pokud je dosud vse bez problemu,
                                            // vypocitam pocet prvku v matici

  for(int i = 0; i < howMany; i++) {  // Cyklus pro prochazeni radku matice
    mResult->matrix[i/mResult->cols][i%mResult->cols] = 0;  // Pro pricitani
                                                            // musim nastavit
                                                            // pocatecni nulu
    for(int j = 0; j < mFirst->cols; j++) {  // Cyklus pro soucet soucinu prvku
      mResult->matrix[i/mResult->cols][i%mResult->cols] += 
        mFirst->matrix[i/mResult->cols][j]  *
        mSecond->matrix[j][i%mResult->cols];
    }
  }

  return OK;  // Funkce konci s navratovou hodnotou kodu OK - bez chyby
}

/*
 * Funkce transMatrix provadi transpozici matice
 * @param *m - matice, kterou chceme transponovat
 * @param *n - matice s vysledkem transpozice 
 * @return   - pokud nastane chyba, funkce vraci jeji kod,
 *             kdyz zadna chyba nenastane, vraci kod OK
 */
int transMatrix(MATRIX *in, MATRIX *out) {
  int count = in->cols * in->rows;  // Vypocitam pocet prvku v matici
  out->rows = in->cols;  // Nastavim vysledne matici pocet radku
  out->cols = in->rows;  // Nastavim vysledne matici pocet sloupcu
  
  if(allocMatrix(out) != OK) { // Pokud bylo doposud vse v poradku, alokuji
                             // prostor pro prvky matice s vysledkem
    return NO_MEM;  // Kdyz se alokace nezdari, vracim kod NO_MEM - malo pameti
  };
  
  for(int i = 0; i < count; i++) {  // Cyklus transponovani matice do vysledne
    out->matrix[i%in->cols][i/in->cols] = in->matrix[i/in->cols][i%in->cols];
  }
  
  return OK;  // Funkce konci s navratovou hodnotou kodu OK - bez chyby
}

/*
 * Funkce exprMatrix provadi vypocet vyrazu: A * trans(B) + A
 * @param *one    - matice A
 * @param *two    - matice B
 * @param *result - vysledna matice 
 * @return        - pokud nastane chyba, funkce vraci jeji kod,
 *                  kdyz zadna chyba nenastane, vraci kod OK
 */
int exprMatrix(MATRIX *one, MATRIX *two, MATRIX *result) {

  MATRIX transB;  // Pomocna promenna pro transponovani matice B
  MATRIX mul;     // Pomocna promenna pro nasobeni matic
  int Err = OK;   // Pomocna promenna pro detekci chyby
  
  // Provedu nejdrive transpozici matice B
  if((Err = transMatrix(two, &transB)) != OK) {  // Pokud se operace nepodari
    return Err;  // Vracim kod chyby
  }
  
  // Dale provedu vynasobeni matic
  if((Err = mulMatrix(one, &transB, &mul)) != OK) {  // Pokud se operace nezdari
    return Err;  // Vracim kod chyby
  }
  
  // Posledni krok je soucet matic
  if((Err = addMatrix(one, &mul, result)) != OK) {  // Provedu kontrolu operace
    return Err;  // Vracim kod chyby, pokud nastala
  }
  
  // Uvolnim prostor v pameti po pomocnych maticich
  freeMatrix(&transB);
  freeMatrix(&mul);
  
  return OK;  // Funkce konci s navratovou hodnotou kodu OK - bez chyby

}

/*
 * Funkce ballMatrix provadi analyzu cesty kulecnikove koule
 * @param *m    - matice reprezentujici kulecnikovy stul
 * @param side  - pocatecni smer koule
 * @param force - sila, kterou se koule pohybu (pocet prochazenych poli)
 * @param row   - radek matice, kde se nachazi koule na pocatku
 * @param col   - sloupec matice, kde se nachazi koule na pocatku
 * @return      - pokud nastane chyba, funkce vraci jeji kod,
 *                kdyz zadna chyba nenastane, vraci kod OK
 */
int ballMatrix(MATRIX *m, int side, int power, int row, int col) {

  int position = CENTER;  // Inicializace pomocne promenne pro pozici koule
  
  if(row >= m->rows || col >= m->cols) {  // Pokud souradnice nejsou v matici
    //return BAD_CORDS;  // jedna se o chybu
    return FALSE;  // change    
  }
  
  // Pokud mam pouze jeden radek v matici a chci se pohybovat jinam nez na V a Z
  if(m->rows == 1 && side != V && side != Z) {
    for(; power > 0; power--) {  // Nemam se kam pohnout
      if(power > 1) {
        printf("%d ", m->matrix[row][col]);  // a vypisuji jen hodnotu na pozici
      } else {
        printf("%d", m->matrix[row][col]);  // a vypisuji jen hodnotu na pozici
      }
    }
  }
  
  // Pokud mam pouze jeden sloupec v matici a chci se pohybovat na V nebo Z
  if(m->cols == 1 && (side == V || side == Z)) {
    for(; power > 0; power--) {  // Nemam se kam pohnout
      if(power > 1) {
        printf("%d ", m->matrix[row][col]);  // a vypisuji jen hodnotu na pozici
      } else {
        printf("%d", m->matrix[row][col]);  // a vypisuji jen hodnotu na pozici
      }
    }
  }

  while(power > 0) {  // Dokud koule nevycerpala vsechnu silu, pokracuje cesta

    if(power > 1) {
      printf("%d ", m->matrix[row][col]);  // Vypisi hodnotu na pozici koule
    } else {
      printf("%d", m->matrix[row][col]);  // Vypisi hodnotu na pozici koule
    }
    
    if(row == 0) {  // Pokud je koule na radku nula
      position = WALL_4;  // Lezi u horni steny
    } else if(row == (m->rows-1)) {  // Pokud je koule na poslednim radku
      position = WALL_2;  // Lezi u spodni steny
    }
    
    if(col == 0) {  // Pokud je koule na sloupci nula
      position += WALL_3;  // Prictu k jeji aktualni pozici levou stenu
    } else if(col == (m->cols-1)) {  // Pokud je koule na poslednim sloupci
      position += WALL_1;  // Prictu k jeji pozici pravou stenu
    }
  
    switch(side) {  // Podle smeru, kterym se ma koule pohybovat, vyberu vetev
      case V: {  // Kdyz se ma pohybovat na vychod

        if(position == WALL_1 || position == CORN_A || position == CORN_B) {
          // Pokud narazila koule na pravou stenu nebo nektery pravy roh
          side = Z;  // Bude se dale pohybovat na zapad
          col--;  // Zaroven se posune o jeden sloupec na zapad
        } else {  // Pokud na pravou stenu nenarazila
          col++;  // Posune se o sloupec na vychod
        }
        break;
      }
      case Z: {  // Kdyz se ma pohybovat na zapad

        if(position == WALL_3 || position == CORN_C || position == CORN_D) {
          // Pokud narazila koule na levou stenu nebo nektery levy roh
          side = V;  // Bude se dale pohybovat na vychod
          col++;  // Zaroven se posune o jeden sloupec na vychod
        } else {  // Pokud na levou stenu nenarazila
          col--;  // Posune se o sloupec na zapad
        }
        break;
      }
      case SV: {  // Kdyz se ma pohybovat na severo-vychod

        if(m->cols == 1 && row == 0 && col == 0) {  // Pokud mam jeden
        // sloupec a jsem na souradnicich 0,0
          side = JV;  // Vzdy se posunu na JV
          row++;  // Posune se o radek nize
        } else if(m->cols == 1 && row == (m->rows-1)) {  // Pokud mam jeden
        // sloupec a jsem na poslednim radku
          if(row % 2 == 1) {  // Pokud je lichy
            side = SZ;  // posouvam se na SZ
          }
          row--;  // posunu se o radek vyse
        } else if(m->cols == 1 && row != (m->rows-1)) {  // Pokud mam jeden
        // sloupec a jsem uprostred nej
          if(row % 2 == 1) {  // Pokud je lichy
            side = SZ;  // Pujdu dale na SZ
          }
          row--;  // Posunu se na SZ
        } else if(position == WALL_1 && (row % 2 == 1)) {
          // Pokud je jeji pozice prava stena a zaroven lichy radek
          side = SZ;  // Bude se dale pohybovat na severo-zapad
          row--;  // Posune se tedy o jede radek vyse
        } else if(position == WALL_4 || position == CORN_D ||
                  position == CORN_A) {
          // Pokud je koule u horni steny nebo v nekterem z hornich rohu
          side = JV;  // Bude se dale pohybovat na jiho-vychod
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // Pokud je na sudem radku
            row++;  // Posune se pouze o radek nize
          } else {  // Pokud je na radku lichem
            row++;  // Posune se o radek nize
            col++;  // a o sloupec vpravo
          }
        } else if(position == CORN_B) {
          // Pokud je koule v pravem dolnim rohu
          if(row % 2 == 1) {  // a na lichem radku
            side = SZ;  // Bude se dale pohybovat na severo-zapad
          }
          row--;  // Musi se tedy presunout o radek vyse
        } else {  // Pokud je koule na jine pozici
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // a zaroven na sudem radku
            row--;  // Posune se o radek vyse
          } else {  // Kdyz je na radku lichem
            row--;  // Posune se o radek vyse
            col++;  // a o sloupec vpravo
          }
        }
        break;
      }
      case SZ: {  // Kdyz se ma pohybovat na severo-zapad

        if(m->cols == 1 && row == 0 && col == 0) {  // Pokud mam jeden sloupec
        // a jsem na souradnicich 0,0
          side = JV;  // Vzdy se posunu na JV
          row++;  // Posune se o radek nize
        } else if(m->cols == 1 && row == (m->rows-1)) {  // Pokud mam jeden
        // sloupec a jsem na poslednim radku
          if(row % 2 == 0) {  // Pokud je sudy
            side = SV;  // posouvam se na SV
          }
          row--;  // posunu se o radek vyse
        } else if(m->cols == 1 && row != (m->rows-1)) {  // Pokud mam jeden
        // sloupec a jsem uprostred nej
          if(row % 2 == 0) {  // Pokud jsem na sudem radku
            side = SV;  // Pujdu dale na SV
          }
          row--;  // Posunu se o pozici na SV
        } else if((position == WALL_3 && (row % 2 == 0)) ||
                  (position == CORN_C && (row % 2 == 0))) {
          // TODO: Optimalizace?
          // Pokud je koule u leve steny nebo v levem dolnim rohu a sudem radku
          side = SV;  // Bude se pohybovat dale na severo-vychod
          if(row % 2 == 0) {  // Pokud je na sudem radku
            row--;  // Posune se o radek vyse
          } else {  // Pokud je na lichem radku
            row--;  // Posune se o radek vyse
            col++;  // a o sloupec vpravo
          }
        } else if(position == WALL_4 || position == CORN_A) {
          // TODO: Optimalizace?
          // Pokud je koule u horni steny nebo v pravem hornim rohu
          side = JZ;  // Bude dale pokracovat na jiho-zapad
          if(row % 2 == 0) {  // Pokud je na sudem radku
            row++;  // Posune se o radek nize
            col--;  // a o sloupec vlevo
          } else {  // Pokud je na lichem radku
            row++;  // Pouze o radek nize
          }
        } else if(position == CORN_D) {
          // TODO: Optimalizace?
          // Pokud je koule v levem hornim rohu
          side = JV;  // Bude se pohybovat na jiho-vychod
          if(row % 2 == 0) {  // Kdyz je na sudem radku
            row++;  // Posune se pouze o radek nize
          } else {  // Pokud je na radku lichem
            row++;  // Posune se o radek nize
            col++;  // a o sloupec vpravo
          }
        } else {  // V jinem pripade se posune ve svem smeru
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // Kdyz je na sudem radku
            row--;  // O radek vyse
            col--;  // a o sloupec vlevo
          } else {  // Z licheho radku
            row--;  // jen o radek vyse
          }
        }
        break;
      }
      case JV: {  // Pokud se ma koule pohybovat na jiho-vychod

        if(m->cols == 1 && row == (m->rows-1)) {  // Pokud mam jeden sloupec
        // a jsem na poslednim radku
          if(row % 2 == 1) {  // Pokud je lichy
            side = SZ;  // posouvam se na SZ
          } else {
            side = SV;  // jinak na SV
          }
          row--;  // posunu se o radek vyse
        } else if(m->cols == 1 && row != (m->rows-1)) {  // Pokud mam jeden
        // sloupec a jsem uprostred nej
          if(row % 2 == 1) {  // Pokud jsem na lichem radku
            side = JZ;  // zmenim smer na JZ
          }
          row++;  // Posunu se o radek nize
        } else if(position == WALL_1 && (row % 2 == 1)) {
          // Kdyz je u prave steny a na lichem radku
          side = JZ;  // Bude dale smerovat na jiho-zapad
          row++;  // Posune se tedy o jeden radek nize
        } else if(position == WALL_2 || position == CORN_C ||
                  (position == CORN_B && (row % 2 == 0))) {
          // Kdyz koule lezi u spodni steny, v levem dolnim rohu
          // nebo v pravem dolnim rohu a zaroven na sudem radku
          side = SV;  // Bude jeji dalsi smer severo-vychod
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // Kdyz je na sudem radku
            row--;  // Posune se o radek vyse
          } else {  // Z licheho radku
            row--;  // se posune o radek vyse
            col++;  // a o sloupec vpravo
          }
        } else if(position == CORN_B && (row % 2 == 1)) {
          // Pokud lezi v pravem dolnim rohu a je na radku lichem
          side = SZ;  // Zmeni se smer na severo-zapad
          row--;  // Posune se o radek vyse
        } else {  // Pokud koule lezi jinde
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // a je na sudem radku
            row++;  // Posune se o radek nize
          } else {  // Z licheho
            row++;  // se posune o radek nize
            col++;  // a zaroven o sloupec vpravo
          }
        }
        break;
      }
      case JZ: {  // Kdyz smeruje koule na jiho-zapad

        if(row == 0 && col == 0 && m->cols == 1) {  // Pokud mam jeden sloupec
        // a jsem na souradnicich 0,0
          side = JV;  // Vzdy se posunu na JV
          row++;  // Posune se o radek nize
        } else if(m->cols == 1 && row == (m->rows-1)) {  // Pokud mam jeden
        // sloupec a jsem na poslednim radku
          if(row % 2 == 1) {  // Pokud je lichy
            side = SZ;  // posouvam se na SZ
          } else {
            side = SV;  // jinak na SV
          }
          row--;  // posunu se o radek vyse
        } else if(m->cols == 1 && row != (m->rows-1)) {  // Pokud mam jeden
        // sloupec a jsem uprostred nej
          if(row % 2 == 0) {  // Pokud jsem na sudem radku
            side = JV;  // Zmenim smer na JV
          }
          row++;  // Posunu se o radek nize
        } else if(position == WALL_2 || position == CORN_B) {
          // Pokud lezi u dolni steny nebo v pravem spodnim rohu
          side = SZ;  // Zmeni se smer na severo-zapad
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // Kdyz je na sudem radku
            row--;  // Posune se o radek vyse
            col--;  // a sloupec vlevo
          } else {  // Z licheho radku
            row--;  // jen o radek vyse
          }
        } else if((position == WALL_3 && (row % 2 == 0)) || position == CORN_D){
          // Pokud se koule nachazi u leve steny na sudem radku
          // nebo v levem hornim rohu
          side = JV;  // Zmeni se smer na jiho-vychod
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // Ze sudeho radku
            row++;  // se posune o radek nize
          } else {  // Z licheho radku
            row++;  // bude posun o radek nize
            col++;  // a sloupec vpravo
          }
        } else if(position == CORN_C && row % 2 == 0) {
          // Pokud je koule v levem dolnim rohu na sudem radku
          side = SV;  // Zmeni smer na severo-vychod
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // Kdyz je na sudem radku
            row--;  // posune se o radek vyse
          } else {  // Kdyz je na radku lichem
            row--;  // posune se o radek vyse
            col++;  // a o sloupec vpravo
          }
        } else if(position == CORN_C && row % 2 == 1) {
          // Pokud je koule v levem dolnim rohu na lichem radku
          side = SZ;  // Zmeni smer na severo-zapad
          row--;  // posune se o radek vyse
        } else {  // Pokud je na jine pozici
          // TODO: Optimalizace?
          if(row % 2 == 0) {  // Posune se ze sudeho radku
            row++;  // o radek nize
            col--;  // a o sloupec vlevo
          } else {  // Z licheho
            row++;  // pouze o radek nize
          }
        }
        break;
      }
      default: {  // Pokud se ma pohybovat neznamym smerem
        return OTHER;  // nemuze tak ucinit a funkce konci s chybou
        break;  // TODO: Optimalizace?
      }
    }
    power--;  // Po kazdem posunu se snizi sila koule o jedna
    position = CENTER;  // Vynuluje se aktualni pozice na stred matice
  }
  printf("\n");  // Po konci funkce se odradkuje vysledek
  return OK;  // Pri uspesnem konci funkce vraci kod OK - bez chyby
}

/**
 * Popisy sousedicich policek
 */
/* Pro sudy radek

      A  /\  B
      C |  | D
      E  \/  F
*/
/* Pro lichy radek


      V  /\  B
      C |  | D
      E  \/  W
*/

/*
 * Funkce minimum hleda nejmensi hodnotu v okoli sestiuhelniku
 * @param r  - radek sestiuhelniku
 * @param s  - sloupec sestiuhelniku  
 * @param *m - matice reprezentujici kulecnikovy stul
 * @return   - vraci nejmesi hodnotu v okoli
 */
int minimum(int r, int s, MATRIX *m) {
  int min = INT_MAX;  // Nastavim na pocatku minimum na nejvetsi hodnotu int

  if(r-1 >= 0) {  // Pokud existuje sousedici policko B
    min = m->matrix[r-1][s];  // Nastavim jej jako nejmensi hodnotu
  }

  if(s-1 >= 0) {  // Pokud existuje sousedici policko C
    if(min == INT_MAX) {  // Pokud jsem jeste nenasel sousedici
      min = m->matrix[r][s-1];  // Nastavim aktualni na nejmensi
    } else if(m->matrix[r][s-1] < min) {  // Jinak hodnotu porovnam
      min = m->matrix[r][s-1];  // a pokud je mensi, tak ulozim
    }
  }

  if(s+1 < m->cols) {  // Pokud existuje sousedici policko D
    if(min == INT_MAX) {  // Pokud jsem jeste nenasel sousedici
      min = m->matrix[r][s+1];  // Nastavim aktualni na nejmensi
    } else if(m->matrix[r][s+1] < min) {  // Jinak hodnotu porovnam
      min = m->matrix[r][s+1];  // a pokud je mensi, tak ulozim
    }
  }

  if(r+1 < m->rows) {  // Pokud existuje sousedici policko F
    if(min == INT_MAX) {  // Pokud jsem jeste nenasel sousedici
      min = m->matrix[r+1][s];  // Nastavim aktualni na nejmensi
    } else if(m->matrix[r+1][s] < min) {  // Jinak hodnotu porovnam
      min = m->matrix[r+1][s];  // a pokud je mensi, tak ulozim
    }
  }
  
  if(r%2 == 0) {
    if(r-1 >= 0 && s-1 >=0) { // Pokud existuje sousedici policko A
      if(min == INT_MAX) {  // Pokud jsem jeste nenasel sousedici
        min = m->matrix[r-1][s-1];  // Nastavim aktualni na nejmensi
      } else if(m->matrix[r-1][s-1] < min) {  // Jinak hodnotu porovnam
        min = m->matrix[r-1][s-1];  // a pokud je mensi, tak ulozim
      }
    }
    
    if(r+1 < m->rows && s-1 >= 0) { // Pokud existuje sousedici policko E
      if(min == INT_MAX) {  // Pokud jsem jeste nenasel sousedici
        min = m->matrix[r+1][s-1];  // Nastavim aktualni na nejmensi
      } else if(m->matrix[r+1][s-1] < min) {  // Jinak hodnotu porovnam
        min = m->matrix[r+1][s-1];  // a pokud je mensi, tak ulozim
      }
    }
  } else {
    if(r-1 >= 0 && s+1 < m->cols) { // Pokud existuje sousedici policko V
      if(min == INT_MAX) {  // Pokud jsem jeste nenasel sousedici
        min = m->matrix[r-1][s+1];  // Nastavim aktualni na nejmensi
      } else if(m->matrix[r-1][s+1] < min) {  // Jinak hodnotu porovnam
        min = m->matrix[r-1][s+1];  // a pokud je mensi, tak ulozim
      }
    }
    
    if(r+1 < m->rows && s+1 < m->cols) { // Pokud existuje sousedici policko W
      if(min == INT_MAX) {  // Pokud jsem jeste nenasel sousedici
        min = m->matrix[r+1][s+1];  // Nastavim aktualni na nejmensi
      } else if(m->matrix[r+1][s+1] < min) {  // Jinak hodnotu porovnam
        min = m->matrix[r+1][s+1];  // a pokud je mensi, tak ulozim
      }
    }
  }

  if(min <= m->matrix[r][s]) {  // Pokud je minimum mensi nebo rovno aktualnimu
    return min;  // Vracim hodnotu minima
  } else {  // Pokud neni mensi
    return INT_MAX;  // Vracim maximum int
  }
}

/*
 * Funkce water pro urceni vodniho toku z pocatecni souradnice pramene
 * @param x       - radek pramene
 * @param y       - sloupec pramene  
 * @param *vstup  - matice reprezentujici vyskovy model
 * @param *vystup - vystupni matice s oznacenym vodnim tokem 
 * Funkce nema navratovou hodnotu
 */
void water(int x, int y, MATRIX *vstup, MATRIX *vystup) {

  vystup->matrix[x][y] = 0;  // Policko, na ktere vstupuji, vzdy znaci tok

  int min = minimum(x, y, vstup);  // Zjistim minimum v okoli aktualni pozice
  
  if(min != INT_MAX) {  // Pokud ma aktualni policko mensiho souseda
    if(x-1 >= 0 &&  // Pokud ma souseda B,
       vstup->matrix[x-1][y] <= min &&  // obsahuje mensi cislo
       vystup->matrix[x-1][y] == 1) {   // a policko jeste nebylo zpracovane
      water(x-1, y, vstup, vystup);     // Hledam v okoli souseda B
    }
  
    if(y-1 >= 0 &&  // Pokud ma souseda C,
       vstup->matrix[x][y-1] <= min &&  // obsahuje mensi cislo
       vystup->matrix[x][y-1] == 1) {   // a policko jeste nebylo zpracovane
      water(x, y-1, vstup, vystup);     // Hledam v okoli souseda C
    }
  
    if(y+1 < vstup->cols &&  // Pokud ma souseda D,
       vstup->matrix[x][y+1] <= min &&  // obsahuje mensi cislo
       vystup->matrix[x][y+1] == 1) {   // a policko jeste nebylo zpracovane
      water(x, y+1, vstup, vystup);     // Hledam v okoli souseda D
    }
  
    if(x+1 < vstup->rows &&  // Pokud ma souseda F,
       vstup->matrix[x+1][y] <= min &&  // obsahuje mensi cislo
       vystup->matrix[x+1][y] == 1) {   // a policko jeste nebylo zpracovane
      water(x+1, y, vstup, vystup);     // Hledam v okoli souseda F
    }
    
    if(x%2 == 0) {
      if(x-1 >= 0 &&
         y-1 >= 0 &&  // Pokud ma souseda A,
         vstup->matrix[x-1][y-1] <= min &&  // obsahuje mensi cislo
         vystup->matrix[x-1][y-1] == 1) {   // a policko jeste nebylo zpracovane
      water(x-1, y-1, vstup, vystup);     // Hledam v okoli souseda A
      }
    
      if(x+1 < vstup->rows &&
         y-1 >= 0 &&  // Pokud ma souseda E,
         vstup->matrix[x+1][y-1] <= min &&  // obsahuje mensi cislo
         vystup->matrix[x+1][y-1] == 1) {   // a policko jeste nebylo zpracovane
        water(x+1, y-1, vstup, vystup);     // Hledam v okoli souseda E
      }
    } else {
      if(x-1 >= 0 &&
         y+1 < vstup->cols &&  // Pokud ma souseda V,
         vstup->matrix[x-1][y+1] <= min &&  // obsahuje mensi cislo
         vystup->matrix[x-1][y+1] == 1) {   // a policko jeste nebylo zpracovane
      water(x-1, y+1, vstup, vystup);     // Hledam v okoli souseda V
      }
    
      if(x+1 < vstup->rows &&
         y+1 < vstup->cols &&  // Pokud ma souseda W,
         vstup->matrix[x+1][y+1] <= min &&  // obsahuje mensi cislo
         vystup->matrix[x+1][y+1] == 1) {   // a policko jeste nebylo zpracovane
        water(x+1, y+1, vstup, vystup);     // Hledam v okoli souseda W
      }
    }
  }
}

/*
 * Funkce obalujici funkci water pro urceni vodniho toku
 * z pocatecni souradnice pramene
 * @param x       - radek pramene
 * @param y       - sloupec pramene  
 * @param *vstup  - matice reprezentujici vyskovy model
 * @param *vystup - vystupni matice s oznacenym vodnim tokem 
 * @return - vraci kod stavu/chyby funkce
 */
int voda(int x, int y, MATRIX *vstup, MATRIX *vystup) {
  
  int err = OK;  // Pomocna promenna pro ukladani stavu/chyby funkci
  
  if(x >= vstup->rows || y >= vstup->cols) {  // Kdyz souradnice nejsou v matici
    //return BAD_CORDS;  // jedna se o chybu
    return FALSE;  // change
  }
  
  // Nastavim rozmery vystupni matice na rozmery matice vstupni
  vystup->rows = vstup->rows;  // Radky
  vystup->cols = vstup->cols;  // Sloupce
  
  // Inicializuji matici pro vystup
  if((err = initMatrix(vystup)) != OK) {
    return err;  // Pokud nastala chyba, vracim jeji kod
  }
  
  // Po kontrole zavolam funkci water pro samotne hledani cesty
  water(x, y, vstup, vystup);
  
  return err;  // Pokud se dostanu az sem, mel bych vratit kod OK
}

/*
 * Funkce main
 * @param argc    - pocet argumentu prikazove radky
 * @param *argv[] - argumenty prikazove radky
 * @return        - pokud program skoncil dobre, vraci EXIT_SUCCESS,
 *                  v opacnem pripade EXIT_FAILURE 
 */
int main(int argc, char *argv[])
{
  // Zjistim si parametry 
  TParams params = getParams(argc, argv);
  
  // Inicializuji matice pro beh programu
  MATRIX first, second, third = {0, 0, NULL};
  first = second = third;
  
  // Pokud je kod parametru jiny nez OK
  if(params.code != OK) {
    printErr(params);  // Vytisknu kod/stav programu
    if(params.code < HELP) {  // Pokud se nejednalo o napovedu
      return EXIT_FAILURE;  // Navratovy kod je chybovy
    } else {
      return EXIT_SUCCESS;  // Jinak uspesny
    }
  }
  
  // Podle parametru funkce spustim prislusnou funkci
  switch(params.function) {
    case F_TEST: {  // Test spravnosti matice
      params.code = loadMatrix(&third, params.file1);  // Zkusim matici nacist
      params.error = params.file1;  // Ulozim si nazev souboru pro pripad chyby
      break;
    }
    case F_ADD: {  // Soucet matic
      // Zkusim matici A nacist
      if((params.code = loadMatrix(&first, params.file1)) != OK) {
        params.error = params.file1;  // Ulozim si nazev souboru pro chybu
        break;
      }
      // Zkusim matici B nacist
      if((params.code = loadMatrix(&second, params.file2)) != OK) {
        params.error = params.file2;  // Ulozim si nazev souboru pro chybu
        break;
      }
      params.code = addMatrix(&first, &second, &third);  // Sectu matice
      break;
    }
    case F_MULT: {  // Nasobeni matic
      // Zkusim nacist matici A
      if((params.code = loadMatrix(&first, params.file1)) != OK) {
        params.error = params.file1;  // Ulozim si nazev souboru pro chybu
        break;
      }
      // Zkusim nacist matici B
      if((params.code = loadMatrix(&second, params.file2)) != OK) {
        params.error = params.file2;  // Ulozim si nazev souboru pro chybu
        break;
      }
      params.code = mulMatrix(&first, &second, &third);  // Nasobim matice
      break;
    }
    case F_TRANS: {  // Transpozice matice
      // Nactu vstupni matici
      if((params.code = loadMatrix(&first, params.file1)) != OK) {
        params.error = params.file1;
        break;
      }
      params.code = transMatrix(&first, &third);  // Provedu transpozici
      break;
    }
    case F_EXPR: {  // Maticovy vyraz
      // Nactu vstupni matici A
      if((params.code = loadMatrix(&first, params.file1)) != OK) {
        params.error = params.file1;  // Ulozim si nazev souboru pro chybu
        break;
      }
      // Nactu vstupni matici B
      if((params.code = loadMatrix(&second, params.file2)) != OK) {
        params.error = params.file2;  // Ulozim si nazev souboru pro chybu
        break;
      }
      params.code = exprMatrix(&first, &second, &third);  // Vypocitam vyraz
      break;
    }
    case F_WATER: {  // Vodni tok
      // Nactu vstupni matici
      if((params.code = loadMatrix(&first, params.file1)) != OK) {
        params.error = params.file1;
        break;
      }
      params.code = voda(params.row, params.col, &first, &third);  // Hledam tok
      params.error = params.file1;  // Ulozim si nazev souboru pro chybu
      break;
    }
    case F_CAROM: {  // Kulecnikova koule
      // Nactu vstupni matici
      if((params.code = loadMatrix(&first, params.file1)) != OK) {
        params.error = params.file1;  // Ulozim si nazev souboru pro chybu
        break;
      }
      params.code = ballMatrix(&first, params.side, params.power,
                               params.row, params.col);  // Hledam cestu koule
      params.error = params.file1;  // Ulozim si nazev souboru pro chybu
      break;
    }
    default: {  // Pokud se dostanu do teto vetve
      params.code = OTHER;  // Musela nastat neosetrena chyba
    }
  }
  
  if(params.code != OK) {  // Pokud je kod chybovy
    printErr(params);  // Tisknu chybu
    freeMatrix(&first);  // a uvolnim misto po maticich
    freeMatrix(&second);
    freeMatrix(&third);
    return EXIT_FAILURE;  // Konec programu s chybou
  }
  
  if(params.function != F_CAROM) {  // Pokud nebyla zadana funkce kulecnik
    printMatrix(&third);  // Vypisuji matici s vysledkem
  }

  // Uvolnim prostredky po maticich
  freeMatrix(&first);
  freeMatrix(&second);
  freeMatrix(&third);

  return EXIT_SUCCESS;  // Konec programu bez chyby
}
