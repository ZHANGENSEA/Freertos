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
Ensuite, dans votre fonction d'initialisation (par exemple, avant le démarrage de l'ordonnanceur), créez le mutex :
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL)
    {
        // Gestion de l'erreur si le mutex n'a pas pu être créé
    }
    /* USER CODE END Init */
}
Étape 3: Utiliser le Mutex autour de printf
Modifiez vos tâches pour qu'elles acquièrent le mutex avant d'appeler printf et le relâchent après :
void task1(void * pvParameters)
{
    int delay = (int) pvParameters;

    for(;;)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdPASS)
        {
            printf("Je suis la tache 1 et je m'endors pour %d ticks\r\n", delay);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(delay);
    }
}

void task2(void * pvParameters)
{
    int delay = (int) pvParameters;

    for(;;)
    {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdPASS)
        {
            printf("Je suis la tache 2 et je m'endors pour %d ticks\r\n", delay);
            xSemaphoreGive(xMutex);
        }
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

2.1.3 Expliquer les mécanismes qui mènent à l’exécution de la fonction.
Réponse : D'abord, on a l'initialisation du Shell 'shell_init();' pour laisser le shell préparer à recevoir et à traiter les commandes. Ensuite, on utilise 'shell_add' pour difinir les identifiants comme des caractères 'f' et 'a', le nom de fonction comme 'fonction' et 'addition', et la description du fonction comme 'fonction inutile'. Et puis, on a l'exécution du Shell, shell_run() démarre la boucle principale du shell, qui écoute les entrées de l'utilisateur via l'UART. Pendant l'exécution du Shell, on a lu les arguments extraits de la ligne de commande par la function uart_read, et il y a certains cas, par exemple '\r'(entrée) pour finir l'entre des arguments, '\b'(espace) pour deleter les arguments, et il a aussi défini le mot clé pour stoquer les caractères comme par une espace. 
En general, les arguments extraits de la ligne de commande sont passés à la fonction cible sous forme d'un tableau de chaînes (char** argv) et leur nombre (int argc) pour les traiter. Après l'exécution de la commande, le shell revient en attente pour la prochaine entrée utilisateur, continuant ainsi la boucle.

2.1.4 Quel est le problème ?
Réponse : Il y a un problème de synchronisation avec l'UART, c'est-à-dire, si plusieurs tâches tentent d'utiliser printf ou HAL_UART_Transmit simultanément sans synchronisation appropriée, cela peut conduire à des sorties mélangées ou corrompues.

2.1.5 Proposer une solution
Réponse : On peut utiliser sémaphore pour bloquer une tâche et l'éveil une autre tâche.

2.2 Que se passe-t-il si l’on ne respecte pas les priorités décrites précédemment ?
Réponse : Si l'on ne respecte pas les priorités décrites précédemment, FreeRTOS peut échouer avec une assertion pour indiquer que la priorité de l'interruption n'est pas configurée correctement pendant les vérifications, et il y a préemption par d'autres interruptions qui feraient appel aux primitives de FreeRTOS. A cause d'interruptions de priorité plus élevée occupant le CPU sans permettre aux opérations de FreeRTOS nécessaires de s'exécuter, des situations où des tâches ou des gestionnaires d'interruption pourraient être bloqués indéfiniment. 
