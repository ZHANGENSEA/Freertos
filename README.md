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
Réponse :  

0.1.5. Écrivez un programme simple permettant de faire clignoter la LED.  
Réponse :  

0.1.6. Modifiez le programme pour que la LED s’allume lorsque le bouton USER est appuyé. Le bouton est sur PI11. 
Réponse :  

1.1.1 En quoi le paramètre TOTAL_HEAP_SIZE a-t-il de l’importance ?
Réponse : Il est 15360, c'est la taille de mémoire. 

1.1.2  Quel est le rôle de la macro portTICK_PERIOD_MS ?
Réponse : 
