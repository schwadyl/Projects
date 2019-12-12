#include "AI.h"
#include <cassert>

// returns true if there is an idle elevator in the building
bool checkServicing(const BuildingState& buildingState);
//returns true if there is a 789 anger level on current floor of an idle elevator
bool currentFloorPickup(const BuildingState& buildingState);
//returns index of elevator that needs to make immediate pickup
int getCurrentFPickupMove(const BuildingState& buildingState);
// returns true if there is a 789 anger level in the building
bool checkHighAngerBuilding(const BuildingState& buildingState);
//returns true if there is a 56 anger level in the building
bool checkMediumAngerBuilding(const BuildingState& buildingState);
//returns floor number with highest anger level
int floorToPickUp(const BuildingState& buildingState);
// Choosest closest idle elevator to send when service move is needed
int elevatorToSend(const BuildingState& buildingState, int floorToPickUp);
// returns string of everyone going in same direction as person with highest anger
string AIpickupList (const Move& move, const BuildingState& buildingState, const Floor& floorToPickup);
// returns true if there is a 1234 anger level on floor of an idle elevator
bool lowCurrentFloorPickup(const BuildingState& buildingState);

string getAIMoveString(const BuildingState& buildingState) {
	string aIPickupMove = "e-p";
    string aIServiceMove = "e-f-";
    
	if (checkServicing(buildingState)) {

		aIPickupMove = "";

		cout << "Otis: All elevators are busy, executing pass move" << endl;

		return aIPickupMove;
	}
	// if 56789 on current floor of idle elevator pick up now
    if (currentFloorPickup(buildingState)) {
		int eNum = getCurrentFPickupMove(buildingState);
		aIPickupMove[1] = eNum + 48;
		cout << "Otis: Person with high anger level, executing immediate pickup for elevator " << eNum << endl;
		return aIPickupMove;
	}
   // if 789 somehwere in building send closest idle elevator
    if (checkHighAngerBuilding(buildingState)){
        int fNum = floorToPickUp(buildingState);
        int eNum = elevatorToSend(buildingState, fNum);
        aIServiceMove[1] = eNum + 48;
        aIServiceMove[3] = fNum + 48;
        cout << "Otis: Person with dangerously high anger level on floor "
                        << fNum << ", executing service move for elevator " << eNum << endl;
        return aIServiceMove;
    }
    // if 56 somehwere in building send closest idle elevator
    if (checkMediumAngerBuilding(buildingState)){
        int fNum = floorToPickUp(buildingState);
        int eNum = elevatorToSend(buildingState, fNum);
        aIServiceMove[1] = eNum + 48;
        aIServiceMove[3] = fNum + 48;
        cout << "Otis: Person with medium anger level on floor " << fNum
                        << ", executing service move for elevator " << eNum << endl;
        return aIServiceMove;
    }
    //if 1234 on current floor of elevator
    if (lowCurrentFloorPickup(buildingState)){
        int eNum = getCurrentFPickupMove(buildingState);
        aIPickupMove[1] = eNum + 48;
        cout << "Otis: Person with low anger level, executing immediate pickup for elevator "
                                                                << eNum << endl;
        return aIPickupMove;
    }
    // servicing to people with low anger (last priority)
    int fNum = floorToPickUp(buildingState);
    int eNum = elevatorToSend(buildingState, fNum);
    aIServiceMove[1] = eNum + 48;
    aIServiceMove[3] = fNum + 48;
    cout << "Otis: Person with low anger level on floor " << fNum
                                        << ", executing service move for elevator " << eNum << endl;
    return aIServiceMove;
}

string getAIPickupList(const Move& move, const BuildingState& buildingState, const Floor& floorToPickup) {
    string list = AIpickupList(move, buildingState, floorToPickup);
	cout << "Otis: Picking up the following people: " << list << endl;
    return list;
}

bool checkServicing(const BuildingState& buildingState){
    bool allServicing = true;
    for (int i = 0; i < NUM_ELEVATORS; i++){
        _Elevator temp = buildingState.elevators[i];
        if (!temp.isServicing){
            allServicing = false;
        }
    }
    return allServicing;
}

bool currentFloorPickup(const BuildingState& buildingState) {
	
	bool pickupCurrentFloor = false;
	for(int i = 0; i < NUM_ELEVATORS; i++) {
			_Elevator tempElevator = buildingState.elevators[i];
			if (!tempElevator.isServicing) {
				int currentFloor = tempElevator.currentFloor;
				_Floor tempFloor = buildingState.floors[currentFloor];
				int numPeople = tempFloor.numPeople;
				for (int j = 0; j < numPeople; j++) {
					_Person tempPerson = tempFloor.people[j];
					//GoBACK TO DECISION 6 OR 6
                    if (tempPerson.angerLevel > 4) {
						pickupCurrentFloor = true;
					}
				}
			}
	}
	return pickupCurrentFloor;
}

bool lowCurrentFloorPickup(const BuildingState& buildingState) {

	bool pickupCurrentFloor = false;
	for (int i = 0; i < NUM_ELEVATORS; i++) {
		_Elevator tempElevator = buildingState.elevators[i];
		if (!tempElevator.isServicing) {
			int currentFloor = tempElevator.currentFloor;
			_Floor tempFloor = buildingState.floors[currentFloor];
			int numPeople = tempFloor.numPeople;
			for (int j = 0; j < numPeople; j++) {
				_Person tempPerson = tempFloor.people[j];
				//Anger level is between 1 and 5
				if (tempPerson.angerLevel >= 0 && tempPerson.angerLevel < 5) {
					pickupCurrentFloor = true;
				}
			}
		}
	}
	return pickupCurrentFloor;

}

int getCurrentFPickupMove(const BuildingState& buildingState) {
	int eIndex = -1;
    int eMaxAnger[NUM_ELEVATORS] = {-1,-1,-1};
    double eAvgAnger[NUM_ELEVATORS] = {-1,-1,-1};
	int sum[NUM_ELEVATORS] = { 0,0,0 };
	// gets max anger of a person on floors that have idle elevators
    for (int i = 0; i < NUM_ELEVATORS; i++) {
		_Elevator tempElevator = buildingState.elevators[i];
		if (!tempElevator.isServicing) {
			int currentFloor = tempElevator.currentFloor;
			_Floor tempFloor = buildingState.floors[currentFloor];
			int numPeople = tempFloor.numPeople;
			for (int j = 0; j < numPeople; j++) {
				_Person tempPerson = tempFloor.people[j];
                if (tempPerson.angerLevel > eMaxAnger[i]){
                    eMaxAnger[i] = tempPerson.angerLevel;
                }
				sum[i] += tempPerson.angerLevel;
			}
			eAvgAnger[i] = sum[i] / (double)numPeople;
		}
	}
	// gets highest of the maxes of the floors from earlier part
    int totalMax = -1;
		for (int k = 0; k < NUM_ELEVATORS; k++) {
			_Elevator tempElevator = buildingState.elevators[k];
			if (!tempElevator.isServicing) {
				if (eMaxAnger[k] > totalMax) {
					totalMax = eMaxAnger[k];
					eIndex = k;
				}
				// tie breaker if multiple floors have same max anger
                else if (eMaxAnger[k] == totalMax) {
					if (eAvgAnger[k] > eAvgAnger[eIndex]) {
						eIndex = k;
					}
				}
			}
		}
		
	return eIndex;
}

// returns if there is a person with anger level 7,8, or 9 in buliding
bool checkHighAngerBuilding(const BuildingState& buildingState) {
	bool pickupCurrentFloor = false;
	for (int i = 0; i < NUM_FLOORS; i++) {
		_Floor tempFloor = buildingState.floors[i];
		int numPeople = tempFloor.numPeople;
		for (int j = 0; j < numPeople; j++) {
			_Person tempPerson = tempFloor.people[j];
			if (tempPerson.angerLevel > 6) {
				pickupCurrentFloor = true;
			}
		}
		
	}
	return pickupCurrentFloor;
}

bool checkMediumAngerBuilding(const BuildingState& buildingState) {
    bool pickupCurrentFloor = false;
    for (int i = 0; i < NUM_FLOORS; i++) {
        _Floor tempFloor = buildingState.floors[i];
        int numPeople = tempFloor.numPeople;
        for (int j = 0; j < numPeople; j++) {
            _Person tempPerson = tempFloor.people[j];
            if (tempPerson.angerLevel < 7 && tempPerson.angerLevel > 4) {
                pickupCurrentFloor = true;
            }
        }
    }
    return pickupCurrentFloor;
}

int floorToPickUp(const BuildingState& buildingState){
    int fMaxAnger[NUM_FLOORS] = {0,0,0,0,0,0,0,0,0,0};
    int floorPickUp = -1;
    int floorSum[NUM_FLOORS] = {0,0,0,0,0,0,0,0,0,0};
    double floorAvg[NUM_FLOORS] = {0,0,0,0,0,0,0,0,0,0};
    // gets highest anger level of single person on each floor in building
    for (int i = 0; i < NUM_FLOORS; i++) {
        _Floor tempFloor = buildingState.floors[i];
        int numPeople = tempFloor.numPeople;
        for (int j = 0; j < numPeople; j++) {
            _Person tempPerson = tempFloor.people[j];
            if (tempPerson.angerLevel > fMaxAnger[i]){
                fMaxAnger[i] = tempPerson.angerLevel;
            }
            floorSum[i] += tempPerson.angerLevel;
        }
        floorAvg[i] = floorSum[i] / (double)numPeople;
    }
    // gets highest anger floor in the building
    int totalMax = -1;
    for (int k = 0; k < NUM_FLOORS; k++) {
        if (fMaxAnger[k] > totalMax) {
            totalMax = fMaxAnger[k];
            floorPickUp = k;
        }
        // tie breaker
        else if (fMaxAnger[k] == totalMax) {
            if (floorAvg[k] > floorAvg[floorPickUp]) {
                floorPickUp = k;
            }
        }
    }

    return floorPickUp;
}

int elevatorToSend(const BuildingState& buildingState, int floorToPickUp){
    int eIndex = -1;
    int minDiff = 20;
    for (int i = 0; i < NUM_ELEVATORS; i++){
        _Elevator tempE = buildingState.elevators[i];
        if (!tempE.isServicing){
            int currentFloor = tempE.currentFloor;
            if (abs(currentFloor - floorToPickUp) < minDiff){
                eIndex = i;
            }
        }
    }
    return eIndex;
}

string AIpickupList (const Move& move, const BuildingState& buildingState, const Floor& floorToPickup){
    string list = "";
    int maxAnger = -1;
    int pIndex = -1;
    for (int i = 0; i < floorToPickup.getNumPeople(); i++){
        Person temp = floorToPickup.getPersonByIndex(i);
        if (temp.getAngerLevel() > maxAnger){
            maxAnger = temp.getAngerLevel();
            pIndex = i;
        }
    }
    Person highAnger = floorToPickup.getPersonByIndex(pIndex);
    int direction = highAnger.getTargetFloor() - highAnger.getCurrentFloor();
    // pick up all people going up
    if (direction > 0){
        for (int j = 0; j < floorToPickup.getNumPeople(); j++){
            Person newTemp = floorToPickup.getPersonByIndex(j);
            if (newTemp.getTargetFloor() - newTemp.getCurrentFloor() > 0){
                list += j + 48;
            }
        }
    }
    //pick up all people going down
    else {
        for (int k = 0; k < floorToPickup.getNumPeople(); k++){
            Person newTemp = floorToPickup.getPersonByIndex(k);
            if (newTemp.getTargetFloor() - newTemp.getCurrentFloor() < 0){
                list += k + 48;
            }
        }
    }
    return list;
}
