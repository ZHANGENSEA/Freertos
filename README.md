# Freertos
0.1.1. Où se situe le fichier main.c ?  
Réponse :  Core – Src 

0.1.2. À quoi servent les commentaires indiquant BEGIN et END (appelons ça des balises) ? 
Réponse :   STM32CubeIDE propose un outil de génération de code pour faciliter la configuration des microcontrôleurs STM32. Les balises BEGIN et END peuvent être utilisées par cet outil pour identifier les sections du code qui ne doivent pas être modifiées lors de la génération de code automatique. Cela permet aux développeurs d'insérer leur propre code personnalisé entre ces balises sans risquer qu'il soit écrasé lors de la mise à jour de la configuration à travers l'outil.  

0.1.3. Quels sont les paramètres à passer à HAL_Delay et HAL_GPIO_WritePin ? Maintenez Ctrl et cliquez sur le nom d’une fonction pour voir son code. 
Réponse :  
HAL_Delay(uint32_t Delay) ，  Delay-spécifie la durée du délai, en millisecondes.
HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)， GPIOx-où x peut être (A..K) pour sélectionner le périphérique GPIO, GPIO_Pin-spécifie le bit de port à écrire, PinState-spécifie la valeur à écrire sur le bit sélectionné.

0.1.4. Dans quel fichier les ports d’entrée/sorties sont-ils définis ?  
Réponse :   gpio.h

1.1.1 En quoi le paramètre TOTAL_HEAP_SIZE a-t-il de l’importance ?
Réponse : Il est 15360 bytes, c'est la quantité de mémoire dynamique. Ce paramètre définit la quantité totale de mémoire que FreeRTOS peut utiliser pour l'allocation dynamique. Cela inclut la création de tâches, de queues, de semaphores, etc. Une taille de tas insuffisante peut entraîner un échec de création des objets FreeRTOS, tandis qu'une taille trop grande peut gaspiller la mémoire précieuse du microcontrôleur.

1.1.2 Quel est le rôle de la macro portTICK_PERIOD_MS ?
Réponse : portTICK_PERIOD_MS est une macro utilisée dans FreeRTOS pour convertir une période de temps spécifiée en millisecondes au nombre de ticks du système. Cela permet d'abstraire la durée réelle entre les ticks du système, qui peut varier en fonction de la configuration de FreeRTOS et de la fréquence du processeur. L'utilisation de cette macro garantit que le délai spécifié dans vTaskDelay() est indépendant de la configuration du système d'exploitation en temps réel et du matériel

1.2.6 Changez les priorités. Expliquez les changements dans l’affichage.
Réponse : taskTake est plus fréquemment exécutée car de priorité plus élevée, elle préempte donc taskGive dès qu'elle est prête à s'exécuter. Cela signifie que juste après que taskGive donne le sémaphore, taskTake prendra le contrôle presque immédiatement pour tenter de prendre le sémaphore.
Les messages taskTake seront plus fréquents, et les messages taskGive seront affichés avec du retard

1.5.11 Observez attentivement la sortie dans la console. Expliquez d’où vient le problème.
Réponse : L'utilisation concurrente de printf (ou toute autre fonction effectuant des I/O) par plusieurs tâches sans synchronisation appropriée peut conduire à des corruptions de données. Cela se produit parce que l'accès à l'UART (ou à tout autre ressource partagée) n'est pas protégé.

1.5.12 Proposez une solution en utilisant un sémaphore Mutex.
Réponse : 
Étape 1: Déclarer le Mutex
Tout d'abord, déclarez un mutex global : 
SemaphoreHandle_t xMutex;
Étape 2: Créer le Mutex
Ensuite, dans le main(), créez le mutex :
xMutex = xSemaphoreCreateMutex();
Étape 3: Utiliser le Mutex autour de printf
Modifiez vos tâches pour qu'elles acquièrent le mutex avant d'appeler printf et le relâchent après :
void task1(void * pvParameters)
{
	int delay = (int) pvParameters;

	for(;;)
	{
		xSemaphoreTake(xMutex, portMAX_DELAY);
		printf("Je suis la tache 1 et je m'endors pour %d ticks\r\n", delay);
		xSemaphoreGive(xMutex);
		vTaskDelay(delay);

	}
}

void task2(void * pvParameters)
{
	int delay = (int) pvParameters;

	for(;;)
	{
		xSemaphoreTake(xMutex, portMAX_DELAY);
		printf("Je suis la tache 2 et je m'endors pour %d ticks\r\n", delay);
		xSemaphoreGive(xMutex);
		vTaskDelay(delay);
	}
}
En conclusion, cette solution s'assure que lorsque la tâche 1 utilise printf, la tâche 2 doit attendre que la tâche 1 ait terminé avant de pouvoir utiliser printf, et vice-versa. Cela empêche l'interférence entre les appels à printf et garantit que les messages de sortie ne seront pas mélangés ou corrompus.

2.1.2 Modifier la fonction pour faire apparaître la liste des arguments.
Réponse : 
int fonction(int argc, char ** argv)
{
	printf("argc = %d\r\n", argc);
	for (int itr = 0; itr < argc; itr++)
	{
		printf("argv[%d] = %s\r\n", itr, argv[itr]);
	}
	return 0;
}

2.1.2 Modifier la fonction pour faire apparaître la liste des arguments.
Réponse : 
int fonction(int argc, char ** argv)
{
	printf("argc = %d\r\n", argc);
	for (int itr = 0; itr < argc; itr++)
	{
		printf("argv[%d] = %s\r\n", itr, argv[itr]);
	}
	return 0;
}

2.1.3 Expliquer les mécanismes qui mènent à l’exécution de la fonction.
Réponse : D'abord, on a l'initialisation du Shell 'shell_init();' pour laisser le shell préparer à recevoir et à traiter les commandes. Ensuite, on utilise 'shell_add' pour difinir les identifiants comme des caractères 'f' et 'a', le nom de fonction comme 'fonction' et 'addition', et la description du fonction comme 'fonction inutile'. Et puis, on a l'exécution du Shell, shell_run() démarre la boucle principale du shell, qui écoute les entrées de l'utilisateur via l'UART. Pendant l'exécution du Shell, on a lu les arguments extraits de la ligne de commande par la function uart_read, et il y a certains cas, par exemple '\r'(entrée) pour finir l'entre des arguments, '\b'(espace) pour deleter les arguments, et il a aussi défini le mot clé pour stoquer les caractères comme par une espace. 
En general, les arguments extraits de la ligne de commande sont passés à la fonction cible sous forme d'un tableau de chaînes (char** argv) et leur nombre (int argc) pour les traiter. Après l'exécution de la commande, le shell revient en attente pour la prochaine entrée utilisateur, continuant ainsi la boucle.

2.1.4 Quel est le problème ?
Réponse : Il y a un problème de synchronisation avec l'UART, c'est-à-dire, si plusieurs tâches tentent d'utiliser printf ou HAL_UART_Transmit simultanément sans synchronisation appropriée, cela peut conduire à des sorties mélangées ou corrompues.

2.1.5 Proposer une solution
Réponse : On peut utiliser sémaphore pour bloquer une tâche et l'éveil une autre tâche.

2.2 Que se passe-t-il si l’on ne respecte pas les priorités décrites précédemment ?
Réponse : Si l'on ne respecte pas les priorités décrites précédemment, FreeRTOS peut échouer avec une assertion pour indiquer que la priorité de l'interruption n'est pas configurée correctement pendant les vérifications, et il y a préemption par d'autres interruptions qui feraient appel aux primitives de FreeRTOS. A cause d'interruptions de priorité plus élevée occupant le CPU sans permettre aux opérations de FreeRTOS nécessaires de s'exécuter, des situations où des tâches ou des gestionnaires d'interruption pourraient être bloqués indéfiniment. 

2.3
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/698e773a-f284-4f4f-a824-127ef49843c0)


3.1.1 Quel est le nom de la zone réservée à l’allocation dynamique ?
Réponse : Tas (Heap). C'est la zone dans laquelle sont écrites les zones mémoires allouées dynamiquement(malloc() ou calloc()).

3.1.2 Est-ce géré par FreeRTOS ou la HAL ?
Réponse : Il est défini dans FreeRTOSConfig.h, donc il est géré par FreeRTOS.

3.1.4 Notez la mémoire RAM et Flash utilisée
Réponse : RAM - 301.06KB(Free) - 19.94 KB(Used) - 5.92% (Usage) , FLASH - 996.22KB (Free) - 27.78KB(Used) - 2.71%(Usage). 

3.1.5 et 6 Créez des tâches bidons jusqu’à avoir une erreur et notez la nouvelle utilisation mémoire.

Réponse : Lors de la création de tâche, par exemple 200, on constate qu'avec l'allocation mémoire que l'on a dans la question 3.1.4, que le nombre de tâches qui se créent ne va pas jusqu'à 200 (car on fixe le nombre de tâches à créer à 200) mais s'arrêtent à 12. L'utilisation mémoire reste la même ici.

3.1.7 et 8 Dans CubeMX, augmentez la taille du tas (TOTAL_HEAP_SIZE). Générez le code, compilez et testez et notez la nouvelle utilisation mémoire. Expliquez les trois relevés.

Réponse : Si on augmente la taille du tas, on voit que le pourcentage de l'usage du RAM augmente aussi. Par exemple, TOTAL_HEAP_SIZE : 15360 -> 153600, l'usage de RAM : 5.92% -> 47.84%.
Concernant le nombre de tâches, toujours fixées à 200, on parvient à obtenir la création de 135 tâches dorénavant. Cela est dû à la disponibilité plus importante d'espace mémoire. Pour les explications des deux autres relevés, cf les questions 5 et 6.

3.2.2 Dans CubeMX, configurez CHECK_FOR_STACK_OVERFLOW
Réponse : ioc - FREERTOS- ConfigParametres - Check_for_stack_overflow - Option 2

3.3.4 Lancez le programme puis mettez-le en pause pour voir les statistiques.
Réponse : Dans ce projet, on peut voir les statistiques des tâches : 
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/6867f110-c6f6-437f-945f-fb27b61f21fd)

3.3.5 Cherchez dans CubeMX comment faire pour afficher l’utilisation de la pile. En mode debug, cliquez sur Toggle Stack Checking (dans l’onglet FreeRTOSTask List en haut à droite).
Réponse : Une fois dans le mode débogage, cherchez la vue "FreeRTOS Task List", et puis cliquez sur Toggle Stack Checking,
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/5036e635-0de7-464f-8411-d20964369e02)

3.3.7 Affichez les sémaphores et les queues.
Réponse : ![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/f5992e24-6a9c-4330-8c4b-423226bc4c34)

3.4 écrire une fonction appelable depuis le shell pour afficher les statistiques dans le termainal.
Réponse : ![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/514d0055-9e53-4c06-9731-953772eaf81f)

4.1 Interfacer l'ADXL345
![74291a6f269ae7ce4bf9c57341dd365](https://github.com/ZHANGENSEA/Freertos/assets/149954066/8e00e410-7264-4c90-b440-f34cf036fb59)

4.2 Premiers tests
4.2.2 Dans la fonction shell, écrivez un code permettant de récupérer la valeur du DEVID, et vérifiez si elle est correcte.
Réponse : Selon le datasheet de l'ADXL345, on trouve que le registre 0x00 est "Read only" et la valeur est fixée à 0xE5, donc le résultat est correcte.
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/70a14d57-65df-4ceb-9611-9e07222867fe)

4.2.3 Quelles sont les valeurs à mettre dans les registres INT_ENABLE et POWER_CTL pour démarrer la mesure et délencher une interruption à chaque mesure ?
Réponse : Pour démarrer la mesure et délencher une interruption à chaque mesure,  la valeur doit mettre D7( DATA_READY) à 1 et les autres à 0 dans les registres INT_ENABLE ; D3(Measure) à 1 et les autres à 0 dans le registre POWER_CTL.

4.2.4 À la suite du code précédent, dans la fonction shell, écrivez un code permettant de lire 4 valeurs consécutives. Utilisez du polling pour attendre que la
broche INT1 passe à High.
Réponse : On a lu x,y,z à quatre fois par une boucle : 
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/f62b8919-54c1-4e4f-8fcd-9acc2069aa26)
Le résultat : 
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/59a387d6-dfc5-4110-99e0-a633e95d4b54)

4.2.5 Faites la moyenne de ces quatre valeurs, mettez les accélérations en forme,et affichez-les à travers l’UART.
Réponse : On a créé le SUM dans la boucle pour mettre chaque valeur dans la tableau SUM, et fait la division hors boucle afin de les afficher.
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/9236b861-c662-4f3a-8eac-aca180d4270b)
Le Résultat : 
![image](https://github.com/ZHANGENSEA/Freertos/assets/149954066/7738fcb9-7dfe-4f5b-b5af-33e5dccf8cfc)










