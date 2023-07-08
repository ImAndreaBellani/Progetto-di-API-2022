# Progetto-di-API-2022
Progetto di Algoritmi e Strutture dati realizzato per il corso "PROVA FINALE (PROGETTO DI ALGORITMI E STRUTTURE DATI)" del Politecnico di Milano nell'anno accademico 2021/2022. Voto: 30 e lode.

### Prestazioni

<table>
  <tr>
    <th></th>
    <th>Tempo impiegato [s]</th>
    <th>Tempo massimo per passare il task [s]</th>
    <th>Memoria occupata [MB]</th>
    <th>Limite memoria per passare il task [MB]</th>
  </tr>
  <tr>
    <th>UPTO18</th>
    <th>0,669</th>
    <th>0,8</th>
    <th>5,72</th>
    <th>18</th>
  </tr>
  <tr>
    <th>UPTO21</th>
    <th>10,006</th>
    <th>25</th>
    <th>77,2</th>
    <th>130</th>
  </tr>
  <tr>
    <th>UPTO24</th>
    <th>10,383</th>
    <th>25</th>
    <th>77,2</th>
    <th>130</th>
  </tr>
  <tr>
    <th>UPTO27</th>
    <th>10,431</th>
    <th>25</th>
    <th>45,5</th>
    <th>130</th>
  </tr>
  <tr>
    <th>UPTO30</th>
    <th>9,136</th>
    <th>25</th>
    <th>37</th>
    <th>130</th>
  </tr>
  <tr>
    <th>CUMLAUDE</th>
    <th>30,059</th>
    <th>50</th>
    <th>93,7</th>
    <th>170</th>
  </tr>
</table>
<br>

### Strutture dati utilizzate

<br>

```c
typedef struct _dict
	{
		int k;
		
		int size_array_max;
		int size_array_real;
		int size_dictionary_max;

		int cand_word;

		char * strings; //dizionario
		char * strings_v; //coda
	} dictionary;
```
<p>
  L'applicativo necessitava di una struttura dati efficiente nell'inserimento, nella ricerca e nell'ordinamento. Ho deciso allora di memorizzare le stringhe
  che compongono il "dizionario" in due strutture dati, una tabella di hash "char * strings" gestita in closed hashing e un semplice arraylist "char * strings_v".
  Se viene richiesto un ordinamento, esso viene effettuato solo sugli elementi utili di "strings_v", mentre le operazioni di ricerca sono sempre eseguite su "strings".
</p>

#### **Per ulteriori informazioni si consulti la documentazione all'interno del file sorgente "Bellani - Progetto di Algoritmi e Strutture dati 2021-2022 [07-09-2022].c"**
