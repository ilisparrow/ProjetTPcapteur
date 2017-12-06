
#include <AcceleroMMA7361.h>



/*Déclaration des PINS*/
int AIN1 =9; // vitesse moteur 1
int AIN2=6; // direction moteur 1
int BIN1=3; // vitesse moteur 2
int BIN2=5; //direction moteur 2

int XREF=6 ; // position initiale du capteur sur axe X
const int Kp=50; //facteur proportionnel ;
const float Ki=1; // facteur integrateur;
const int Kd=10 ; //facteur derivateur
int somme_erreur;
int variation_erreur;
int erreur_precedente=0;
float commande;
int erreur =0 ;
unsigned commande_moteur; 

//Declaration pour les capteur
AcceleroMMA7361 accelero; // Création du composant
int x; // Création des variables pour les 3 accélérations 
int y;
int z;
int nombreMoyenne = 5;
int moyenne;

void setup()
{
 Serial.begin(9600); // Démarrer la liaison série
 accelero.begin(13, 12, 11, 10, A0, A1, A2); // Démarrer le composant
 accelero.setARefVoltage(5); // Régler la tension de référence
 accelero.setSensitivity(LOW); // Régler la sensibilité du composant +/-6G
 accelero.calibrate(); // Calibrer le composant


 
 pinMode(AIN1, OUTPUT);
 pinMode(AIN2, OUTPUT);
 pinMode(BIN1, OUTPUT);
 pinMode(BIN2, OUTPUT);
 
}

void loop()
{
  moyenne = 0;
   for(int i=0;i<nombreMoyenne; i++)
 {  
    moyenne += accelero.getXAccel(); // Lecture de l'axe X
  }
    x = moyenne/nombreMoyenne;
    erreur = XREF - x;
    somme_erreur += erreur;
    variation_erreur = erreur - erreur_precedente;
    commande = Kp * erreur + Ki * somme_erreur + Kd * variation_erreur;
    erreur_precedente = erreur;

Serial.print("\nerreur_:");
Serial.print(erreur);



commande_moteur = map (commande, -1500, 1500, 90, 200); //après avoir affiché la commande on voit bien qu'elle va de -1500 jusqu'à 1500

if ( commande < 0 )
{
  analogWrite(AIN1, commande_moteur);
  digitalWrite(AIN2, 0);
  analogWrite(BIN1, commande_moteur);
  digitalWrite(BIN2, 0);  
  
}
else if (commande > 0)
{
  analogWrite(AIN2, commande_moteur);
  digitalWrite(AIN1, 0);
  analogWrite(BIN2, commande_moteur);
  digitalWrite(BIN1, 0); 
}




 
 delay(50); // Délais pour rendre ça lisible
 
}



