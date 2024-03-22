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

1.1.6 Changez les priorités. Expliquez les changements dans l’affichage.
Réponse : taskTake est plus fréquemment exécutée car de priorité plus élevée, elle préempte donc taskGive dès qu'elle est prête à s'exécuter. Cela signifie que juste après que taskGive donne le sémaphore, taskTake prendra le contrôle presque immédiatement pour tenter de prendre le sémaphore.
Les messages taskTake seront plus fréquents, et les messages taskGive seront affichés avec du retard
