# Practice Mode
Questo programma nasce dall'esigenza di non disturbare i propri colleghi suonatori durante le prove nel momento in cui si decide di **studiare una canzone** in sala prove.
Con **Practice Mode** è possibile, tramite il click di un bottone, silenziare nel proprio monitor tutti gli altri componenti della band a parte se stessi.

## Interfaccia
L'interfaccia è strutturata in modo molto semplice, con *KEY_LEFT* e *KEY_RIGHT* si naviga tra le opzioni.
Per ogni sezione selezionata vengono forniti i comandi possibili.
Questo è il menu di configurazione del **mixer**:

<img width="648" height="404" alt="interfaccia" src="https://github.com/user-attachments/assets/c4e9099c-bbd9-468a-a6da-459fd772ff18" />

Questo è il menu di configurazione dei **bus** (identico a quello dei canali):

<img width="641" height="398" alt="interfaccia_bus" src="https://github.com/user-attachments/assets/21cc6528-c7f0-4a84-aeea-253d10a071b2" />

Questo è il menu per far partire la **Practice Mode**:

<img width="644" height="398" alt="interfaccia_timer" src="https://github.com/user-attachments/assets/d1c8adbf-f9e5-41f5-b4b8-4be5669dc67f" />

---
## Requisiti
- PC
- cmake
- mixer x-air xr18
- libreria C: *oscpkt*
- libreria C: *ncurses*

---
## Inizializzazione
Controllare nelle impostazioni del mixer qual è il suo **indirizzo IP** (possibilmente impostarlo statico per non dover rimodificare le impostazioni del programma) e la sua **porta**.
È molto consigliato creare un file di configurazione come da esempio seguente:

```
10    # numero dello snapshot che si sta usando correntemente
# strumento -> bus
# non possono esserci canali di input duplicati
strumento1 01
strumento2 02
strumento3 03
strumento4 01

# strumwento -> input channel
# non possono esserci canali di input duplicati 
strumento1 09
strumento2 12
strumento3 03
strumento4 01
```
