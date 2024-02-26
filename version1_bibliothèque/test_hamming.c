#include <stdio.h>
#include "hamming.h"

int main() {
	    // Exemple d'utilisation des fonctions d'encodage et de décodage
	    
	         // Données originales
	             unsigned char originalData = 0x02;  // Vous pouvez changer cette valeur
	    
	                 // Encodage
	                     unsigned char encodedData = HammingMatrixEncode(originalData);
	    
	                         // Affichage des résultats
	                             printf("Original Data: 0x%X\n", originalData);
	                                 printf("Encoded Data : 0x%X\n", encodedData);
	    
	                                    // Simuler une erreur dans le code encodé (par exemple, inversion d'un bit)
	                                         unsigned char corruptedData = encodedData ^ 0x04;  // Inversion du 3e bit
	    
	                                             // Décodage
	                                                 unsigned char decodedData = HammingMatrixDecode(corruptedData);
	    
	                                                     // Affichage des résultats après décodage
	                                                         printf("\nCorrupted Data: 0x%X\n", corruptedData);
	                                                             printf("Decoded Data  : 0x%X\n", decodedData);
	    
	                                                                return 0;
	                                                               }
