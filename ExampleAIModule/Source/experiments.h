/*
 Copyright 2001 The University of Texas at Austin

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include "neat.h"
#include "network.h"
#include "population.h"
#include "organism.h"
#include "genome.h"
#include "species.h"

using namespace std;

using namespace NEAT;

//The XOR evolution routines *****************************************
Population *xor_test(int gens);
bool xor_evaluate(Organism *org);
int xor_epoch(Population *pop,int generation,char *filename, int &winnernum, int &winnergenes,int &winnernodes);

//Single pole balancing evolution routines ***************************
Population *pole1_test(int gens); 
bool pole1_evaluate(Organism *org);
int pole1_epoch(Population *pop,int generation,char *filename);
int go_cart(Network *net,int max_steps,int thresh); //Run input
//Move the cart and pole
void cart_pole(int action, float *x,float *x_dot, float *theta, float *theta_dot);

//Double pole balacing evolution routines ***************************
class CartPole;

Population *pole2_test(int gens,int velocity);
bool pole2_evaluate(Organism *org,bool velocity,CartPole *thecart);
int pole2_epoch(Population *pop,int generation,char *filename,bool velocity, CartPole *thecart,int &champgenes,int &champnodes, int &winnernum, ofstream &oFile);

class CartPole {
public:
  CartPole(bool randomize,bool velocity);
  virtual void simplifyTask();  
  virtual void nextTask();
  virtual double evalNet(Network *net,int thresh);
  double maxFitness;
  bool MARKOV;

  bool last_hundred;
  bool nmarkov_long;  //Flag that we are looking at the champ
  bool generalization_test;  //Flag we are testing champ's generalization

  double state[6];

  double jigglestep[1000];

protected:
  virtual void init(bool randomize);

private:

  void performAction(double output,int stepnum);
  void step(double action, double *state, double *derivs);
  void rk4(double f, double y[], double dydx[], double yout[]);
  bool outsideBounds(); 

  static const int NUM_INPUTS = 7;
  double MUP;			//static const double MUP=0.000002;
  double MUC;
  double GRAVITY;
  double MASSCART;
  double MASSPOLE_1;

  double LENGTH_1;		  /* actually half the pole's length */

  double FORCE_MAG;
  double TAU;		  //seconds between state updates 

  double one_degree;	/* 2pi/360 */
  double six_degrees;
  double twelve_degrees;
  double fifteen_degrees;
  double thirty_six_degrees;
  double fifty_degrees;

  double LENGTH_2;
  double MASSPOLE_2;
  double MIN_INC;
  double POLE_INC;
  double MASS_INC;

  //Queues used for Gruau's fitness which damps oscillations
  int balanced_sum;
  double cartpos_sum;
  double cartv_sum;
  double polepos_sum;
  double polev_sum;



};

#endif






