#include <AcceleroMMA7361.h>

AcceleroMMA7361 accelero; // Création du composant
int x; // Création des variables pour les 3 accélérations 
int y;
int z;

void setup()
{
 Serial.begin(9600); // Démarrer la liaison série
 accelero.begin(13, 12, 11, 10, A0, A1, A2); // Démarrer le composant
 accelero.setARefVoltage(5); // Régler la tension de référence
 accelero.setSensitivity(LOW); // Régler la sensibilité du composant +/-6G
 accelero.calibrate(); // Calibrer le composant
}

void loop()
{
 x = accelero.getXAccel(); // Lecture de l'axe X
 y = accelero.getYAccel(); // Lecture de l'axe Y
 z = accelero.getZAccel(); // Lecture de l'axe Z
 //Serial.print("nx: "); // Afficher la valeur de l'axe X
 Serial.print(x);
// Serial.print(" ");
// Serial.print(" \nty: "); // Afficher la valeur de l'axe Y
 /*Serial.print(y);
  Serial.print(" ");
 //Serial.print(" \ntz: "); // Afficher la valeur de l'axe Z
 Serial.print(z);
  Serial.print(" ");
 //Serial.println("\ntG*10^-2");
 */
 delay(500); // Délais pour rendre ça lisible
}
