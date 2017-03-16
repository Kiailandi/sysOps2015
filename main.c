#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

//Timestamp composto da data e ora
char* timestamp(){
    //Buffer usato per contenere valore di ritorno, grandezza "sovrastimata" per sicurezza
    char s[800]; 
    time_t t = time(NULL); 
    struct tm * p = localtime(&t); 
    strftime(s, 1000, "[%d/%m/%y %H:%M:%S]", p); 
    return s; 
}

//Funzione ausiliaria per formare la stringa unendo le componenti in base al livello di logging
char* logformat(char* command, int retvalue, char* l){
    //Buffer usato per contenere valore di ritorno, grandezza "sovrastimata" per sicurezza
    char result [1024];
    if(strcmp(l, "low") == 0){
        char str [80];
        strcpy(str, command);
        char* token;
        //Split per ottenere solo il "comando puro" -> senza parametri
        token = strtok(str, " ");
        char * ts = timestamp();
        //Stringa composta da timestamp + comando + valore di ritorno del comando
        sprintf(result, "%s %s %d", ts, token, retvalue);
    }
    else if(strcmp(l, "medium") == 0){
            //Stringa composta da timestamp + comando + parametri + valore di ritorno del comando
            sprintf(result, "%s %s %d", timestamp(), command, retvalue);
        }
        else{
            char* flag;
            //Contollo il primo carattere per vedere se il comando è interno [!] o no
            if(strcmp(&command[0], "!"))
                flag = "i";
            else
                flag = "e";
            //Stringa composta da timestamp + comando + parametri + tipo di comando + valore di ritorno del comando
            sprintf(result, "%s %s [%s] %d", timestamp(), command, flag, retvalue);
        }
    return result;
}

int logger(char* f, char* command, int retvalue, char* l){
    //Modalità di apertura a+, quindi append in fondo al file se esiste altrimenti lo crea
    FILE* output = fopen(f, "a+");
    //Controllo che l'utente abbia i permessi di accesso in scrittura [eseguito dopo la fopen per evitare errore in caso di file non presente]
    if(access(f, W_OK ) == -1){
        fclose(output);
        return 1;
    }
    char* result = logformat(command, retvalue, l);
    //Stampo sul file la stringa ottenuta dal metodo ausiliario
    fprintf(output, "%s\n", result);
    fclose(output);
    return 0;
}

//Stampo l'attuale livello di logging
int showlevel(char* l){
    printf("%s\n", l);
    return 0;
}

//Attivo il logging
int logon(int *logging){
  *logging = 1;
  return 0;
}

//Disattivo il logging
int logoff(int *logging){
  *logging = 0;
  return 0;
}

//Mostra il contenuto del file di logging
int logshow(char* f){
    //Controllo i permessi in lettura e l'esistenza del file
    if(access(f, R_OK ) == -1)
        return 1;
    else{
        //Apro il file in lettura
        FILE* input = fopen(f, "r");
        char line[180];
        //Stampo il nome del file
        printf("%s\n", f);
        //Leggo riga per riga e stampo a video
        while(fgets(line, 180, input) != NULL)
        {
            printf("%s", line);   
        }
        fclose(input);
    }
    return 0;
}

//Funziona ausiliaria per contollare se il nuovo livello è accettabile
int checklevel(char* l){
    return (!(strcmp(l, "low") & strcmp(l, "medium") & strcmp(l, "high")));
}

int setlevel(char** l, char* newlevel){
    //Se il nuovo livello è accettabile modifico il valore di l 
    if(checklevel(newlevel)){
        *l = newlevel;
        return 0;
    }
    else
        return 1;
}

//Cambia il messaggio mostrato all'utente prima di ogni comando
int setprompt(char** p, char* newprompt){
    *p = newprompt;
    return 0;
}

//Fork preventiva per il comando successivo
int run(){
    int pid = fork();
    //Ritorno il pid per distinguere errori/padre/figlio
    return pid;
}

//Funzione per ottenre il comando di uscita con il valore di ritorno da, eventualmente, registrare sul file di log
int quit(){
   return 0; 
}

//Gestione degli argomenti della console + menù + gestione input utente
int main(int argc, char* argv[])
{
  //Setto dei valori di default per gli argomenti opzionali della console
  char* p = ">";
  //Valori possibili: low, mid, high
  char* l = "low";
  char* f = "log.txt";
  //Variabili utilizzate per la optget
  int index;
  int c;
  int logging = 1;
  int opterr = 0;

  //I parametri possibili sono -p <prompt> | -l <loglevel> | -f <logfile>
  while ((c = getopt (argc, argv, "p:l:f:")) != -1)
    //Switch per controllare gli argomenti passati e assegnarli alle rispettive variabili
    switch (c)
      {
      case 'p':
        p = optarg;
        break;
      case 'l':
        //Controllo che il valore sia accettabile
        if(checklevel(optarg))
            l = optarg;
        else{
            printf("%s\n", "Accepted value are low | medium | high");
            return 1;
        }
        break;
      case 'f':
        //Controllo che il file/percorso passato sia un file di testo
        if(strcmp(&optarg[strlen(optarg)-4], ".txt") == 0 | strcmp(&optarg[strlen(optarg)-4], ".log") == 0)
          f = optarg;
        else{
          printf("%s\n", "Need path to a .txt or .log file");
          return 1;
        }
        break;
      case '?':
        //Errore nel caso in cui i parametri non abbiano i rispettivi argomenti
        if (optopt == 'p' | optopt == 'l' | optopt == 'f')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        //Errore nel caso di parametri sconosciuti
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n Correct usage: -p <prompt> | -l <loglevel> | -f <logfile>\n", optopt);
        else
          //Errore nel caso di caratteri non stampabili
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  //Stampo i valori dei 3 parametri che verranno utilizzati   
  printf ("prompt = %s, loglevel = %s, logfile = %s\n",
          p, l, f);

  //Variabili per la lettura dell'input
  char* line = NULL;
  int len;
  //Variabile per i valori di ritorno dei comandi
  int retvalue = 1;
  //Variabili per dividere i comandi [interni] con parametro dal rispettivo parametro
  char str[80];
  char* token = NULL;
  char* param = NULL;
  
  //Variabile utilizzata per riconoscere il figlio dal padre dopo la fork
  int child = 0;
  
  do{
    //Controllo per far eseguire solo un comando nel processo figlio dopo la fork
    if(child == 1)
        child = -1;
     
    //Stampa del messaggio/prompt   
    printf("%s:", p);
    
    //Prendo l'input dall'utente
    getline(&line, (size_t*)&len, stdin);
    
    //Sostituisco l'invio di fine riga con il terminatore di stringhe
    if (line[strlen(line) - 1] == '\n')
      line[strlen(line) - 1] = '\0';
     
    //Controllo se il comando è interno o esterno.
    if(line[0] == '!'){
    //Per tutti i comandi controllo se il logging è attivato e in caso richiamo la funzione per la scrittura su file
    //La struttra a if else a cascata è per simulare uno switch con stringhe invece che singoli char per la scelta
        if(strcmp(line, "!showlevel") == 0){
            retvalue = showlevel(l);
            if(logging){
                logger(f, line, retvalue, l);
            }   
        }else if(strcmp(line, "!logon") == 0){
            //Passaggio parametro per riferimento
            retvalue = logon(&logging);
            if(logging){
                logger(f, line, retvalue, l);
            }
            printf("%s\n", "logging is on");
        }else if(strcmp(line, "!logoff") == 0){            
            //Passaggio parametro per riferimento
            retvalue = logoff(&logging);
            if(logging){
                logger(f, line, retvalue, l);
            }
            printf("%s\n", "logging is off");
        }else if(strcmp(line, "!logshow") == 0){
            retvalue = logshow(f);
            if(logging){
                logger(f, line, retvalue, l);
            }
            //Se il file non è accessibile stampo un messaggio di errore
            if(retvalue == 1){
                printf("%s\n", "Error, file not accessible");
            }
        }else if(strcmp(line, "!run") == 0){
            retvalue = run(l);
            //Controllo che non sia il processo figlio
            if(retvalue != 0){
                if(retvalue < 0){
                    //Se il pid è negativo stampo un messaggio di errore
                    printf("%s\n", "Error, can't fork");
                }else if(printf > 0){
                    if(logging){
                        logger(f, line, retvalue, l);
                    }
                    //Se il pid è postivo sono nel padre e attendo che il figlio finisca di eseguire il comando
                    char* status;
                    waitpid(retvalue, &status, 0);
                    printf("%s\n", "Back to orginal process");
                }
            }
            else{
                //Se il pid è 0 sono nel figlio
                printf("%s\n", "Next command will be executed in a new process");
                //Setto la variabile ausiliaria per limitare l'esecuzione del processo al solo prossimo comando
                child = 1;
            }
        }else if(strcmp(line, "!quit") == 0){
            //Chiudo la console corrente
            retvalue = quit();
            if(logging){
                logger(f, line, retvalue, l);
            }
            break;
        }else{
            //Divido i comandi con parametro in comando + parametro
            strcpy(str, line);
            token = strtok(str, " ");
            if(token != NULL)
                param = strtok(NULL, " ");
            if(strcmp(token, "!setlevel") == 0){
                //Passaggio per riferimento
                retvalue = setlevel(&l, param);
                if(logging){
                    logger(f, line, retvalue, l);
                }
                if(retvalue == 1){
                    //Se il valore non è accettabile restituisco un messaggio di errore
                    printf("%s\n", "Error. Accepted values are low | medium | high");
                }
            }else if(strcmp(token, "!setprompt") == 0){
                //Passaggio per riferimento
                retvalue = setprompt(&p, param);
                if(logging){
                    logger(f, line, retvalue, l);
                }
            }else
                //Se il comando non è tra quelli riconosciuti stampo un messaggio di errore
                printf("%s\n", "Unknown command");
        }
    }
    else{
        //Se il comando è esterno lo faccio eseguire dal sistema
        retvalue = system(line);
        if(logging){
            logger(f, line, retvalue, l);
        }
    }
  //Ritorno al padre se il figlio ha già eseguito un comando
  }while(child > -1); 

  //"Forzo" la stampa da buffer a video
  fflush(stdout);
  free(line);

  return 0;
}