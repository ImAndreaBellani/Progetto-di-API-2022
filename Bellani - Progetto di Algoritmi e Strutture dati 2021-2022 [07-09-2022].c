/*

	****************************************************
 	*                                                  *
	* Progetto di Algoritmi e Strutture dati 2021-2022 *
	*                                                  *
	*          realizzato da: Andrea Bellani           *
	*                                                  *
	*            scritto con: Sublime Text             *
	*    compilato con: GCC 11 su Ubuntu on Windows    *
	*                                                  *
	****************************************************

	-> iniziato il:                                                                                     04/08/2022
	-> prima versione totalmente funzionante:                                                           12/08/2022 alle 01:05
	-> prima sottoposizione al verificatore:                                                            15/08/2022 alle 11:56
	-> prima sottoposizione OPEN passata con successo:                                                  15/08/2022 alle 12:30 [5a   sottoposizione]
	-> prima sottoposizione UPTO18 passata con successo:                                                30/08/2022 alle 12:33 [518a sottoposizione]
	-> prima sottoposizione UPTO30 passata con successo in almeno un subtask:                           30/08/2022 alle 16:46 [52a  sottoposizione]
	-> prima sottoposizione UPTO30 passata con successo in tutti i subtask:                             31/08/2022 alle 21:15 [81a  sottoposizione]
	-> prima sottoposizione CUMLAUDE passata con successo:                                              31/08/2022 alle 21:17 [32a  sottoposizione]
	-> prima sottoposizione che ha passato con successo tutti i test (OPEN, UPTO18, UPTO30, CUMLAUDE):  31/08/2022 alle 23:07
	-> ultimato il:                                                                                     07/09/2022
	
	numero di sottoposizioni fatte:
	-> OPEN:      114
	-> UPTO18:    556
	-> UPTO30:    101
	-> CUMLAUDE:   53

	riassunto del codice:
	-> il vocabolario delle parole ammissibili è implementato come un dizionario gestito in
	   closed hashing. La funzione di hash utilizza il double hashing (ove la funzione di hash usata per
	   il probing restituisce sempre un valore dispari, in modo da essere sicuri di coprire tutti i bucket)
	   e le due "sottofunzioni" di hash utilizzano entrambe il metodo della moltiplicazione (tutte queste funzioni
	   sono state illustrate dal Professor Barenghi all'interno delle lezioni di Algoritmi e Principi
	   dell'Informatica);
	-> per "hashare" le stringhe, alle funzioni di hash viene dato un "surrogato" della stringa sottoforma di intero
	   (che nel codice è chiamato "magic"). Questo numero è un intero a 64 bit che possiamo vedere come un vettore di "blocchi" da 6
	   bit ciasuno ("b1", "b2", ... , "b10"). Per una generica stringa di "k" caratteri "s", "bi" è la "conversione in
	   intero" tramite la funzione "map" dell'i-esimo carattere di "s". Se "s" ha più di "10" caratteri, quelli non
	   verranno considerati nel calcolo di "magic". Questo porta a innumerevoli collisioni nel momento in cui tutte le stringhe
	   hanno uguali i primi 10 caratteri, ma questo non rappresenta un problema per i test del verificatore. Per sopperire alla cosa
	   si sarebbe potuto utilizzare un vettore di variabili da 64 bit allocato dinamicamente, ma i tempi di gestione sarebbero
	   notevolmente aumentati e ho preferito evitare;
	-> ogni stringa del vocabolario è memorizzata 2 volte, la prima nel dizionario ("d.strings") e la seconda in un vettore
	   ("d.strings_v"). Quest'ultimo è gestito come una coda, ove l'indice "d.cand_word" memorizza quante delle stringhe della
	   coda sono ancora "valide", ovvero rispettano ancora i vincoli indovinati in quella partita (se, ad esempio, "d.cand_word = 5" significa
	   che solo le prime 5 stringhe della coda rispettano ancora i vincoli indovinati fino a quel momento). In questo modo le funzioni
	   "print_filtered" e "rescan_records" lavorano unicamente su quelle stringhe;
	-> i vincoli indovinati vengono memorizzati in 3 strutture diverse:
		-> right_in_place : un vettore di "k" caratteri che all'i-esima posizione contiene il carattere che si è indovinato
		                    essere all'i-esima posizione;
		-> right_not_in_place : un vettore di 64 elementi ove l'elemento all'i-esima posizione è una coppia di due valori,
		                        uno ("oc") che conteggia quanto il carattere avente risultato della funzione "map" uguale a "i" si
		                        è scoperto comparire nella stringa da indovinare e uno ("exact") che indica se "oc" è il numero esatto
		                        di volte in cui quel carattere deve comparire oppure rappresenta solo un limite inferiore;
		-> not_in_place : vettore di "64 * k" flag. Se la flag in posizione "i + k*j" è "true" significa che il carattere avente
						  "i" come risultato di "map()" non può comparire in posizione "j".
	-> l'ordinamento da effettuare prima della "stampa filtrate" è "quick sort" (con partizione di Hoare) dove il pivot è selezionato
	   casualmente (algoritmo presentato a lezione dal Professor Barenghi).
*/

#include <stdio.h>  //fscanf(), fprintf(), fclose()
#include <stdlib.h> //malloc(), realloc(), free()
#include <string.h> //strncpy(), strncmp()
#include <stdint.h> //uint8_t, uint32_t, uint64_t

#define not(x) !(x)
#define and &&
#define or ||
#define true 1
#define false 0

#define ALPHABET_FIRST_LOWER_CHAR   'a'
#define ALPHABET_LAST_LOWER_CHAR    'z'
#define ALPHABET_FIRST_UPPER_CHAR   'A'
#define ALPHABET_LAST_UPPER_CHAR    'Z'
#define ALPHABET_FIRST_NUMBER_CHAR  '0'
#define ALPHABET_LAST_NUMBER_CHAR   '9'
#define ALPHABET_FIRST_SYMBOL_CHAR  '-'
#define ALPHABET_LAST_SYMBOL_CHAR   '_'
#define ALPHABET_LENGTH 64

#define COMMAND_NEW_MATCH        "+nuova_partita"
#define COMMAND_PRINT_FILTERED   "+stampa_filtrate"
#define COMMAND_BEGIN_EXPANSION  "+inserisci_inizio"
#define COMMAND_END_EXPANSION    "+inserisci_fine"
#define COMMAND_STARTER_CHAR '+'
#define COMMAND_MAX_LENGTH 17

#define MARKER_RIGHT_IN_PLACE      '+'
#define MARKER_RIGHT_NOT_IN_PLACE  '|'
#define MARKER_NOT_RIGHT           '/'
#define MARKER_SPECIAL             ' ' //usato in "word_checker()" per evitare di accedere a memoria non inizializzata

#define OUTPUT_NOT_EXISTS    "not_exists"
#define OUTPUT_GUESSED_WORD  "ok"
#define OUTPUT_LOST_MATCH    "ko"

#define SPECIAL_SYMBOL '*'

#define DEFAULT_DICTIONARY_CHARACTER '*'
#define IRRATIONAL 1.6832583426756376 //per avere un buon hashing con metodo della moltiplicazione

#define DEFAULT_DICTIONARY_SIZE     100000 //quanto è grande inizialmente il dizionario
#define REALLOCATE_DICTIONARY_SIZE  500000 //di quanto va espanso nel momento in cui la funzione di hash non trova più posti liberi
#define DEFAULT_ARRAY_SIZE          100000 //quanto è grande inizialmente la coda
#define REALLOCATE_ARRAY_SIZE       500000 //di quanto va espansa la coda nel momento in cui è piena

#define K_LIMIT 10
#define BITS_PER_CHAR 6

#define INVALID_POS -1 //valore restituito da "insert()" e "search()" nel momento in cui, rispettivamente, il dizionario
                       //risulta pieno o l'elemento da cercare non è presente

typedef uint8_t bool;

typedef struct _dict
	{
		int     k; //è comodo che "k" venga memorizzato della struttura del vocabolario così da non doverlo passare in (praticamente) tutte le funzioni
		
		int     size_array_max; //grandezza massima, in quel momento, della coda "d.strings_v" (raggiunta, la coda va reallocata)
		int     size_array_real; //grandezza effettiva, in quel momento, della coda "d.strings_v"
		int     size_dictionary_max; //numero di bucket del dizionario
		
		int     cand_word; //indice che stabilisce quali elementi della coda sono stringhe che rispettano i vincoli indovinati
						   //fino a quel momento e quali no
		
		char *  strings; //dizionario
		char *  strings_v; //coda
	} dictionary; //struttura che memorizza il vocabolario di parole ammissibili (ed altri parametri utili alla sua gestione)

typedef struct _block
	{
		int   oc; //contatore dell'occorrenza di un carattere nella stringa di riferimento indovinata fino a quel momento
		bool  exact; //flag che indica se "oc" indica un numero esatto o solo un limite inferiore
	} occur;
typedef struct _rec
	{ //spiegati nell'introduzione
		char *  right_in_place;
		occur   right_not_in_place[ALPHABET_LENGTH];
		bool *  not_in_place;
	} record; //struttura che memorizza i vincoli indovinati durante la partita

void                      allocate_dictionary      (dictionary *    ,    int                      ); //inizializza le variabili relative al vocabolario
void                      allocate_records         (record *        ,    int                      ); //inizializza le variabili relative ai vincoli indovinati
int                       insert                   (dictionary *    ,    char []                  ); //inserisce una nuova parola nel dizionario
unsigned long long int    magic_calculator         (char []         ,    int                      ); //calcola il valore "magic" di una stringa
int                       map                      (char           ); //calcola un identificativo da "0" a "63" per uno dei simboli dell'alfabeto
int                       hash                     (int             ,    int                       ,    int       ); //funzione di hash che fa double hashing con "h1()" e "h2()"
int                       h1                       (int             ,    unsigned long long int   ); //prima funzione di hash
int                       h2                       (int             ,    unsigned long long int   ); //seconda funzione di hash
void                      reallocate_dictionary    (dictionary *   ); //aumenta i bucket del dizionario reallocandolo
void                      quick_sort               (dictionary *    ,    int                       ,    int       ); //quick_sort visto a lezione
int                       partition                (dictionary *    ,    int                       ,    int       ); //partizione di Hoare vista a lezione
void                      print_filtered           (FILE *          ,    dictionary *             ); //stampa le prime "cand_word" stringhe della coda
void                      reset                    (dictionary *    ,    record *                 ); //resetta tutti le variabili il cui valore ha una valenza solo all'interno di una partita
bool                      check_records            (char []         ,    record *                  ,    int       ); //controlla che una stringa del vocabolario rispetti o meno i vincoli indovinati fino a quel punto
void                      swap                     (dictionary *    ,    int                       ,    int       ); //scambia tra loro due elementi della coda
int                       search                   (dictionary *    ,    char []                  ); //cerca un elemento nel dizionario
void                      word_checker             (char []         ,    char []                   ,    char []    ,    int   ); //restituisce il confronto "+", "|", "/" illustrato nella consegna del progetto
void                      expand_records           (record *        ,    char []                   ,    char []    ,    int   ); //aggiunge ai vincoli già dedotti i nuovi vincoli appena dedotti (dopo il risultato di "word_checker()")
void                      rescan_records           (dictionary *    ,    record *                 ); //sistema la coda affinché abbia nelle prime "cand_word" posizioni solo le stringhe conformi ai vincoli aggiornati

int main (int argc, char * argv[])
	{
		int
			sc; //variabile per memorizzare il valore di ritorno di "fscanf()" (così da evitare il warning per averne ignorato il valore di ritorno)
		FILE
			* file_input, * file_output; //puntatori a file nel caso si vogliano utilizzare file di testo per l'input e/o l'output
		int
			k, k_plus, n_file_string, i, n, p, guessed, pos;
			//n_file_string: lunghezza massima di una stringa in input
			//guessed: flag che indica se la parola da indovinare nella partita è stata indovinata
		bool
			ordered; //indica se gli elementi della coda che rispettano i vincoli sono ordinati (così da non doverli riordinare inutilmente)

		file_input = stdin;
		file_output = stdout;

		sc = fscanf(file_input, "%d", &k); //lettura di "k"

		k_plus = k+1;
		n_file_string = (k > COMMAND_MAX_LENGTH) ? k : COMMAND_MAX_LENGTH;
		char
			the_rif[k+1]; //stringa per la parola da indovinare
				
		dictionary
			d;
		record
			r;

		allocate_dictionary(&d, k);		
		allocate_records(&r, k);

		char
			rif_word[n_file_string+1], output[k+1];
			//rif_word: stringa per memorizzare la generica riga letta dal file
			//output: dove "word_checker()" metterà il risultato del confronto tra tentativo e parola da indovinare

		ordered = false;
		do 
			{
				sc = fscanf(file_input, "%s", rif_word);
				
				if (rif_word[0] != COMMAND_STARTER_CHAR) //se non si è letto un comando (ovvero si è letta una parola da inserire nel vocabolario)
					{
						if (d.size_array_real == d.size_array_max) //se la coda è piena
							{
								d.size_array_max = d.size_array_max + REALLOCATE_ARRAY_SIZE;
								d.strings_v = (char *) realloc(d.strings_v, sizeof(char) * d.size_array_max * k_plus); //si realloca la coda
							}
						strncpy(d.strings_v + d.size_array_real*k_plus, rif_word, k); //si inserisce la nuova parola nella coda
						*(d.strings_v + (d.size_array_real+1)*k_plus - 1) = '\0';

						d.size_array_real++;

						do
							{
								p = insert(&d, rif_word); //si inserisce la parola nel dizionario
								if (p == INVALID_POS)
									{
										reallocate_dictionary(&d); //si continua ad ampliare il dizionario fino a quando l'inserimento non va a buon fine
									}
							}
						while (p == INVALID_POS);
						
						ordered = false;
					}
				else
					{
						if (rif_word[1] == 's') //se si riceve il comando "+stampa_filtrate"
							{
								if (not(ordered)) //se la coda non è ordinata
									{
										quick_sort(&d, 0, d.size_array_real-1); //la si ordina
										ordered = true;
									}
								d.cand_word = d.size_array_real;
								print_filtered(file_output, &d); //si stampa la coda
							}
					}
			}
		while ((sc != EOF) and (strncmp(rif_word, COMMAND_NEW_MATCH, n_file_string) != 0)); //fino a quando non arriva il comando "+nuova_partita"

		guessed = false;
		d.cand_word = d.size_array_real;
		sc = fscanf(file_input, "%s", the_rif);
		sc = fscanf(file_input, "%d", &n);
		sc = fscanf(file_input, "%s", rif_word);
		i = 0; //contatore che indica di quanti tentativi si dispone ancora nella partita corrente

		do
			{
				while (rif_word[0] == COMMAND_STARTER_CHAR) //fino a quando si è letto un comando
					{
						switch (rif_word[1])
							{
								case 's': //se si è letto "+stampa_filtrate"
									if (not(ordered))
										{
											quick_sort(&d, 0, d.cand_word-1); //si ordina solo i primi "cand_word" elementi della coda
											ordered = true;
										}
									print_filtered(file_output, &d); //li si stampa
									sc = fscanf(file_input, "%s", rif_word);
									break;
								case 'n': //se si è letto "+nuova_partita"
									reset(&d, &r); //si resettano i vincoli e si ripristina l'indice della coda
									i = 0;
									ordered = false; //"cand_word" è stato ripristinato, dunque vanno considerati tutti gli elementi della coda
									sc = fscanf(file_input, "%s", the_rif);
									sc = fscanf(file_input, "%d", &n);
									sc = fscanf(file_input, "%s", rif_word);
									guessed = false;
									break;
								default: //se si è letto "+inserisci_inizio"
									do
										{
											sc = fscanf(file_input, "%s", rif_word);
											if (rif_word[0] != COMMAND_STARTER_CHAR)
												{
													//stesso inserimento visto nella prima parte del main
													if (d.size_array_real == d.size_array_max)
														{
															d.size_array_max = d.size_array_max + REALLOCATE_ARRAY_SIZE;
															d.strings_v = (char *) realloc(d.strings_v, sizeof(char) * d.size_array_max * (k_plus));
														}
													strncpy(d.strings_v + d.size_array_real*k_plus, rif_word, k);
													*(d.strings_v + (d.size_array_real+1)*k_plus - 1) = '\0';
													
													if (check_records(d.strings_v + d.size_array_real*k_plus, &r, d.k)) //se la nuova parola rispetta i vincoli
														{
															if (d.size_array_real != d.cand_word)
																swap(&d, d.size_array_real, d.cand_word); //la si fa salire nella parte "alta" della coda
															
															d.cand_word++; //c'è una parola in più che rispetta i vincoli
														}

													d.size_array_real++;

													do
														{
															p = insert(&d, rif_word);
															if (p == INVALID_POS)
																{
																	reallocate_dictionary(&d);
																}
														}
													while (p == INVALID_POS);
													ordered = false; //la coda è stata ampliata, dunque (potenzialmente) è stata disordinata
												}
										}
									while (rif_word[0] != COMMAND_STARTER_CHAR);
									sc = fscanf(file_input, "%s", rif_word);		
							}
					}
				
				if (strncmp(the_rif, rif_word, k) == 0) //se si è indovinata la parola
					{
						fprintf(file_output, "%s\n", OUTPUT_GUESSED_WORD);
						guessed = true;
					}	
				else
					{
						pos = search(&d, rif_word); //si cerca la parola nel vocabolario
						if (pos != INVALID_POS) //se è stata trovata
							{
								word_checker(the_rif, rif_word, output, k); //si calcola il confronto tra lei e la parola da indovinare
								expand_records(&r, rif_word, output, k); //dal confronto calcolato si aggiornano i vincoli
								rescan_records(&d, &r); //si riorganizza la coda in modo che nelle prime "cand_word" posizioni ci siano solo le parole che ancora rispettano i vincoli
								fprintf(file_output, "%s\n", output);
								i++;
								fprintf(file_output, "%d\n", d.cand_word);
								ordered = false;
							}
						else
							{
								fprintf(file_output, "%s\n", OUTPUT_NOT_EXISTS);
							}
					}

				if (i == n) //se è stato raggiunto il limite di tentativi per quella partita
					{
						if (not(guessed)) //se non si è riusciti a indovinare la parola
							{
								fprintf(file_output, "%s\n", OUTPUT_LOST_MATCH);
							}
					}

			}
		while (fscanf(file_input, "%s", rif_word) != EOF);

		fclose(file_output);
		fclose(file_input);
		free(d.strings);
		d.strings = NULL;
		free(d.strings_v);
		d.strings_v = NULL;
		free(r.right_in_place);
		r.right_in_place = NULL;
		free(r.not_in_place);
		r.not_in_place = NULL;
		
		return (0);
	}
void allocate_dictionary (dictionary * d, int k)
	{
		int
			i, k_plus;

		d->k = k;
		d->size_array_max = DEFAULT_ARRAY_SIZE;
		d->size_array_real = 0;
		d->size_dictionary_max = DEFAULT_DICTIONARY_SIZE;
		d->cand_word = 0;
		d->strings = (char *) malloc(sizeof(char) * DEFAULT_DICTIONARY_SIZE * (k+1));
		d->strings_v = (char *) malloc(sizeof(char) * DEFAULT_ARRAY_SIZE * (k+1));

		k_plus = k+1;
		for (i = 0 ; i < DEFAULT_DICTIONARY_SIZE ; i++)
			{
				*(d->strings + i*k_plus) = DEFAULT_DICTIONARY_CHARACTER;
				*(d->strings + (i+1)*k_plus -1) = '\0';
			}

		return;
	}
void allocate_records (record * r, int k)
	{
		int
			i,j;

		r->right_in_place = (char *) malloc(sizeof(char) * k);
		r->not_in_place = (bool *) malloc(k * ALPHABET_LENGTH * sizeof(bool));

		for (i = 0 ; i < k ; i++)
			{
				r->right_in_place[i] = SPECIAL_SYMBOL;
			}
		for (i = 0 ; i < ALPHABET_LENGTH ; i++)
			{
				r ->right_not_in_place[i].oc = 0;
				r ->right_not_in_place[i].exact = false;
				for (j = 0 ; j < k ; j++)
					{
						*(r->not_in_place + i*k + j) = false;
					}
			}

		return;
	}
int insert (dictionary * d, char string[])
	{
		int
			pos, i, k_plus, y;
		unsigned long long int
			magic;
		bool
			found_free_place;

		k_plus = d->k +1;
		found_free_place = false;
		magic = magic_calculator(string, d->k);
		y = hash(magic, 0, d->size_dictionary_max);
		i = 0;
		do
			{
				pos = hash(magic, i, d->size_dictionary_max);
				if (*(d->strings + pos*k_plus) == DEFAULT_DICTIONARY_CHARACTER)
					{
						strncpy(d->strings + pos*(k_plus), string, d->k);
						*(d->strings + (pos + 1)*(k_plus) - 1) = '\0';
						found_free_place = true;
					}
				i++;
			}
		while (((not(found_free_place))) and (hash(magic, i, d->size_dictionary_max) != y));

		if (not(found_free_place))
			pos = INVALID_POS;

		return (pos);
	}
unsigned long long int magic_calculator (char string[], int k)
	{
		int
			i;

		unsigned long long int
			x;

		x = 0xFFFFFFFFFFFFFFFF;
		if (k > K_LIMIT)
			k = K_LIMIT;

		for (i = k-1 ; i >= 0 ; i--)
			{
				x = x<<BITS_PER_CHAR;
				x = x|(map(*(string + i)));	
			}
		
		return (x);
	}
int map (char c)
	{
		if (c == ALPHABET_FIRST_SYMBOL_CHAR)
			{
				return (0);
			}
		else
			{
				if ((c >= ALPHABET_FIRST_NUMBER_CHAR) and (c <= ALPHABET_LAST_NUMBER_CHAR))
					{
						return (c - ALPHABET_FIRST_NUMBER_CHAR + 1);
					}
				else
					{
						if ((c >= ALPHABET_FIRST_UPPER_CHAR) and (c <= ALPHABET_LAST_UPPER_CHAR))
							{
								return (c - ALPHABET_FIRST_UPPER_CHAR + 11);
							}
						else
							{
								if ((c >= ALPHABET_FIRST_LOWER_CHAR) and (c <= ALPHABET_LAST_LOWER_CHAR))
									{
										return (c - ALPHABET_FIRST_LOWER_CHAR + 37 + 1);
									}
								else
									{
										return (ALPHABET_LAST_UPPER_CHAR - ALPHABET_FIRST_UPPER_CHAR + 11 + 1);
									}
							}
					}
			}	
	}
int hash (int magic, int step, int m)
	{
		if (step == 0)
			return(h1(magic, m));
		else
			return ((h1(magic, m) + h2(magic, m)*step)%m);
	}
int h1 (int magic, unsigned long long int m)
	{
		return ((int) (magic * (uint32_t) ((double) IRRATIONAL * ((uint64_t)1<<32)))%m);
	}
int h2 (int magic, unsigned long long int m)
	{
		int
			a;

		a =  (int) ((magic * (uint32_t) ((double) IRRATIONAL * ((uint64_t)1<<32)))%m);
		if (a%2 == 0)
			a++;
		
		return (a);
	}
void reallocate_dictionary (dictionary * d)
	{
		int
			k_plus, i, j, pos;
				
		k_plus = d->k +1;
		d->size_dictionary_max = d->size_dictionary_max + REALLOCATE_DICTIONARY_SIZE;
		d->strings = (char *) realloc(d->strings, d->size_dictionary_max * sizeof(char) * k_plus);

		for (i = 0 ; i < d->size_dictionary_max ; i++)
			{
				*(d->strings + i*k_plus) = DEFAULT_DICTIONARY_CHARACTER;
				*(d->strings + (i+1)*k_plus -1) = '\0';
			}
		for (i = 0 ; i < d->size_array_real ; i++)
			{
				do
					{
						pos = insert(d, d->strings_v + i*k_plus);
						if (pos == INVALID_POS)
							{
								d->size_dictionary_max = d->size_dictionary_max + REALLOCATE_DICTIONARY_SIZE;
								d->strings = (char *) realloc(d->strings, d->size_dictionary_max * sizeof(char) * k_plus);
								for (j = 0 ; j < d->size_dictionary_max ; j++)
									{
										*(d->strings + j*k_plus) = DEFAULT_DICTIONARY_CHARACTER;
										*(d->strings + (j+1)*k_plus -1) = '\0';
									}
								i = 0;
							}
					}
				while (pos == INVALID_POS);
			}

		return;
	}
void quick_sort (dictionary * d, int inf, int sup)
	{
		int
			pivot;

		if (inf < sup)
			{
				pivot = partition(d, inf, sup);
				quick_sort(d, inf, pivot);
				quick_sort(d, pivot+1, sup);
			}
	}
int partition (dictionary * d, int inf, int sup)
	{
		int
			i, j, k_plus, random;

		k_plus = d->k+1;
		
		i = inf-1;
		j = sup+1;

		random = (rand()%(sup-inf+1))+inf;
		
		char
			x[k_plus], tmp[k_plus];

		strncpy(x, (d->strings_v + (random)*k_plus), d->k);

		while (true)
			{
				do
					{
						j--;
					}
				while (strncmp((d->strings_v + j*k_plus), x, d->k) > 0);
				do
					{
						i++;
					}
				while (strncmp((d->strings_v + i*k_plus), x, d->k) < 0);
				if (i < j)
					{
						strncpy(tmp, (d->strings_v + i*k_plus), d->k);
						strncpy((d->strings_v + i*k_plus), (d->strings_v + j*k_plus), d->k);
						strncpy((d->strings_v + j*k_plus), tmp, d->k);
					}
				else
					{
						return (j);
					}
			}
	}
void print_filtered (FILE *pf, dictionary * d)
	{
		int
			i, k_plus;


		k_plus = d->k + 1;
		for (i = 0 ; i < d->cand_word ; i++)
			{
				fprintf(pf, "%s\n", (d->strings_v + i*(k_plus)));
			}

		return;
	}
void reset (dictionary * d, record * r)
	{
		int
			i, j;

		d->cand_word = d->size_array_real;

		for (i = 0 ; i < d->k ; i++)
			{
				*(r->right_in_place + i) = SPECIAL_SYMBOL;
			}
		for (i = 0 ; i < ALPHABET_LENGTH ; i++)
			{
				r ->right_not_in_place[i].oc = 0;
				r ->right_not_in_place[i].exact = false;
				for (j = 0 ; j < d->k ; j++)
					{
						*(r->not_in_place + i*d->k + j) = false;
					}
			}

		return;
	}
bool check_records (char string[], record * r, int k)
	{
		int
			i, occ[ALPHABET_LENGTH];
		bool
			compass;

		compass = true;

		for (i = 0 ; i < ALPHABET_LENGTH ; i++)
			{
				occ[i] = 0;
			}

		i = 0;
		while ((compass) and (i < k))
			{
				if ((r->right_in_place[i] != SPECIAL_SYMBOL) and (r->right_in_place[i] != string[i]))
					{
						compass = false;
					}
				if (compass)
					{
						if (*(r->not_in_place + k*map(string[i]) + i))
							{
								compass = false;
							}
						if (compass)
							{
								occ[map(string[i])]++;	
							}
					}
				i++;
			}

		i = 0;
		while ((compass) and (i < ALPHABET_LENGTH))
			{
				if (r->right_not_in_place[i].exact)
					{
						if (occ[i] != r->right_not_in_place[i].oc)
							{
								compass = false;
							}
					}
				else
					{
						if (occ[i] < r->right_not_in_place[i].oc)
							{
								compass = false;
							}
					}
				i++;
			}

		return (compass);
	}
void swap (dictionary * d, int i, int j)
	{
		char
			str_a[d->k +1];
		int
			k_plus;

		k_plus = d->k +1;
		
		str_a[d->k] = '\0';
		strncpy(str_a, d->strings_v + i*k_plus, d->k);
		strncpy(d->strings_v + i*k_plus, d->strings_v + j*k_plus, d->k);
		strncpy(d->strings_v + j*k_plus, str_a, d->k);

		return;
	}
int search (dictionary * d, char string[])
	{
		int
			pos, i, k_plus, y;
		bool
			found;
		int
			magic;

		k_plus = d->k+1;
		found = false;
		i = 0;
		magic = magic_calculator(string, d->k);
		y =  hash(magic, 0, d->size_dictionary_max);

		do
			{
				pos = hash(magic, i, d->size_dictionary_max);
				if (strncmp(d->strings + pos*k_plus, string, d->k) == 0)
					{
						found = true;
					}
				i++;
			}
		while ((*(d->strings + pos*k_plus) != DEFAULT_DICTIONARY_CHARACTER) and (not(found)) and (hash(magic, i, d->size_dictionary_max) !=y));

		if (not(found))
			pos = INVALID_POS;

		return (pos);
	}
void word_checker (char rif_word[], char word[], char output[], int k)
	{
		int
			i, j;
		int
			occur[ALPHABET_LENGTH];

		for (i = 0 ; i < ALPHABET_LENGTH ; i++)
			{
				occur[i] = 0;
			}
		
		for (i = 0 ; i<k ; i++)
			{
				if (rif_word[i] == word[i])
					{
						output[i] = MARKER_RIGHT_IN_PLACE;
					}
				else
					{
						occur[map(rif_word[i])]++;
						output[i] = MARKER_SPECIAL;
					}
			}
		
		output[i] = '\0';
		for (j = 0; j<k; j++)
			{
				if (output[j] != MARKER_RIGHT_IN_PLACE) 
					{
						if (occur[map(word[j])] > 0)
							{
								occur[map(word[j])]--;
								output[j] = MARKER_RIGHT_NOT_IN_PLACE;
							}
						else
							{
								output[j] = MARKER_NOT_RIGHT;
							}
					}		
			}

		return;
	}
void expand_records (record * r, char word[], char output[], int k)
	{
		int
			i;
		int
			occorren[ALPHABET_LENGTH], max[ALPHABET_LENGTH];

		for (i = 0 ; i < ALPHABET_LENGTH ; i++)
			{
				occorren[i] = 0;
				max[i] = 0;
			}

		for (i = 0 ; i < k ; i++)
			{
				if (output[i] == MARKER_RIGHT_IN_PLACE)
					{
						r -> right_in_place[i] = word[i];
						occorren[map(word[i])]++;
					}
			}
		for (i = 0 ; i < k ; i++)
			{
				switch (output[i])
					{
						case MARKER_NOT_RIGHT:
							*(r->not_in_place + map(word[i])*k + i) = true; 						
							if (not(r->right_not_in_place[map(word[i])].exact))
								{
									r -> right_not_in_place[map(word[i])].exact = true;
									r-> right_not_in_place[map(word[i])].oc = occorren[map(word[i])] + max[map(word[i])];
								}
							break;
						case MARKER_RIGHT_NOT_IN_PLACE:
							*(r->not_in_place + map(word[i])*k + i) = true; 					
							if (not(r ->right_not_in_place[map(word[i])].exact))
								{
									max[map(word[i])]++;
								}
							break;
						default:
							break;
					}		
			}

		for (i = 0 ; i < ALPHABET_LENGTH ; i++)
			{
				if (not(r->right_not_in_place[i].exact))
					{
						max[i] = max[i] + occorren[i];
						if (max[i] > r->right_not_in_place[i].oc)
							{
								r->right_not_in_place[i].oc = max[i];
							}
					}	
			}

		return;
	}
void rescan_records (dictionary * d, record * r)
	{
		int
			i, m, g, k_plus;

		m = d->cand_word;
		k_plus = d->k +1;

		bool
			cands[m];

		for (i = 0 ; i < m ; i++)
			{
				cands[i] = check_records(d->strings_v + i*k_plus, r, d->k);
				if (not(cands[i]))
					{
						d->cand_word--;
					}
			}
		
		g = m;
		for (i = 0 ; i < g ; i++)
			{
				if (not(cands[i]))
					{
						if (cands[g-1])
							{
								swap(d, g-1, i);
							}
						else
							{
								i--;
							}
						g--;
					}				
			}
		
		return;
	}
