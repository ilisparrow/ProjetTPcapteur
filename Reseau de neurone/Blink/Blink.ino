/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi

/******************************************************************
 * Network Configuration - customized per network
 ******************************************************************/
 #include <math.h>

 const int valeurCapteurAvantChuteAvant = 1;//a changer au moment de la calibration
 const int valeurCapteurAvantChuteArriere = 1;//a changer au moment de la calibration
 const int valeurCapteurAvantChuteAvantSURDEUX = 1;
 const int valeurCapteurAvantChuteArriereSURDEUX = 1;

 const int ValeurTensionCorrection = 1;//a changer au moment de la calibration



const int PatternCount = 4;//Nombre de "cas" a travailler.
const int InputNodes = 1;//nombre d'entrée, ici 1 capteur
const int HiddenNodes = 1;//Nombre de noeuds caché,aufmente le temps d'apprentissage mais aussi la complexité des problemes à resoudre. a prendre au moins égale au nombre de noeuds de sortie
const int OutputNodes = 1;//Nombre de noeuds de sortie, ici 1 car un seul actionneur
const float LearningRate = 0.5;//Par default 0,3, reduit le temps d'apprtissame mais permets de ne pas osciller autour d'un position d'équilibre
const float Momentum = 0.9;//adoucit l'apprentissage et permet d'eviter de trouner autour d'un état optimal local, en reintroduisant un erreur precedente.
const float InitialWeightMax = 0.5;//pour les valeurs de départ, devrait rester petites
const float Success = 0.0004;//a quel niveau d'erreur devons nous considerer que c'est suffisant.


//Les valeurss extremes recue en entrée
const byte Input[PatternCount][InputNodes] = {
  { valeurCapteurAvantChuteAvant },  //
  { valeurCapteurAvantChuteArriere},  //
  { valeurCapteurAvantChuteAvantSURDEUX},  //
  { valeurCapteurAvantChuteArriereSURDEUX}  //
};

const byte Target[PatternCount][OutputNodes] = {
  { ValeurTensionCorrection},
  { (-1)*ValeurTensionCorrection},
  { (0,70)*ValeurTensionCorrection },
  { (-0,70)*ValeurTensionCorrection  }
};

/******************************************************************
 * End Network Configuration
 ******************************************************************/


int i, j, p, q, r;
int ReportEvery100;
int RandomizedIndex[PatternCount];
long  TrainingCycle;
float Rando;
float Error;
float Accum;


float Hidden[HiddenNodes];
float Output[OutputNodes];
float HiddenWeights[InputNodes+1][HiddenNodes];
float OutputWeights[HiddenNodes+1][OutputNodes];
float HiddenDelta[HiddenNodes];
float OutputDelta[OutputNodes];
float ChangeHiddenWeights[InputNodes+1][HiddenNodes];
float ChangeOutputWeights[HiddenNodes+1][OutputNodes];

void setup(){
  Serial.begin(9600);
  randomSeed(analogRead(3));
  ReportEvery100 = 1;
  for( p = 0 ; p < PatternCount ; p++ ) {
    RandomizedIndex[p] = p ;
  }
}

void loop (){
entrainement();

}

void entrainement(){

  /******************************************************************
  * Initialize HiddenWeights and ChangeHiddenWeights
  ******************************************************************/

    for( i = 0 ; i < HiddenNodes ; i++ ) {
      for( j = 0 ; j <= InputNodes ; j++ ) {
        ChangeHiddenWeights[j][i] = 0.0 ;
        Rando = float(random(100))/100;
        HiddenWeights[j][i] = 2.0 * ( Rando - 0.5 ) * InitialWeightMax ;
      }
    }
  /******************************************************************
  * Initialize OutputWeights and ChangeOutputWeights
  ******************************************************************/

    for( i = 0 ; i < OutputNodes ; i ++ ) {
      for( j = 0 ; j <= HiddenNodes ; j++ ) {
        ChangeOutputWeights[j][i] = 0.0 ;
        Rando = float(random(100))/100;
        OutputWeights[j][i] = 2.0 * ( Rando - 0.5 ) * InitialWeightMax ;
      }
    }
    Serial.println("Initial/Untrained Outputs: ");
    toTerminal();
  /******************************************************************
  * Begin training
  ******************************************************************/

    for( TrainingCycle = 1 ; TrainingCycle < 2147483647 ; TrainingCycle++) {

  /******************************************************************
  * Randomize order of training patterns
  ******************************************************************/

      for( p = 0 ; p < PatternCount ; p++) {
        q = random(PatternCount);
        r = RandomizedIndex[p] ;
        RandomizedIndex[p] = RandomizedIndex[q] ;
        RandomizedIndex[q] = r ;
      }
      Error = 0.0 ;
  /******************************************************************
  * Cycle through each training pattern in the randomized order
  ******************************************************************/
      for( q = 0 ; q < PatternCount ; q++ ) {
        p = RandomizedIndex[q];

  /******************************************************************
  * Compute hidden layer activations
  ******************************************************************/

        for( i = 0 ; i < HiddenNodes ; i++ ) {
          Accum = HiddenWeights[InputNodes][i] ;
          for( j = 0 ; j < InputNodes ; j++ ) {
            Accum += Input[p][j] * HiddenWeights[j][i] ;
          }
          Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
        }

  /******************************************************************
  * Compute output layer activations and calculate errors
  ******************************************************************/

        for( i = 0 ; i < OutputNodes ; i++ ) {
          Accum = OutputWeights[HiddenNodes][i] ;
          for( j = 0 ; j < HiddenNodes ; j++ ) {
            Accum += Hidden[j] * OutputWeights[j][i] ;
          }
          Output[i] = 1.0/(1.0 + exp(-Accum)) ;
          OutputDelta[i] = (Target[p][i] - Output[i]) * Output[i] * (1.0 - Output[i]) ;
          Error += 0.5 * (Target[p][i] - Output[i]) * (Target[p][i] - Output[i]) ;
        }

  /******************************************************************
  * Backpropagate errors to hidden layer
  ******************************************************************/

        for( i = 0 ; i < HiddenNodes ; i++ ) {
          Accum = 0.0 ;
          for( j = 0 ; j < OutputNodes ; j++ ) {
            Accum += OutputWeights[i][j] * OutputDelta[j] ;
          }
          HiddenDelta[i] = Accum * Hidden[i] * (1.0 - Hidden[i]) ;
        }


  /******************************************************************
  * Update Inner-->Hidden Weights
  ******************************************************************/


        for( i = 0 ; i < HiddenNodes ; i++ ) {
          ChangeHiddenWeights[InputNodes][i] = LearningRate * HiddenDelta[i] + Momentum * ChangeHiddenWeights[InputNodes][i] ;
          HiddenWeights[InputNodes][i] += ChangeHiddenWeights[InputNodes][i] ;
          for( j = 0 ; j < InputNodes ; j++ ) {
            ChangeHiddenWeights[j][i] = LearningRate * Input[p][j] * HiddenDelta[i] + Momentum * ChangeHiddenWeights[j][i];
            HiddenWeights[j][i] += ChangeHiddenWeights[j][i] ;
          }
        }

  /******************************************************************
  * Update Hidden-->Output Weights
  ******************************************************************/

        for( i = 0 ; i < OutputNodes ; i ++ ) {
          ChangeOutputWeights[HiddenNodes][i] = LearningRate * OutputDelta[i] + Momentum * ChangeOutputWeights[HiddenNodes][i] ;
          OutputWeights[HiddenNodes][i] += ChangeOutputWeights[HiddenNodes][i] ;
          for( j = 0 ; j < HiddenNodes ; j++ ) {
            ChangeOutputWeights[j][i] = LearningRate * Hidden[j] * OutputDelta[i] + Momentum * ChangeOutputWeights[j][i] ;
            OutputWeights[j][i] += ChangeOutputWeights[j][i] ;
          }
        }
      }

  /******************************************************************
  * Every 100 cycles send data to terminal for display
  ******************************************************************/
      ReportEvery100 = ReportEvery100 - 1;
      if (ReportEvery100 == 0)
      {
        Serial.println();
        Serial.println();
        Serial.print ("TrainingCycle: ");
        Serial.print (TrainingCycle);
        Serial.print ("  Error = ");
        Serial.println (Error, 5);

        toTerminal();

        if (TrainingCycle==1)
        {
          ReportEvery100 = 99;
        }
        else
        {
          ReportEvery100 = 100;
        }
      }


  /******************************************************************
  * If error rate is less than pre-determined threshold then end
  ******************************************************************/

      if( Error < Success ) break ;
    }
    Serial.println ();
    Serial.println();
    Serial.print ("TrainingCycle: ");
    Serial.print (TrainingCycle);
    Serial.print ("  Error = ");
    Serial.println (Error, 5);

    toTerminal();

    Serial.println ();
    Serial.println ();
    Serial.println ("Training Set Solved! ");
    Serial.println ("--------");
    Serial.println ();
    Serial.println ();
    ReportEvery100 = 1;

}
void toTerminal()
{

  for( p = 0 ; p < PatternCount ; p++ ) {
    Serial.println();
    Serial.print ("  Training Pattern: ");
    Serial.println (p);
    Serial.print ("  Input ");
    for( i = 0 ; i < InputNodes ; i++ ) {
      Serial.print (Input[p][i], DEC);
      Serial.print (" ");
    }
    Serial.print ("  Target ");
    for( i = 0 ; i < OutputNodes ; i++ ) {
      Serial.print (Target[p][i], DEC);
      Serial.print (" ");
    }
/******************************************************************
* Compute hidden layer activations
******************************************************************/

    for( i = 0 ; i < HiddenNodes ; i++ ) {
      Accum = HiddenWeights[InputNodes][i] ;
      for( j = 0 ; j < InputNodes ; j++ ) {
        Accum += Input[p][j] * HiddenWeights[j][i] ;
      }
      Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
    }

/******************************************************************
* Compute output layer activations and calculate errors
******************************************************************/

    for( i = 0 ; i < OutputNodes ; i++ ) {
      Accum = OutputWeights[HiddenNodes][i] ;
      for( j = 0 ; j < HiddenNodes ; j++ ) {
        Accum += Hidden[j] * OutputWeights[j][i] ;
      }
      Output[i] = 1.0/(1.0 + exp(-Accum)) ;
    }
    Serial.print ("  Output ");
    for( i = 0 ; i < OutputNodes ; i++ ) {
      Serial.print (Output[i], 5);
      Serial.print (" ");
    }
  }


}

void InputToOutput(float In1, float In2, float In3, float In4)
{
  float TestInput[] = {0, 0, 0, 0};
  TestInput[0] = In1;
  TestInput[1] = In2;
  TestInput[2] = In3;
  TestInput[3] = In4;

  /******************************************************************
    Compute hidden layer activations
  ******************************************************************/

  for ( i = 0 ; i < HiddenNodes ; i++ ) {
    Accum = HiddenWeights[InputNodes][i] ;
    for ( j = 0 ; j < InputNodes ; j++ ) {
      Accum += TestInput[j] * HiddenWeights[j][i] ;
    }
    Hidden[i] = 1.0 / (1.0 + exp(-Accum)) ;
  }

  /******************************************************************
    Compute output layer activations and calculate errors
  ******************************************************************/

  for ( i = 0 ; i < OutputNodes ; i++ ) {
    Accum = OutputWeights[HiddenNodes][i] ;
    for ( j = 0 ; j < HiddenNodes ; j++ ) {
      Accum += Hidden[j] * OutputWeights[j][i] ;
    }
    Output[i] = 1.0 / (1.0 + exp(-Accum)) ;
  }
#ifdef DEBUG
  SerialUSB.print ("  Output ");
  for ( i = 0 ; i < OutputNodes ; i++ ) {
    SerialUSB.print (Output[i], 5);
    SerialUSB.print (" ");
  }
#endif
}
