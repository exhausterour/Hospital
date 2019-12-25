/*
  Author: Anthony Garcia
  Email: anthony2018@my.fit.edu
  Course: CSE 2010
  Section: 02
  Description: This program creates a schedule for the arrival of patients, depending on their esi value, and the arrival of doctors, depending on their time value.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//stucture for the patients array
typedef struct patients{
	unsigned short time;
	char *name_patient;
	unsigned short esi;
}PATIENTS;

//structure for the doctors array
typedef struct doctors{
	unsigned short time;
	char *name_doctor;
	char *name_patient;
}DOCTORS;

//structures for patients priority queue
typedef struct pqueue_patient{
	PATIENTS priority_patients[200];
	short size;
}PQ_PATIENTS;

//structure for doctors priority queue
typedef struct pqueue_doctors{
	DOCTORS priority_doctors[50];
 	short size;
}PQ_DOCTORS;

//structure for pointing the doctors that are working
typedef struct working_doctors{
	char *name_doctor;
	char *name_patient;
	short time;
	struct working_doctors *next;
}WORK_DOCTORS;

/*receives the head of the priority queue, the time, name, and esi value. Uses the esi value to choose which one will be in the first position. I only use the esi value for deciding 
 which one will be a in a lower rank and not the time becuase when i read a line a do an action. The function inserts a value into the priority queue in the correct position*/
void insert_patient(PQ_PATIENTS *pq_patients, unsigned short time, char name[], unsigned short esi);
/*receives the head from the linked list, priority queue, time, name of the doctor, and name of the patient. The function inserts a value into the priority queue in the correct position using time*/
void insert_doctor(WORK_DOCTORS **injob_doctors, PQ_DOCTORS *pq_doctors, unsigned short time, char *name_doctor, char *name_patient);
/*receives the file address, the head of the linked list and the head of both priority queues. Creates the schedule while reading from the file. It's the function that calls all the other functions.*/
void create_schedule(FILE *f_pointer, WORK_DOCTORS **injob_doctors, PQ_DOCTORS *pq_doctors, PQ_PATIENTS *pq_patients);
/*receives the head of the doctors priority queue, and two locations. The function swaps both locations*/
void swap_doctors(PQ_DOCTORS *pq_doctors, int loc_1, int loc_2);
/*receives the head of the patient priority queue, and two locations. The function swaps both locations*/
void swap_patients(PQ_PATIENTS *pq_patients, int loc_2, int loc_1);
/*the function returns the size of doctor's priority queue. receives the head of the priority queue.*/
int empty_doctors(PQ_DOCTORS *pq_doctors);
/*the function returns the size of patient's priority queue. receives the head of the priority queue.*/
int empty_patients(PQ_PATIENTS *pq_patients);
/*receives the head of the linked list and the values that are going to be saved. The function saves the values in ascending order, using time, in the linked list*/
void add_working_doctor(WORK_DOCTORS **injob_doctors, DOCTORS *doctor);
/*returns the name of the doctor. receives the head of the doctors priority queue. removes the doctor with the lowest time from the priority queue*/
char* remove_doctor(PQ_DOCTORS *pq_doctors);
/*receives the head of the patient priority queue. removes the patient with the lowest time and esi(but it only uses the esi for comparing) from the priority queue*/
void remove_patient(PQ_PATIENTS *pq_patients);
/*returns a time value. receives to time values and returns the maximum time value between the two given*/
short max_time(short time1, short time2);
/*receives the head of the linked list. removes the first position of the linked list*/
void remove_working_doctor(WORK_DOCTORS **injob_doctors);
/*return the exponent value. receives the base and the exponent. the function computes the value recursively*/
int exponent(int base, int expo);
/*returns the correct time format. receives the time value and checks if the format is correct. If its not correct it corrects it*/
int time_correction(int time);

int main(int argc, char* argv[])
{
  /*
    description of declared variables, with meaningful names
   */
	PQ_DOCTORS doctors;
	doctors.size = -1;

	PQ_PATIENTS patients;
	patients.size = -1;

	WORK_DOCTORS *injob_doctors = NULL;

	FILE *file_pointer = fopen(argv[argc-1], "r");
	create_schedule(file_pointer, &injob_doctors, &doctors, &patients);
	fclose(file_pointer);

  return 0;
}

void add_working_doctor(WORK_DOCTORS **injob_doctors, DOCTORS *doctor){
	//create a new node using mallo and give it the desire values
	WORK_DOCTORS *new_node = (WORK_DOCTORS*)malloc(sizeof(WORK_DOCTORS));
	new_node->name_doctor = doctor->name_doctor;
	new_node->name_patient = doctor->name_patient;
	new_node->time = doctor->time;
	new_node->next = NULL;
	//if the head is null then make the new node the head
	if (*injob_doctors == NULL)
		*injob_doctors = new_node;
	//if the next position is null check which one is bigger between the head and the new node
	else if ((*injob_doctors)->next == NULL){
		//for adding the new node before the head
		if ((*injob_doctors)->time > doctor->time){
			new_node->next = *injob_doctors;
			*injob_doctors = new_node;
		}
		//adding the new node after the head
		else
			(*injob_doctors)->next = new_node;
	}
	else{
		//if the time of the head is bigger than the new node then add the new node as the head
		if ((*injob_doctors)->time > doctor->time){
			new_node->next = *injob_doctors;
			*injob_doctors = new_node;
		}
		else{
			//create a temp node that moves, in the worst case scenario, through the whole loop
			WORK_DOCTORS *temp_node = *injob_doctors;
			//if the position after next is null then stop the loop, also if the time of the new node is smaller than the temp next stop the loop
			while(temp_node->next->next != NULL && temp_node->next->time <= doctor->time)
				temp_node = temp_node->next;
			//if the time of the new node is less than the temp next then add it befor the temp next
			if (temp_node->next->time > doctor->time){
				new_node->next = temp_node->next;
				temp_node->next = new_node;				
			}
			//if not then add it at the end
			else{
				temp_node->next->next = new_node;
			}
		}
	}
}

int time_correction(int time){
	//if the mod of the time is bigger then 60 then change the hour section from a to a+1 and change the minute section from b to b-60
	if (time%100 >= 60){
		//1167 = 1167/100 = 11+1 = 12*100 = 1200 + (1167%100 - 60) = 1200 + (67-60) = 1200 + 7 = 1207
		time = ((int)(time/100) + 1)*100 + (time%100 - 60);
		return time;
	}
	//everything is correct
	return time;
}

//return the value of the size (-1 if empty)
int empty_patients(PQ_PATIENTS *pq_patients){
	return pq_patients->size;
}

//return the value of the size (-1 if empty)
int empty_doctors(PQ_DOCTORS *pq_doctors){
	return pq_doctors->size;
}

//recursively do the exponent value for integers
int exponent(int base, int expo){
	//base case
	if (expo == 1)
		return base;
	//recursive calling
	return base * exponent(base, expo - 1);
}

void create_schedule(FILE *f_pointer, WORK_DOCTORS **injob_doctors, PQ_DOCTORS *pq_doctors, PQ_PATIENTS *pq_patients){
	//variables used for reading from the file and saving the name of the doctor in service and the patient
	char action[15];
	char *name;
	unsigned short time = 0;
	unsigned short esi = 0;
	char *name_patient;
	char *name_doctor;
	short time_patient;
	short esi_patient;

	do{
		//makes a memory space for the name and then read from the file and save it in action
		name = (char*)malloc(15*sizeof(char));
		fscanf(f_pointer, "%s", action);
		//if the name matches with the action and the file is not at then keep reading the necessary values
		if(strcmp(action, "doctorArrives") == 0 && feof(f_pointer) == 0){
			fscanf(f_pointer,"%hu %s", &time, name);
			//if at least one of the priority queue and the linked least is empty then print the arrival and save it to the priority queue 
			if ((empty_doctors(pq_doctors) == -1 || empty_patients(pq_patients) == -1) && *injob_doctors == NULL){
				printf("%s %.4hu %s\n", action, time, name);
				insert_doctor(&(*injob_doctors), pq_doctors, time, name, NULL);
			}
			else{
				while(1){
					/*if the head is not null and the time read is bigger than the end_time of the working doctor and either the patients priority queue is empty or the max_time 
					between the min doctor and the min patient is less than the end of work of the doctor, finish the treatment first and then save the value read in the file*/
					if (*injob_doctors != NULL && time > (*injob_doctors)->time && (empty_patients(pq_patients) == -1 || max_time(pq_doctors->priority_doctors[0].time, pq_patients->priority_patients[0].time) >= (*injob_doctors)->time)){
						printf("doctorFinishesTreatmentAndPatientDeparts %.4hu %s %s\n", (*injob_doctors)->time, (*injob_doctors)->name_doctor, (*injob_doctors)->name_patient);
						remove_working_doctor(&(*injob_doctors));
					}
					else if(empty_patients(pq_patients) != -1 && time > max_time(pq_doctors->priority_doctors[0].time, pq_patients->priority_patients[0].time)){
						//saving the values of the patient that is going to be removed
						name_patient = pq_patients->priority_patients[0].name_patient;
						time_patient = pq_patients->priority_patients[0].time;
						esi_patient = pq_patients->priority_patients[0].esi;
						remove_patient(pq_patients);
						//choose time as the biggest between the patient and the doctor
						time_patient = max_time(time_patient, pq_doctors->priority_doctors[0].time);
						printf("doctorStartsTreatingPatient %hu %s %s\n", time_patient, pq_doctors->priority_doctors[0].name_doctor, name_patient);
						//set at what time the doctor is going to finish
						time_patient = time_patient + exponent(2, 6 - esi_patient);
						//makes the time format correct if it's not correct
						time_patient = time_correction(time_patient);
						//remove doctor and give only the name
						insert_doctor(&(*injob_doctors), pq_doctors, time_patient, remove_doctor(pq_doctors), name_patient);	
					}
					else
						break;
				}
				printf("%s %.4hu %s\n", action, time, name);
				insert_doctor(&(*injob_doctors), pq_doctors, time, name, NULL);
			}
		}
		//if the name matches with the action and the file is not at then keep reading the necessary values
		else if(strcmp(action, "patientArrives") == 0 && feof(f_pointer) == 0){
			fscanf(f_pointer,"%hu %s %hu", &time, name, &esi);
			//if at least one of the priority queue and the linked least is empty then print the arrival and save it to the priority queue 
			if ((empty_doctors(pq_doctors) == -1 || empty_patients(pq_patients) == -1) && *injob_doctors == NULL){
				printf("%s %.4hu %s %hu\n", action, time, name, esi);
				insert_patient(pq_patients, time, name, esi);
			}
			else{
				//printf("alo1\n");
				while(1){
					/*if the head is not null and the time read is bigger than the end_time of the working doctor and either the patients priority queue is empty or the max_time 
					between the min doctor and the min patient is less than the end of work of the doctor, finish the treatment first and then save the value read in the file*/
					if (*injob_doctors != NULL && time > (*injob_doctors)->time && (empty_patients(pq_patients) == -1 || max_time(pq_doctors->priority_doctors[0].time, pq_patients->priority_patients[0].time) >= (*injob_doctors)->time)){
						printf("doctorFinishesTreatmentAndPatientDeparts %.4hu %s %s\n", (*injob_doctors)->time, (*injob_doctors)->name_doctor, (*injob_doctors)->name_patient);
						remove_working_doctor(&(*injob_doctors));
					}
					//if the patient list is not empty and the time read is less than the max time between the min doctor and the min patient then remove patient from priority queue and add it to the doctors priority
					else if(empty_patients(pq_patients) != -1 && time > max_time(pq_doctors->priority_doctors[0].time, pq_patients->priority_patients[0].time)){
						//saving the values of the patient that is going to be removed
						name_patient = pq_patients->priority_patients[0].name_patient;
						time_patient = pq_patients->priority_patients[0].time;
						esi_patient = pq_patients->priority_patients[0].esi;
						remove_patient(pq_patients);
						//choose time as the biggest between the patient and the doctor
						time_patient = max_time(time_patient, pq_doctors->priority_doctors[0].time);
						printf("doctorStartsTreatingPatient %hu %s %s\n", time_patient, pq_doctors->priority_doctors[0].name_doctor, name_patient);
						//set at what time the doctor is going to finish
						time_patient = time_patient + exponent(2, 6 - esi_patient);
						//makes the time format correct if it's not correct
						time_patient = time_correction(time_patient);
						//remove doctor and give only the name and insert it back to the priority queue with the patient name
						insert_doctor(&(*injob_doctors), pq_doctors, time_patient, remove_doctor(pq_doctors), name_patient);
					}
					else
						break;
				}
				printf("%s %.4hu %s %hu\n", action, time, name, esi);
				insert_patient(pq_patients, time, name, esi);
			}
			
		}
	}while(!feof(f_pointer));


	while(1){
		/*if the head is not null and either the patients priority queue is empty or the max_time between the min doctor and the min patient is less 
		than the end of work of the doctor, finish the treatment first and then save the value read in the file*/
		if (*injob_doctors != NULL && ((empty_patients(pq_patients) == -1 || pq_doctors->priority_doctors[0].time >= (*injob_doctors)->time) || pq_patients->priority_patients[0].time >= (*injob_doctors)->time)){
			printf("doctorFinishesTreatmentAndPatientDeparts %.4hu %s %s\n", (*injob_doctors)->time, (*injob_doctors)->name_doctor, (*injob_doctors)->name_patient);
			remove_working_doctor(&(*injob_doctors));
		}
		//if the patient list is not empty
		else if(empty_patients(pq_patients) != -1){
			//saving the values of the patient that is going to be removed
			name_patient = pq_patients->priority_patients[0].name_patient;
			time_patient = pq_patients->priority_patients[0].time;
			esi_patient = pq_patients->priority_patients[0].esi;
			remove_patient(pq_patients);
			//choose time as the biggest between the patient and the doctor
			time_patient = max_time(time_patient, pq_doctors->priority_doctors[0].time);
			printf("doctorStartsTreatingPatient %hu %s %s\n", time_patient, pq_doctors->priority_doctors[0].name_doctor, name_patient);
			//choose time as the biggest between the patient and the doctor
			time_patient = time_patient + exponent(2, 6 - esi_patient);
			//makes the time format correct if it's not correct
			time_patient = time_correction(time_patient);
			//remove doctor and give only the name and insert it back to the priority queue with the patient name
			insert_doctor(&(*injob_doctors), pq_doctors, time_patient, remove_doctor(pq_doctors), name_patient);
		}
		else
			break;
	}
}

void remove_working_doctor(WORK_DOCTORS **injob_doctors){
	//use a temp to save the position after the head
	WORK_DOCTORS *temp_node = (*injob_doctors)->next;
	//set name doctor, name patient and next to NULL and then free the head
	(*injob_doctors)->name_doctor = NULL;
	(*injob_doctors)->name_patient = NULL;
	(*injob_doctors)->next = NULL;
	free(*injob_doctors);
	//at the end set the head to point to the temp_node
	*injob_doctors = temp_node;
}

short max_time(short time1, short time2){
	//if the difference is positive or 0 then time 1 is bigger or time1 equals time2 therefore return time
	if (time1 - time2 >= 0)
		return time1;
	//if it's negative then return time2
	return time2;
}

void swap_doctors(PQ_DOCTORS *pq_doctors, int loc_1, int loc_2){
	int temp_time = 0;
	char *name_temp;

	//swap values of time with temp_time
	temp_time = pq_doctors->priority_doctors[loc_1].time;
	pq_doctors->priority_doctors[loc_1].time = pq_doctors->priority_doctors[loc_2].time;
	pq_doctors->priority_doctors[loc_2].time = temp_time;	

	//swap doctor name pointers using name_temp
	name_temp = pq_doctors->priority_doctors[loc_1].name_patient;
	pq_doctors->priority_doctors[loc_1].name_patient = pq_doctors->priority_doctors[loc_2].name_patient;
	pq_doctors->priority_doctors[loc_2].name_patient = name_temp;

	//swap patient name pointers using name_temp
	name_temp = &(*(pq_doctors->priority_doctors[loc_1].name_doctor));
	pq_doctors->priority_doctors[loc_1].name_doctor = &(*(pq_doctors->priority_doctors[loc_2].name_doctor));
	pq_doctors->priority_doctors[loc_2].name_doctor = &(*name_temp);
}


void insert_doctor(WORK_DOCTORS **injob_doctors, PQ_DOCTORS *pq_doctors, unsigned short time, char *name_doctor, char *name_patient){
	//this variable are the location of the child and parent
	int child_loc = 0;
	int parent_loc = 0;

	//changing the size of the array since its an insertion, initializing the positions of the child and parent 
	(pq_doctors->size)++;
	child_loc = pq_doctors->size;
	parent_loc = child_loc/2;

	//loop until you get to the root
	while(parent_loc != 0){
		//if the time of the patient, that it's going to be inserted, is bigger than or equal to the time of the parent the add it as a child and end the function
		if (time >= pq_doctors->priority_doctors[parent_loc].time){
			pq_doctors->priority_doctors[child_loc].time = time;
			pq_doctors->priority_doctors[child_loc].name_patient = name_patient;
			pq_doctors->priority_doctors[child_loc].name_doctor = name_doctor;
			//if the value of the name is not NULL then add it to working doctor sorted linked list
			if (pq_doctors->priority_doctors[child_loc].name_patient != NULL)
				add_working_doctor(&(*injob_doctors), &(pq_doctors->priority_doctors[child_loc]));
			return;
		}
		//make the parent be in the position of his child and then change the child and parent
		else{
			pq_doctors->priority_doctors[child_loc].time = pq_doctors->priority_doctors[parent_loc].time;
			pq_doctors->priority_doctors[child_loc].name_patient = pq_doctors->priority_doctors[parent_loc].name_patient;
			pq_doctors->priority_doctors[child_loc].name_doctor = pq_doctors->priority_doctors[parent_loc].name_doctor;
			//child becomes the parent and parent keeps being the parent of the child
			child_loc = parent_loc;
			parent_loc = child_loc/2;
		}
	}
	//if we are in the position below the root then add the value to be its child and then swap them if the child has a smaller value
	pq_doctors->priority_doctors[child_loc].time = time;
	pq_doctors->priority_doctors[child_loc].name_patient = name_patient;
	pq_doctors->priority_doctors[child_loc].name_doctor = name_doctor;
	if (parent_loc == 0 && pq_doctors->priority_doctors[child_loc].time < pq_doctors->priority_doctors[parent_loc].time){
		swap_doctors(pq_doctors, child_loc, parent_loc);
		//if the value of the name is not NULL then add it to working doctor sorted linked list
		if (pq_doctors->priority_doctors[parent_loc].name_patient != NULL)
			add_working_doctor(&(*injob_doctors), &(pq_doctors->priority_doctors[parent_loc]));
		return;
	}
	//if the value of the name is not NULL then add it to working doctor sorted linked list
	if (pq_doctors->priority_doctors[child_loc].name_patient != NULL)
		add_working_doctor(&(*injob_doctors), &(pq_doctors->priority_doctors[child_loc]));
}

char* remove_doctor(PQ_DOCTORS *pq_doctors){
	int child_loc = 0;
	int current_loc = 0;
	int itemto_place_loc = 0;
	char *name;

	//if there is nothing in the prioritu queue then end the function becuase there's nothing to remove
	if(empty_doctors(pq_doctors) == -1)
		return name;

	//save the location of the last item, change the size of the array since an item will be remove and set the location of the root and the first child;
	name = pq_doctors->priority_doctors[0].name_doctor;
	itemto_place_loc = pq_doctors->size;
	(pq_doctors->size)--;
	current_loc = 0;
	child_loc = 1;

	//while the location of the child is bigger or equal to the location keep looping
	while(child_loc <= pq_doctors->size){
		//if child is less than the size then check which of the two child is smaller. Then change to the smallest one.
		if (child_loc < pq_doctors->size){
			if (pq_doctors->priority_doctors[child_loc + 1].time < pq_doctors->priority_doctors[child_loc].time)
				child_loc++;
		}
		//if the child is bigger than the item i want to place then swap them
		if (pq_doctors->priority_doctors[child_loc].time >= pq_doctors->priority_doctors[itemto_place_loc].time)
			swap_doctors(pq_doctors, child_loc, itemto_place_loc);
		//else then save the child in the current position
		else{
			pq_doctors->priority_doctors[current_loc].time = pq_doctors->priority_doctors[child_loc].time;
			pq_doctors->priority_doctors[current_loc].name_patient = pq_doctors->priority_doctors[child_loc].name_patient;
			pq_doctors->priority_doctors[current_loc].name_doctor = pq_doctors->priority_doctors[child_loc].name_doctor;
			current_loc = child_loc;
			child_loc = 2*current_loc + 1;
		}
	}
	//at the end save the item to place in the last position which will always be the current loc at the end
	pq_doctors->priority_doctors[current_loc].time = pq_doctors->priority_doctors[itemto_place_loc].time;
	pq_doctors->priority_doctors[current_loc].name_patient = pq_doctors->priority_doctors[itemto_place_loc].name_patient;
	pq_doctors->priority_doctors[current_loc].name_doctor = pq_doctors->priority_doctors[itemto_place_loc].name_doctor;
	return name;
}

void remove_patient(PQ_PATIENTS *pq_patients){
	int child_loc = 0;
	int current_loc = 0;
	int itemto_place_loc = 0;
	char *name;

	//if there is nothing in the prioritu queue then end the function becuase there's nothing to remove
	if(empty_patients(pq_patients) == -1)
		return;

	//save the location of the last item, change the size of the array since an item will be remove and set the location of the root and the first child;
	itemto_place_loc = pq_patients->size;
	(pq_patients->size)--;
	current_loc = 0;
	child_loc = 1;

	//while the location of the child is bigger or equal to the location keep looping
	while(child_loc <= pq_patients->size){
		//if child is less than the size then check which of the two child is smaller. Then change to the smallest one.
		if (child_loc < pq_patients->size){
			if (pq_patients->priority_patients[child_loc + 1].esi < pq_patients->priority_patients[child_loc].esi)
				child_loc++;
		}
		//if the child is bigger than the item i want to place then swap them
		if (pq_patients->priority_patients[child_loc].esi >= pq_patients->priority_patients[itemto_place_loc].esi)
			swap_patients(pq_patients, child_loc, itemto_place_loc);
		//else then save the child in the current position
		else{
			pq_patients->priority_patients[current_loc].time = pq_patients->priority_patients[child_loc].time;
			pq_patients->priority_patients[current_loc].esi = pq_patients->priority_patients[child_loc].esi;
			pq_patients->priority_patients[current_loc].name_patient = pq_patients->priority_patients[child_loc].name_patient;
			current_loc = child_loc;
			child_loc = 2*current_loc + 1;
		}
	}

	//at the end save the item to place in the last position which will always be the current loc at the end
	pq_patients->priority_patients[current_loc].time = pq_patients->priority_patients[itemto_place_loc].time;
	pq_patients->priority_patients[current_loc].esi = pq_patients->priority_patients[itemto_place_loc].esi;
	pq_patients->priority_patients[current_loc].name_patient = pq_patients->priority_patients[itemto_place_loc].name_patient;
}

void swap_patients(PQ_PATIENTS *pq_patients, int loc_2, int loc_1){
	char *name = NULL;
	unsigned short num_temp = 0;

	//swap time using num_temp
	num_temp = pq_patients->priority_patients[loc_2].time;
	pq_patients->priority_patients[loc_2].time = pq_patients->priority_patients[loc_1].time;
	pq_patients->priority_patients[loc_1].time = num_temp;

	//swap patient name pointers using name
	name = pq_patients->priority_patients[loc_2].name_patient;
	pq_patients->priority_patients[loc_2].name_patient = pq_patients->priority_patients[loc_1].name_patient;
	pq_patients->priority_patients[loc_1].name_patient = name;

	//swap esi value using num_temp
	num_temp = pq_patients->priority_patients[loc_2].esi;
	pq_patients->priority_patients[loc_2].esi = pq_patients->priority_patients[loc_1].esi;
	pq_patients->priority_patients[loc_1].esi = num_temp;
}

void insert_patient(PQ_PATIENTS *pq_patients, unsigned short time, char name[], unsigned short esi){
	//this variable are the location of the child and parent
	int child_loc = 0;
	int parent_loc = 0;

	//changing the size of the array since its an insertion, initializing the positions of the child and parent 
	(pq_patients->size)++;
	child_loc = pq_patients->size;
	parent_loc = child_loc/2;

	//loop until you get to the root
	while(parent_loc != 0){
		//if the esi value, of the patient that it's going to be inserted, is bigger than or equal to the esi value of the parent the add it as a child and end the function
		if (esi >= pq_patients->priority_patients[parent_loc].esi){
			pq_patients->priority_patients[child_loc].time = time;
			pq_patients->priority_patients[child_loc].name_patient = name;
			pq_patients->priority_patients[child_loc].esi = esi;
			return;
		}
		//make the parent be in the position of his child and then change the child and parent
		else{
			pq_patients->priority_patients[child_loc].time = pq_patients->priority_patients[parent_loc].time;
			pq_patients->priority_patients[child_loc].name_patient = pq_patients->priority_patients[parent_loc].name_patient;
			pq_patients->priority_patients[child_loc].esi = pq_patients->priority_patients[parent_loc].esi;
			//child becomes the parent and parent keeps being the parent of the child
			child_loc = parent_loc;
			parent_loc = child_loc/2;
		}
	}

	//if we are in the position below the root then add the value to be its child and then swap them if the child has a smaller value
	pq_patients->priority_patients[child_loc].time = time;
	pq_patients->priority_patients[child_loc].name_patient = name;
	pq_patients->priority_patients[child_loc].esi = esi;
	if (parent_loc == 0 && pq_patients->priority_patients[child_loc].esi < pq_patients->priority_patients[parent_loc].esi)
		swap_patients(pq_patients, child_loc, parent_loc);
}
