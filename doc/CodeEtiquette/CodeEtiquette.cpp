float CodeEtiquette::aFunction(int firstVar, int secondVar){
	//If and forstatements
	//Put brackets in ALL if's statements, thing about the next generation
	if(something == true)
		for(int i=0;i<1000;i++)
			if(i%2==50)
				somethingElse++;
	//The above code is correct but is deficult to read from other.
	//This code is the same but is more readable
	if(something == true){
		for(int i=0;i<1000;i++){
			if(i%2==50){
				somethingElse++;
			}
		}
	}
	
	//DONT USE one line if statements (with some exceptions)!!!!!
	//this is bad:
	int variable = (something == true || (somethingElse == false && anotherThing == true)) ? firstFloat*secondFloat+doubleVar/atomsInUniverse : firstFloat/secondFloat+doubleVar/quarksInUniverse; 
	//In the other hand this is simply
	int variable = (something == true) ? 2 : 1;
	
	//Remember that this: 
	if(wim !=0){
		if(wim.get() != 0){
			if(wim.ball_size() > 0){
				doSomething();
			}
		}
	}
	//Is the same with this:
	if(wim !=0 && wim.get() != 0 && wim.ball_size() > 0){
		doSomething();
	}
	//SO DONT USE THE FIRST ONE
}


double CodeEtiquette::doubleFunctionForVectorsAdd(int firstVector[15], int secondVector[15]){
	//Remember to break you code in blocks
	
	//An if block
	if(something == true){
		for(int i=0;i<1000;i++){
			if(i%2==50){
				somethingElse++;
			}
		}
	}
	
	//A block that do some math's
	int one = 1;
	int two = 2;
	one *= two;
	two *= one/two;
	two += one*two;
	
	//A for loop block
	for(int i=0; i<100000; i++){
		two = two*two+one;
		one = i*i*one;
	}
	
	//An if block
	if(two%2 == 0){
		doSomething();
	}	
}
