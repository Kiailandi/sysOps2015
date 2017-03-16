# sysOps2015

----------------------------

## Versione Italiana

[ Progetto Sistemi Operativi 2015 @ UniTN ]

### TODO:

- [ ] includere makefile
- [ ] traduzione in inglese commenti (?)
- [ ] traduzione in inglese readme (?)

### Descrizione Progetto

Implementare una shell interattiva (che accetti almeno comandi basilari) con le seguenti caratteristiche:
- deve accettare tre argomenti opzionali: ```-p <prompt> -l <loglevel> -f <logfile>``` per settare un prompt personalizzato, un livello di “logging” (un valore tra *“low”*, *“middle”* e *“high”*) e il nome del file di log. Argomenti errati devono mostrare un opportuno avviso.


- deve mostrare un prompt personalizzato (di default o settato con l'argomento) e accettare comandi che sono passati alla shell standard, a meno che il primo carattere non vuoto sia un punto esclamativo, in questo caso ciò che segue è interpretato come un comando *“interno”* speciale:
  - ```!showlevel```: mostra il livello impostato (default o settato con l'argomento)
  - ```!logon```: attiva il logging
  - ```!logoff```: disattiva il logging
  - ```!logshow```: mostra il log attuale (nome del file + contenuto)
  - ```!setlevel <loglevel>```: cambia il livello di log
  - ```!setpromp <prompt>```: cambia il prompt
  - ```!run```: esegue l'azione successiva in un processo separato
  - ```!quit```: esce dalla shell


- la shell scrive in un file di log (di default o scelto dall'utente) le azioni svolte, a seconda del livello impostato:
  - ogni riga inizia con un riferimento al timestamp seguito dal comando eseguito (senza parametri)
  - se il livello di log è *“low”* non ci sono altre informazioni intermedie
  - se il livello di log è *“middle”* deve essere riportata l'intera stringa (con tutti i parametri)
  - se il livello di log è *“high”* deve essere anche riportata l'indicazione se si tratta di un comando interno o esterno
  - ogni riga termina con lo status (codice d'errore) dell'esecuzione del comando lanciato


### Descrizione Makefile

- La compilazione/esecuzione dell’elaborato deve poter avvenire accedendo alla cartella di progetto e tramite l’esecuzione di un “make” con le seguenti modalità:
  - ```make```: deve generare in output una breve descrizione del progetto e la lista delle opzioni di make (altri target) disponibili
  - ```make bin```: deve generare i binari compilati eseguibili dentro una cartella da crearsi automaticamente denominata “bin” che conterrà i soli file oggetto eseguibili
  - ```make assets``` deve generare dei possibili file di input (di prova) dentro una cartella da crearsi automaticamente denominata “assets”
  - ```make test``` deve generare i binari e gli assets e lancial’eseguibile utile a provare i file di input generati
  - ```make clean``` elimina eventuali file temporanei, binari e assets generati (questa regola deve essere sempre richiamata automaticamente dalle altre)
